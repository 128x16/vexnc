#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <rfb/rfb.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int logdirenabled = 0;
int fdlog = -1;
char logfn[64] = "";

int mkderp(const char *name) {
  int ret = mkdir(name, S_IRWXU | S_IRWXG | S_IRWXO);
  if (ret < 0 && errno == EEXIST) {
    return 0;
  }
  return ret;
}

int spewtxt(int fd, const char *fmt, ...) {
  va_list va;
  char buf[256] = {0xF0, 0x9F, 0xBF, 0xBF, '\n', 0};

  va_start(va, fmt);
  vsnprintf(buf + 5, sizeof(buf) - 5, fmt, va);
  va_end(va);

  size_t len = strlen(buf);
  buf[len++] = '\n';

  return write(fd, buf, len);
}

int spewchr(int fd, uint16_t chr) {
  char buf[4];
  int i = 0;
  if (chr < 0x80) {
    buf[i++] = chr;
  } else if (chr < 0x800) {
    buf[i++] = 0xC0 | chr >> 6;
    buf[i++] = 0x80 | chr & 0x3F;
  } else {
    buf[i++] = 0xE0 | chr >> 12;
    buf[i++] = 0x80 | (chr >> 6) & 0x3F;
    buf[i++] = 0x80 | chr & 0x3F;
  }
  return write(fd, buf, i);
}

int openlogfile() {
  char curfn[64];
  time_t now;
  time(&now);
  strftime(curfn, 64, "%Y-%m-%d_%H", localtime(&now));
  if (!strncmp(curfn, logfn, 64))
    return 0;

  int fd = open(curfn, O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (fd < 0) {
    perror("open logfile");
    return 1;
  }

  if (fdlog != -1 && close(fdlog) < 0) {
    perror("close logfile");
  }

  fdlog = fd;
  return 0;
}

void userofflog(rfbClientRec *cl) {
  if (openlogfile()) return;
  if (spewtxt(fdlog, "disconnect %s", cl->host) < 0)
    perror("spewtxt");
}

enum rfbNewClientAction useronlog(rfbClientRec *cl) {
  cl->clientGoneHook = userofflog;
  if (openlogfile()) return RFB_CLIENT_ACCEPT;
  if (spewtxt(fdlog, "connect %s", cl->host) < 0)
    perror("spewtxt");
  return RFB_CLIENT_ACCEPT;
}

void keylog(rfbBool down, rfbKeySym keySym, rfbClientRec* cl) {
  if (!down) return; // only log key downs
  if (openlogfile()) return; // ensure log file opened

  if (spewchr(fdlog, (uint16_t) keySym) < 0)
    perror("spewchr");
}

int main(int argc, char const *argv[]) {
  if (argc < 3) {
    printf("usage: %s IMAGEFILE TITLE [LOGDIR]\n", argv[0]);
    return 0;
  }

  if (argc >= 4) {
    if (mkderp(argv[3]) < 0) {
      perror("mkdir logdir");
      return 1;
    }
    if (chdir(argv[3]) < 0) {
      perror("chdir logdir");
      return 1;
    }
    logdirenabled = 1;
  }

  int imx, imy, imc;
  unsigned char *im = stbi_load(argv[1], &imx, &imy, &imc, 4);
  if (im == NULL) {
    printf("bad image: %s\n", stbi_failure_reason());
    return 1;
  }

  rfbScreenInfoPtr rfbScreen = rfbGetScreen(NULL, NULL, imx, imy, 8, 3, 4);
  rfbScreen->desktopName = argv[2] ? argv[2] : "";
  rfbScreen->frameBuffer = (char*)im;
  rfbScreen->alwaysShared = TRUE;
  if (logdirenabled) {
    rfbScreen->newClientHook = useronlog;
    rfbScreen->kbdAddEvent = keylog;
  }

  rfbInitServer(rfbScreen);
  rfbRunEventLoop(rfbScreen, -1, FALSE);

  stbi_image_free(im);
  rfbScreenCleanup(rfbScreen);

  return 0;
}
