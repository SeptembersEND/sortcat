#!/bin/sh

cc main.c -lm \
	-Wall\
	$(pkg-config --cflags --libs sdl2) \
	-o a.out
