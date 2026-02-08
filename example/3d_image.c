#include "../src/3d.h"
#include "../src/lib.h"

// NOTE: this dictates the time between snapshots
size_t g_example_3d_image__wait = 10000;

void example_3d_image__full(Image *img, model *obj) {
  g_zaway           = 8;
  g_xaway           = 4;
  g_f               = 0;

  put_3dmodel(obj, img, NULL, put_3dface_rotate);
  usleep(g_example_3d_image__wait);
  snapshot(img);
}

void example_3d_image__rotate_fill(Image *img, model *obj) {
  g_zaway           = 12;
  g_xaway           = 4;
  g_f               = 0;
  const float adder = M_PI / ((obj->flen));

  for (int x = 0; x < obj->flen; x++) {
    fill(img, (color){~0, ~0, ~0});
    for (int y = 0; y < x; y++) {
      put_3dface(img, obj->points, obj->plen, obj->faces[y], put_3dface_rotate);
    }
    usleep(g_example_3d_image__wait);
    g_f += adder;
    usleep(g_example_3d_image__wait);
    snapshot(img);
  }
}

void example_3d_image(Image *img, const char *model_file) {
  ende--;
  model obj = import_objF(model_file);

  example_3d_image__full(img, &obj);
  usleep(g_example_3d_image__wait);

  printf("[+] Roatate model\n");
  example_3d_image__rotate_fill(img, &obj);

  ende++;
  snapshot(img);
}

// vim: shiftwidth=2 tabstop=2 expandtab
