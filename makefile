CC=gcc

SANDYBRIDGE=-march=sandybridge
IVYBRIDGE=-march=ivybridge
SKYLAKE=-march=skylake

CFLAGS=-std=c99 -Ofast -ffast-math -Wl,--strip-all,--build-id=none,--gc-sections -fno-ident -fomit-frame-pointer -Wall -Wextra -Wpedantic -static -mwindows
TARGET=ShaderScreenSaver
LIB=-lgdi32 -lscrnsave

default: gl

gl: screensaver_gl.c scrnsave.c
	$(CC) $^ -o $(TARGET)_gl.scr $(CFLAGS) $(LIB) $(SANDYBRIDGE) -lopengl32
