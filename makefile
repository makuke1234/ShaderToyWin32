CC=gcc

SANDYBRIDGE=-march=sandybridge
IVYBRIDGE=-march=ivybridge
SKYLAKE=-march=skylake

CFLAGS=-std=c99 -Ofast -ffast-math -Wl,--strip-all,--build-id=none,--gc-sections -fno-ident -fomit-frame-pointer -Wall -Wextra -Wpedantic -static -mwindows
TARGET=ShaderScreenSaver
LIB=-lgdi32 -lscrnsave

default: release_sandybridge release_ivybridge release_skylake

resource.o: resource.rc
	windres -i $^ $@

gl: screensaver_gl.c scrnsave.c
	$(CC) $^ -o $(TARGET)_gl.scr $(CFLAGS) $(LIB) $(SANDYBRIDGE) -lopengl32

release_sandybridge: screensaver.c scrnsave.c
	$(CC) $^ -o $(TARGET)_sandy.scr $(CFLAGS) $(LIB) $(SANDYBRIDGE)
release_ivybridge: screensaver.c scrnsave.c
	$(CC) $^ -o $(TARGET)_ivy.scr $(CFLAGS) $(LIB) $(IVYBRIDGE)
release_skylake: screensaver.c scrnsave.c
	$(CC) $^ -o $(TARGET).scr $(CFLAGS) $(LIB) $(SKYLAKE)