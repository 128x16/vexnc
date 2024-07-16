LIBS := $(shell pkg-config --cflags --libs libvncserver) -lm -Ivendor/

vexnc: vexnc.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	rm vexnc
