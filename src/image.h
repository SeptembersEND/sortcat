#ifndef SRC_IMAGE__H
#define SRC_IMAGE__H

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef INIT_COLOR
#define INIT_COLOR 255
#endif

/**
 * typedef struct image - Structure defining an image
 * @width:	pixel width of image
 * @height:	pixel height of image
 * @size:	total pixels in image
 * @depth:	max possible values for each pixel
 * @red:	pointer to red pixels
 * @green:	pointer to green pixels
 * @blue:	pointer to blue pixels
 */
typedef struct {
  size_t width;
  size_t height;
  size_t size;
  size_t depth;
  uint8_t *red;
  uint8_t *green;
  uint8_t *blue;
} Image;
/**
 * typedef struct color - defines a color
 */
typedef struct {
  // NOTE: some functions depend on the order of these variables!
  unsigned int r;
  unsigned int g;
  unsigned int b;
  // TODO: add capability to use alpha
  //unsigned int a;
} color;

enum SNAPSHOT_OPTIONS {
  DN_INC = 0b01,
};

#include "image.c"

#endif
// vim: shiftwidth=2 tabstop=2 expandtab
