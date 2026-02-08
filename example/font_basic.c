#include "../src/3d.h"
#include "../src/font.h"
#include "../src/lib.h"
#include "text.h"

void example_font_basic__center(Image* img) {
  char* fit_text = abandon;

  int fit = strlen(fit_text);
  int zy      = 4 * g_letterHeight;

  int nl      = (img->width - zy) / g_letterWidth;
  nl -= 6; // Margin
  int max_nl = img->height / g_letterHeight;
  int fitl = fit/nl;
  int m_zy = ((nl/2) - (fit/2)) * g_letterWidth;
  zy += m_zy;

  int zx   = ((max_nl / 2) - (fitl / 2)) * g_letterHeight;
  g_letterColor = hex2color("#ff0546");

  writenText(img, fit, fit_text, (point){.x = zx, .y = zy}, (point){.x = -6, .y = -6});
  snapshot(img);
}

void example_font_basic__text(Image* img) {
  char* str_text = lorem_ipsum;
  size_t str_len = strlen(str_text);

  g_letterColor = hex2color("#50ff50");
  writenText(img, str_len, str_text,
                            (point){.x = g_letterWidth, .y = g_letterHeight},
                            (point){.x = -1, .y = -1});
  snapshot(img);
}

void example_font_basic(Image* img) {
  ende--;
  example_font_basic__center(img);
  usleep(100000);
  example_font_basic__text(img);
  ende++;
}
// vim: shiftwidth=2 tabstop=2 expandtab
