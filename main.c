#include <stdio.h>
#include <time.h>
#include <stddef.h>

#define INIT_COLOR ~0

#include "src/lib.h"
#include "src/3d.h"


int main(void) {
  printf("[+] Started %ld\n", time(NULL));
  Image *img = NULL;

  // NEW(orig, import, "in.ppm");
  NEW(new, init, 512, 512, 255);
  // NEW(gb, sobelOperator, &orig);
  // NEW(gb, gaussianBlur, &orig);
  // quicksort(&img);
  // copyImg(img, &orig);


  ende = 1;
  snapshot(img);
  printf("[+] Ended %ld\n", time(NULL));
}

// vim: shiftwidth=2 tabstop=2 expandtab
