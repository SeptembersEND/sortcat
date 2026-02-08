#ifndef SRC_LIB__H
#define SRC_LIB__H

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef NEW
#define NEW(NAME, FUNC, ...)                                                   \
  Image NAME = FUNC(__VA_ARGS__);                                              \
  img        = &NAME;
#endif

#define UNUSED(x) (void)(x)

#include "image.h"

#endif
