#-*- mode:makefile-gmake; -*-
ROOT = $(shell pwd)
TARGET = mfgd

INCLUDE += -I$(ROOT)/src
SOURCES = $(wildcard $(ROOT)/src/*.c)

pkg-config = $(shell pkg-config --$(1) --static $(2))

OBJS = $(patsubst %.c,%.o,$(SOURCES))
CPPFLAGS = $(OPTCPPFLAGS)
SDL_LIBS = $(call pkg-config,libs,SDL_image) $(call pkg-configs,libs,sdl)
SDL_CFLAGS = $(call pkg-config,cflags,SDL_image) $(call pkg-configs,cflags,sdl)
LIBS = $(call pkg-config,libs,glfw3) $(call pkg-config,libs,glew) $(SDL_LIBS) $(OPTLIBS)
CFLAGS = -g -std=c99 $(call pkg-config,cflags,glfw3) $(call pkg-config,cflags,glew) $(SDL_CFLAGS) $(INCLUDE) -Wall -Werror $(OPTFLAGS) -O3

ifeq ($(shell uname),Darwin)
# OS X specific libs required by glfw3
# LIBS += -framework GLUT -framework OpenGL -framework Cocoa -framework IOKit
else
# TODO: support EGL?
endif

.DEFAULT_GOAL = all
all: $(TARGET)

clean:
	rm -rf $(OBJS) $(TARGET) $(TARGET).o $(TARGET).new

.PHONY: all clean

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@.new
	mv $@.new $@

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^
