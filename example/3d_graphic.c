#ifndef EXAMPLE_GRAPHIC__H
#define EXAMPLE_GRAPHIC__H

#include "../src/lib.h"


#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef NEW
#define NEW(NAME, FUNC, ...)                                                   \
  Image NAME = FUNC(__VA_ARGS__);                                              \
  img        = &NAME;
#endif

SDL_Window* win;
SDL_Surface* surf;

#define WIDTH img->width
#define HEIGHT img->height

uint32_t* pixels;


void update_img(Image* img, model obj, const float adder) {
  static int b = 0;
  static int x=1;

  g_f += adder;
  fill(img, (color){~0,~0,~0});
  for (int y = 0; y < x; y++) {
    put_3dface(img, obj.points, obj.plen, obj.faces[y], put_3dface_rotate);
  }

  if (x < obj.flen && b == 0) {
    x++;
  } else if (x >= obj.flen) {
    b = 1;
  }

  if (b == 1 && x > 0) {
    x--;
  } else if (b == 1 && x <= 0) {
    b = 0;
    x++;
  }
}


void update(Image* img) {
  memset(pixels, ~0, sizeof(uint32_t) * WIDTH*HEIGHT);
  for (int x = 0; x < img->size; x++) {
    pixels[x] = (img->red[x] << 16) | (img->green[x] << 8) | (img->blue[x]);
  }
}

void example_3dgraphic_loop(Image *img, const char *model_file) {
  SDL_Init(SDL_INIT_VIDEO);
  win = SDL_CreateWindow("Framebuffer Rendering", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  surf = SDL_GetWindowSurface(win);
  pixels = (uint32_t*)surf->pixels;


  model obj = import_objF(model_file);

  g_zaway           = 8;
  g_xaway           = 4;
  g_f               = 0;
  const float adder = M_PI / ((obj.flen));

  fill(img, (color){~0,~0,~0});
  update_img(img, obj, adder);
  update(img);

  ende = 1;

  bool quit = false;
  SDL_Event e;
  unsigned int time = 0;
  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }

    if (time++ % 8) {
      fill(img, (color){~0,~0,~0});
      update_img(img, obj, adder);
      update(img);
      SDL_UpdateWindowSurface(win);
    }
    usleep(10000);
  }

  SDL_DestroyWindow(win);
  SDL_Quit();
}

void example_3d_graphic(const char* file) {
  printf("[+] Started `example_graphic` %ld\n", time(NULL));

  Image *img = NULL;
  NEW(new, init, 512, 512, 255);
  const char *model_file = file;
  example_3dgraphic_loop(img, model_file);

  printf("[+] Ended %ld\n", time(NULL));
}



#endif
// pkg-config --cflags --libs sdl2
// vim: shiftwidth=2 tabstop=2 expandtab
