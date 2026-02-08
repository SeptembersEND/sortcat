unsigned int g_letterWidth = 8;
unsigned int g_letterHeight = 13;
static color g_letterColor = (color){0};
void writeLetter(Image* img, char c, point p) {
  if (c < 32 || c > 126)
    return;
  unsigned char chr = c - 32;

  for (int x=0; x<13; x++) {
    for (int y = 0; y < g_letterWidth; y++) {
      if (!(letters[chr][x] & (1 << y)))
        continue;
      plot_pointColor(img, (point){.x = x + p.x, .y = y + p.y}, img->width,
                      g_letterColor);
    }
  }
}

// TODO: rewrite, add struct for options
void writenText(Image* img, size_t n, char* str, point orig, point orig2) {
  point shift = {
    .x = orig.x,
    .y = orig.y,
  };

  int nl      = (img->width - orig.y) / g_letterWidth;
  nl += orig2.y;
  int max     = nl * ((img->height / g_letterHeight) + orig2.x);
  int letters = 0;

  for (int x=1;x<n && *str != '\0'; x++) {
    if (*str != '\n') {
      writeLetter(img, *str++, shift);
      letters++;
      if (letters >= max) {
        printf("[+]\twritenText(): Unable to complete full text, not enough room\n");
        return;
      }
    } else {
      str++;
      letters += nl - (x%nl);
      x = nl;
    }
    if (x%nl == 0) {
      shift.y = orig.y;
      shift.x += g_letterHeight;
      continue;
    }
    shift.y += g_letterWidth;
  }
}
// vim: shiftwidth=2 tabstop=2 expandtab
