#define INIT_COLOR 255
#include "src/lib.c"

uint8_t invert(uint8_t f, uint8_t s) { return 255 - f; }

#define NEW(NAME, FUNC, ...)                                                   \
  Image NAME = FUNC(__VA_ARGS__);                                              \
  img        = &NAME;

int main(void) {
  printf("[+] Started %ld\n", time(NULL));
  Image *img = NULL;
  NEW(new, init, 256, 256, 255);
  // NEW(orig, import, "in.ppm");
  // NEW(so, sobelOperator, &orig);
  // NEW(gb, gaussianBlur, &orig);
  // quicksort(&img);

  ende = 1;
  snapshot(img);
  printf("[+] Ended %ld\n", time(NULL));
}

// vim: shiftwidth=2 tabstop=2 expandtab
