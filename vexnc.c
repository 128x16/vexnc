// gcc -O2 -march=native -o vexnc vexnc.c -lvncserver -lm

#include <rfb/rfb.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main(int argc, char const *argv[]) {
  if (argc < 3) {
    printf("usage: %s IMAGEFILE TITLE\n", argv[0]);
    return 0;
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

  rfbInitServer(rfbScreen);
  rfbRunEventLoop(rfbScreen, -1, FALSE);

  stbi_image_free(im);
  rfbScreenCleanup(rfbScreen);

  return 0;
}
