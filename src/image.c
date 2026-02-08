/* Image Type Manipulation {{{ */
int isallocated(Image* i) {
  if (i->red == NULL || i->green == NULL || i->blue == NULL || i->size == 0) {
    printf("image->red\t== '%p'\n", (void *)i->red);
    printf("image->green\t== '%p'\n", (void *)i->green);
    printf("image->blue\t== '%p'\n", (void *)i->blue);
    printf("image->size\t== '%p'\n", (void *)i->size);
    return 1;
  }
  return 0;
}
/**
 * init() - Create an blank image.
 */
Image init(unsigned int w, unsigned int h, unsigned int d) {
  size_t size = w * h;
  Image img = {
    .width   =  w,
    .height  =  h,
    .size    =  size,
    .depth   =  d,
    .red     =  malloc(sizeof(uint8_t) * size),
    .green   =  malloc(sizeof(uint8_t) * size),
    .blue    =  malloc(sizeof(uint8_t) * size),
  };

  memset(img.red, INIT_COLOR, size);
  memset(img.green, INIT_COLOR, size);
  memset(img.blue, INIT_COLOR, size);

  return img;
}
/* }}} */

/* Export {{{ */
void export_p3(FILE* fp, Image *image) {
  assert(fp != NULL);
  assert(image != NULL);
  assert(isallocated(image) == 0);
  fprintf(fp, "P3\n%ld %ld\n%ld\n", image->width, image->height, image->depth);
  for (int x = 0; x < image->size; x++) {
    fprintf(fp, "%d %d %d\n", image->red[x], image->green[x], image->blue[x]);
  }
}

void export(FILE *fp, Image *image) {
  assert(fp != NULL);
  assert(image != NULL);
  assert(isallocated(image) == 0);

  // TODO: specify export type
  export_p3(fp, image);
}

#define FILEOUT	"out/test"
unsigned int snapshot_options = 0b0;
int ende                      = 0;
void snapshot(Image* image) {
  if (ende <= 0)
    return;
  assert(image != NULL);

  assert(isallocated(image) == 0);
  static unsigned int i = 0;
  if ((snapshot_options & DN_INC) == 1)
    i = 0;

  static char buf [128] = {0};
  snprintf(buf, 127, FILEOUT "%.5d.ppm", i++);
  FILE *fp = fopen(buf, "w");
  assert(fp != NULL);

  printf("[+] Exporting %s -- %ld*%ld %ld pixels\n", buf, image->width,
         image->height, image->size);
  export(fp, image);
  fclose(fp);
}
/* }}} */
/* Import {{{ */
#define IF_NEND ptr != end
unsigned int import_unum(uint8_t **ptr, uint8_t *end) {
  unsigned int out = 0;
  uint8_t *tptr = *ptr;
  while (isdigit(*tptr)) {
    out *= 10;
    out += (*tptr) - '0';
    tptr++;
    assert(tptr != end);
  }
  *ptr = tptr;
  return out;
}

uint8_t normal(unsigned int x, unsigned int) { return x; }
uint8_t (*rfilter)(unsigned int, unsigned int) = normal; // rtest;
uint8_t (*bfilter)(unsigned int, unsigned int) = normal; // btest;
uint8_t (*gfilter)(unsigned int, unsigned int) = normal; // gtest;

Image import_p6(uint8_t *buffer, uint8_t *end) {
  printf("[+]\tImporting as P6\n");
  uint8_t *ptr = buffer + 2; // for first P6
  assert(*ptr == '\n' && IF_NEND);
  ptr++;

  unsigned int w = import_unum(&ptr, end);
  fprintf(stderr, "[*] width: %d, ", w);
  assert(w != 0 && IF_NEND);
  assert(*ptr == ' ');
  ptr++;

  unsigned int h = import_unum(&ptr, end);
  fprintf(stderr, "height: %d, ", h);
  assert(h != 0 && IF_NEND);

  assert(*ptr == '\n' && IF_NEND);
  ptr++;
  unsigned int mv = import_unum(&ptr, end);
  fprintf(stderr, "maxval: %d, ", mv);
  fprintf(stderr, "size: %d\n", h*w);
  assert(h != 0 && IF_NEND);
  assert(*ptr == '\n');
  ptr++;

  unsigned int size = h * w;
  Image img = init(w, h, 255);
  unsigned int i = 0;

#define MOD(X) X
#define MOD_R(X) rfilter(MOD(X), i)
#define MOD_G(X) gfilter(MOD(X), i)
#define MOD_B(X) bfilter(MOD(X), i)

  while (IF_NEND && i < size) {
    img.red   [i] = MOD_R(*ptr++);
    img.green [i] = MOD_G(*ptr++);
    img.blue  [i] = MOD_B(*ptr++);
    i++;
  }

#undef MOD_B
#undef MOD_G
#undef MOD_R
#undef MOD

  return img;
}

Image import(const char *file) {
  printf("[+] Importing %s\n", file);
  FILE *fp = fopen(file, "rb"); // read binary
  assert(fp != NULL);

  fseek(fp, 0, SEEK_END);
  long fsize = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  uint8_t *buffer = malloc(fsize + 1);
  fread(buffer, fsize, 1, fp);
  fclose(fp);

  // TODO: add P3
  assert(buffer[0] == 'P' && buffer[1] == '6');

  return import_p6(buffer, &(buffer[fsize]));
}
/* }}} */

/* Operations {{{ */
/**
 * Operations - Functions that manipulate the Image type
 *
 * Operations are functions that are used to create, destroy, modify, copy,
 * etc. the `Image` type. They do not modify the pixels, unless the process
 * requires it, or allows it.
 */

/* Filter {{{ */
/* Filter Functions {{{ */
uint8_t invert(uint8_t f) { return 255 - f; }
/* }}} */


void filter(Image* img, uint8_t (*filter)(uint8_t)) {
  for (size_t x = 0; x < img->size; x++) {
    img->red[x]   = filter(img->red[x]);
    img->green[x] = filter(img->green[x]);
    img->blue[x]  = filter(img->blue[x]);
  }
}
/* }}} */
/* Scale {{{ */
// TODO: add support scale down
Image scale(Image* src, int r) {
  assert(r >= 1 && "Scalling down is currently not supported");
  Image buf = init(src->width*r, src->height*r, src->depth);

  int bi = 0;
  for (int x=0; x<src->height; x++) {
    for (int z=0; z<r; z++) {
      for (int y=0; y<src->width; y++) {
        int rc = src->red[(x*src->width)+y];
        int gc = src->green[(x*src->width)+y];
        int bc = src->blue[(x*src->width)+y];
        for (int z2=0; z2<r; z2++) {
          buf.red[bi] = rc;
          buf.green[bi] = gc;
          buf.blue[bi]  = bc;
          bi++;
        }
      }
    }
  }
  return buf;
}
/* }}} */
/* Copy {{{ */
/**
 * context: These two images must be the same size
 */
void copyImg_f(Image* dst, Image* src, uint8_t (*f)(uint8_t, uint8_t)) {
	assert(dst->width == src->width);
	assert(dst->height == src->height);
	assert(dst->size == src->size);

	for (int x=0; x<dst->size; x++) {
		dst->red[x]		=    f(dst->red[x],    src->red[x]);
		dst->green[x]	=  f(dst->green[x],  src->green[x]);
		dst->blue[x]	=   f(dst->blue[x],   src->blue[x]);
	}
}

/**
 * context: These two images must be the same size
 */
void copyImg(Image* dst, Image* src) {
	assert(dst->width == src->width);
	assert(dst->height == src->height);
	assert(dst->size == src->size);

	memcpy(dst->red, src->red, dst->size);
	memcpy(dst->green, src->green, dst->size);
	memcpy(dst->blue, src->blue, dst->size);
}
/* }}} */
/* }}} */
/* Manipulations {{{ */
/**
 * Manipulations - Manipulate the Image type pixels directly
 *
 * Manipulations modify the data of the image directly, and do not touch
 * other aspecs of an image.
 */

/* Fill {{{ */
void fill(Image* src, color fill) {
  memset(src->red, fill.r, src->size);
  memset(src->green, fill.g, src->size);
  memset(src->blue, fill.b, src->size);
}
/* }}} */
/* Create Gradient {{{ */
void createGradientTD(Image* i, int a) {
  float ga = ((float)i->depth / (float)i->width) * a;
  float g = 0.0;
  for (int x = 0; x < i->size;) {
    g += ga;
    for (int y=0; y < i->height; y++,x++) {
      i->red[x]    =  g;
      i->blue[x]   =  g;
      i->green[x]  =  g;
    }
  }
}
void createGradientLR(Image* i, int a) {
  float ga = ((float)i->depth / (float)i->width) * a;
  float g = 0.0;
  for (int x = 0; x < i->size;) {
    for (int y=0; y < i->width; y++,x++) {
      g += ga;
      i->red[x]    =  g;
      i->blue[x]   =  g;
      i->green[x]  =  g;
    }
    g=0.0;
  }
}
/* }}} */

/* Bubble Sort {{{ */
#define BSSWAP(A)                                                              \
  do {                                                                         \
    uint8_t z = A[i];                                                          \
    A[i] = A[i + 1];                                                           \
    A[i + 1] = z;                                                              \
  } while (0)
void bchange(Image* img, unsigned int m) {
  int i = (img->size - m) - 1;
  while (i > 0) {
    if (img->red[i] < img->red[i + 1]) {
      BSSWAP(img->red);
      BSSWAP(img->green);
      BSSWAP(img->blue);
    }

    i--;
  }
}
void bubblesort(Image* img, unsigned int len) {
  for (int x = 0; x < len; x++) {
    bchange(img, x);
  }
}
/* }}} */
/* Quick Sort {{{ */
Image *qsort_img = NULL;
uint8_t **qsort_arr = NULL;
#define QSARR (*qsort_arr)
#define QSSWAP_H(A, B, C)                                                      \
  do {                                                                         \
    uint8_t z = A[B];                                                          \
    A[B] = A[C];                                                               \
    A[C] = z;                                                                  \
  } while (0);
// #define QSSWAP(B,C) QSSWAP_H(red, B, C) QSSWAP_H(green,B,C)
// QSSWAP_H(blue,B,C)
#define QSSWAP(B, C) QSSWAP_H(QSARR, B, C)
int times = 0;
int _quicksort_h(int low, int high) {
  int p = QSARR[low];
  int i = low;
  int j = high;

  while (i < j) {
    while (QSARR[i] <= p && i <= high - 1) {
      i++;
    }

    while (QSARR[j] > p && j >= low + 1) {
      j--;
    }
    if (i < j) {
      QSSWAP(i, j);
    }
  }
  QSSWAP(low, j);
  if (times++ > 255*3) {
    times=0;
    snapshot(qsort_img);
  }
  return j;
}
void _quicksort(int low, int high, int d) {
  if (low < high) {
    d++;
    int pi = _quicksort_h(low, high);
    _quicksort(low, pi - 1, d);
    _quicksort(pi + 1, high, d);
  }
}
void quicksort(Image* img) {
  assert(isallocated(img) == 0);
  qsort_img = img;

  // TODO: have quicksort `sort` function for what swapping to use
  qsort_arr = &(img->red);
  _quicksort(0, img->size-1, 0);
  qsort_arr = &(img->green);
  _quicksort(0, img->size-1, 0);
  qsort_arr = &(img->blue);
  _quicksort(0, img->size-1, 0);

  qsort_arr = NULL;
  qsort_img = NULL;
}
/* }}} */

/* TODO: Sobel Operator {{{ */
// TODO: move vvv to good location
uint8_t add(uint8_t dst, uint8_t src) {
  if (src < dst)
    return (dst + src);
  return 0;
}
uint8_t subtract(uint8_t dst, uint8_t src) {
  if (src >= dst)
    return (dst - src);
  return 0;
}
uint8_t mean(uint8_t dst, uint8_t src) {
	return (dst + src)/2;
}
uint8_t f_floor(uint8_t dst, uint8_t src) {
  if (src < 255/4) {
    return 0;
  }
  return dst;
}
uint8_t f_max(uint8_t dst, uint8_t src) {
  if (src < 255/4) {
    return 0;
  }
  return dst;
}

// TODO:
Image sobelOperator(Image* src) {
  Image buf = init(src->width, src->height, src->depth);
  copyImg(&buf, src);
  for (int x = src->width + 1; x < src->size - src->width - 1; x++) {
#define X(A) \
  ((A[x - src->width - 1] * -1) + \
	 (A[x - src->width]		  *  0) + \
	 (A[x - src->width + 1] *  1) + \
   (A[x - 1]				      * -2) + \
	 (A[x]						      *  0) + \
	 (A[x + 1]				      *  2) + \
	 (A[x + src->width - 1] * -1) + \
   (A[x + src->width]		  *  0) + \
	 (A[x + src->width + 1] *  1))
#define Z(A) \
  ((A[x - src->width - 1] * -1) + \
	 (A[x - src->width]		  * -2) + \
	 (A[x - src->width + 1] * -1) + \
   (A[x - 1]				      *  0) + \
	 (A[x]						      *  0) + \
	 (A[x + 1]				      *  0) + \
	 (A[x + src->width - 1] *  1) + \
   (A[x + src->width]		  *  2) + \
	 (A[x + src->width + 1] *  1))
#define Y(A)	sqrt(X(A) * Z(A))

    buf.red[x]    =  Y(src->red);
    buf.green[x]  =  Y(src->green);
    buf.blue[x]   =  Y(src->blue);

#undef X
#undef Y
#undef Z
    if (x%(src->width) == 0) {
      snapshot(&buf);
    }
  }
  return buf;
}
/* }}} */
/* TODO: Gausian Blur {{{ */
// TODO: Custom kernels.
// TODO: Fix edge detection.
uint8_t _gaussianBlur(int index, uint8_t* arr, Image const* img) {
  const int w = img->width;
  const int nav[5][5] = {
    {-((w*2)-2), -((w*2)-1), -((w*2)), -((w*2)+1), -((w*2)+2)},
    {-(w-2), -(w-1), -(w), -(w+1), -(w+2)},
    {-2, -1, 0, 1, 2},
    {w-2, w-1, w, w+1, w+2},
    {(w*2)-2, (w*2)-1, (w*2), (w*2)+1, (w*2)+2},
  };
  const uint8_t k[5][5] = {
    {1, 4, 7, 4, 1},
    {4, 16, 26, 16, 4},
    {7, 26, 41, 26, 7},
    {4, 16, 26, 16, 4},
    {1, 4, 7, 4, 1},
  };
  int r2 = 0;
  for (int x=0; x<5; x++) {
    for (int y=0; y<5; y++) {
      r2 += k[x][y] * arr[index-nav[x][y]];
    }
  }

  return r2/(277);
}
Image gaussianBlur(Image* src) {
  Image buf = init(src->width, src->height, src->depth);
  copyImg(&buf, src);

  for (int x = src->width + 1; x < src->size - src->width - 1; x++) {

    buf.red[x]    =  _gaussianBlur(x, src->red,   src);
    buf.green[x]  =  _gaussianBlur(x, src->green, src);
    buf.blue[x]   =  _gaussianBlur(x, src->blue,  src);

    if (x%(src->width) == 0) {
      snapshot(&buf);
    }
  }
  return buf;
}
/* }}} */
/* TODO: Stenography {{{ */
/**
 * stenogrphy_putSize() - Returns the size in bytes, that an image can hold.
 * size:   The length of image used, (e.g. sum of Red, Green and Blue pixels)
 * strip:  The length, from least significant bit to most, used
 *
 * Takes the size of an image, and the strip length, and calculates the amount
 * available for stenography.
 *
 * context: This is for use in the stenography set.
 * return: capacity in bytes available for stenography in image.
 */
unsigned int stenography_putSize(size_t size, unsigned int strip) {
  return (size * strip)/8;
}

/**
 * stenography_putSizeImg() - Returns size in bytes an image can hold.
 * img:   Image to use in stenography
 * strip: The lengt, from least significant bit to most, used
 *
 * Take in an image, get the size, and uses the strip length, to calculate the
 * feasible total capacity of the image.
 *
 * context: This is for use in the stenography set.
 * return: capacity in bytes available for stenography in image.
 */
unsigned int stenography_putSizeImg(Image* img, unsigned int strip) {
  return stenography_putSize(img->size, strip);
}

void sten_strip(Image* img, unsigned int len) {
  // Move 1 len distance, sub one to get all 1's.
  // If len == 0, then strip = 0.
  unsigned int strip = ~((1U << len) - 1U);
  for (int x=0; x<img->size; x++) {
    img->red[x]    =  img->red[x]    &  strip;
    img->green[x]  =  img->green[x]  &  strip;
    img->blue[x]   =  img->blue[x]   &  strip;
  }
}

void stenography_put(Image* img, unsigned int strip, uint8_t* data, size_t dlen) {
  assert(strip <= 8);
  size_t total = (img->size * strip)/8;
  assert(total > dlen);

  assert(strip % 2 == 0); // is even

  sten_strip(img, strip);

  const size_t smask = (1U << strip) - 1U;

  int ii = 0;
  for (int x=0; x<dlen && ii<img->size; x++) {
    uint8_t hold = data[x];
    // TODO: fix, can get stuck between each section.
    // making blue missout
    for (int y=0; y<8;) {
      uint8_t b = hold & smask;
      img->red[ii] &= ~smask;
      img->red[ii] |= b;
      hold >>= strip;
      y += strip;

      b = hold & smask;
      img->green[ii] &= ~smask;
      img->green[ii] |= b;
      hold >>= strip;
      y += strip;

      b = hold & smask;
      img->blue[ii] &= ~smask;
      img->blue[ii] |= b;
      hold >>= strip;
      y += strip;
      ii++;
    }
  }
}

uint8_t *stenography_get(Image *img, int strip) {
  assert(strip <= 8);
  assert(strip % 2 == 0); // is even

  const size_t smask = (1U << strip) - 1U;

  size_t len = 0;
  const int tmplen = 256;
  uint8_t* buf = malloc(sizeof(uint8_t) * tmplen);
  memset(buf, 0, tmplen);

  int ii=0;
  for (; ii<img->size;) {
    uint8_t h = 0;
    for (int x=0; x<8;) {
      uint8_t a = img->red[ii] & smask;
      h += a << x;
      x += strip;

      a = img->green[ii] & smask;
      h += a << x;
      x += strip;

      a = img->blue[ii] & smask;
      h += a << x;
      x += strip;
      ii++;
    }
    if (isprint(h)) {
      assert(len < tmplen-1); // TODO: add resize buffer
      buf[len++] = h;
      continue;
    }

    return buf;
  }

  return NULL;
}
/* }}} */
/* }}} */

// vim: shiftwidth=2 tabstop=2 expandtab
