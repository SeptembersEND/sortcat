/* Hex to Color {{{ */
int hex2dec(char ch) {
  if (isdigit(ch)) {
    return ch - '0';
  } else if (ch >= 'a' && ch <= 'f') {
    return (ch - 'a') + 10;
  } else if (ch >= 'A' && ch <= 'F') {
    return (ch - 'A') + 10;
  }

  return -1;
}

// TODO: add support for Alpha, and three and four length hexes
color hex2color(const char *hex) {
  color ret = {0};
  int len = strlen(hex);
  unsigned int* r = &(ret.r);

  if (*hex == '#' && len == 7) {
    hex++;
    len = 6;
  }

  if (len == 6) {

    for (int x = 0; x < 3; x++) {
      int h = hex2dec(*hex++);
      assert(h != -1 && "Invalid charactor in hex string");
      *(r+x) = h << 4;

      h = hex2dec(*hex++);
      assert(h != -1 && "Invalid charactor in hex string");
      *(r+x) += h;
    }
    return ret;
  } else {
    assert(1 && "Hex format is not correct");
  }

  return ret;
}
/* }}} */

/* Ploting {{{ */
void plot_pointColor(Image *img, point p, size_t w, color c) {
  int v = p.y + (p.x * w);
  if (v >= img->size)
    return;
  img->red[v]   = c.r;
  img->green[v] = c.g;
  img->blue[v]  = c.b;
}

void plot_point(Image *img, point p, size_t w) {
  int v = p.y + (p.x * w);
  if (v >= img->size)
    return;
  img->red[v]   = (INIT_COLOR-(img->depth))%img->depth;
  img->green[v] = (INIT_COLOR-(img->depth))%img->depth;
  img->blue[v]  = (INIT_COLOR-(img->depth))%img->depth;
}


// <https://en.wikipedia.org/wiki/Midpoint_circle_algorithm>
// <https://zingl.github.io/bresenham.html>
unsigned int g_draw_circle_index = 0;
void draw_circle(Image* img, int xm, int ym, int r) {
  int x = -r, y = 0, err = 2 - 2 * r;
  int re = r;
  do {
    plot_point(img, (point){xm - x, ym + y}, img->width);
    plot_point(img, (point){xm - y, ym - x}, img->width);
    plot_point(img, (point){xm + x, ym - y}, img->width);
    plot_point(img, (point){xm + y, ym + x}, img->width);

    re = err;
    if (re <= y)
      err += ++y * 2 + 1;
    if (re > x || err > y)
      err += ++x * 2 + 1;

    static int hold = 0;
    if (g_draw_circle_index != 0 && x != hold && x%((r/g_draw_circle_index)) == 0) {
      hold = x;
      snapshot(img);
      usleep(10000);
    }

  } while (x < 0);
}

// <https://sites.cc.gatech.edu/grads/m/Aaron.E.McClennen/Bresenham/code.html>
void draw_line(Image *img, int x0, int y0, int x1, int y1) {
  // TODO: detect if out of bounds and draw
  //int w = img->width;
  //if (x0 < 0) x0 = 0;
  //if (x1 < 0) x1 = 0;
  //if (x0 > w) x0 = w;
  //if (x1 > w) x1 = w;
  //
  //int h = img->height;
  //if (y0 < 0) y0 = 0;
  //if (y1 < 0) y1 = 0;
  //if (y0 > h) y0 = h;
  //if (y1 > h) y1 = h;

  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy, e2;

  while(1) {
    plot_point(img, (point){x0, y0}, img->width);
    if (x0 == x1 && y0 == y1)
      break;

    e2 = 2*err;
    if (e2 >= dy) {
      err += dy;
      x0 += sx;
    }
    if (e2 <= dx) {
      err += dx;
      y0 += sy;
    }
  }
}
/* }}} */
/* Convert {{{ */
point get3dto2d(Image* img, point3d p) {
  int w    = img->width;
  int h    = img->height;
  assert(w == h && "currently unable to process in non square viewports correctly");

  // Project xyz to xy
  float px = p.x / p.z;
  float py = p.y / p.z;

  // Convert -1 .. 1 to x coordinate
  // `1 - ()` flips y
  // TODO:FIX: x being y?
  int x    = (1 - (px + 1.0) / 2.0) * w;
  int y    = ((py + 1.0) / 2.0) * h;

  return (point){x, y};
}
/* }}} */

float g_zaway = 10;
float g_xaway = 10;
float g_f = 0.0;

point3d put_3dface_rotate(point3d p) {
  const float c = cos(g_f);
  const float s = sin(g_f);

  // <https://stackoverflow.com/questions/14607640/rotating-a-vector-in-3d-space#14609567>
  return (point3d){
      .x = (p.x) + g_xaway,
      .y = (p.y * c - p.z * s),
      .z = (p.y * s + p.z * c) + g_zaway - g_f,
  };
}

/* Put Types {{{ */
point3d put_3dface_none(point3d p) {
  return (point3d){p.x PZ_MOD_X, p.y PZ_MOD_Y, p.z PZ_MOD_Z};
}

void put_3dface(Image *img, point3d *p, size_t len, face3d f,
                point3d (*mod)(point3d)) {
  assert(f.l < len);

  point3d (*m)(point3d) = NULL;
  if (mod == NULL)
    m = put_3dface_none;
  else
    m = mod;

  int max   = f.l;
  int loops = (max == 2) ? 1 : max; // Prevent loop over draw
  for (int x = 0; x < loops; x++) {
    int fg = f.p[x];
    assert(fg < len);
    point p1 = get3dto2d(
        img, (*m)((point3d){.x = p[fg].x, .y = p[fg].y, .z = p[fg].z}));

    fg = f.p[(x + 1) % max];
    if (fg >= len)
      return;
    assert(fg < len);
    point p2 = get3dto2d(
        img, (*m)((point3d){.x = p[fg].x, .y = p[fg].y, .z = p[fg].z}));

    draw_line(img, p1.x, p1.y, p2.x, p2.y);
  }
}

void put_3dpoints(Image* img, point3d* p, size_t len) {
  int w = img->width;
  for (int x=0; x<len; x++) {
    point p1 = get3dto2d(img, (point3d){.x = p[x].x, .y = p[x].y, .z = p[x].z+2});
    plot_pointColor(img, (point){p1.x, p1.y}, w, (color){50, 255, 50});
  }
}

void put_3dmodel(model *mdl, Image *img, void (*const per_face)(model *, int),
                 point3d (*const transform)(point3d)) {
  for (int x = 0; x < mdl->flen; x++) {
    ende--;

    put_3dface(img, mdl->points, mdl->plen, mdl->faces[x], transform);

    if (per_face != NULL)
      per_face(mdl, x);

    snapshot(img);
    ende++;
  }
  snapshot(img);
}
/* }}} */
/* Import OBJ {{{ */
void import_obj_count(uint8_t *ptr, uint8_t *end, size_t *v, size_t *f) {
  size_t cnt_v = 0;
  size_t cnt_f = 0;
  uint8_t *cnt = ptr;
  do {
    switch (*cnt) {
    case 'v':
      if (++cnt != end && *cnt == ' ') {
        cnt_v++;
      }
      break;

    case 'f':
      if (++cnt != end && *cnt == ' ') {
        cnt_f++;
      }
      break;

    case '#':
    default:
      break;
    }

    while (cnt != end && *cnt != '\n') cnt++;

  } while (cnt++ != end);

  (*v) = cnt_v;
  (*f) = cnt_f;
}

point3d import_obj_getVertex(uint8_t* ptr, uint8_t* end) {
  float ret[3] = {0};

  int len=0;
  for (int x=0; x<3; x++) {
    for (; *(ptr + len) != ' ' && *(ptr + len) != '\n';)
      len++;

    uint8_t *p1e = &(ptr[len]);
    ret[x]       = (strtof((const char *)ptr, (char **)&p1e));
    ptr = p1e;
  }

  return (point3d){
      .x = ret[1],
      .y = ret[0],
      .z = ret[2],
  };
}

face3d import_obj_getFace(uint8_t* ptr, uint8_t* end, face3d_point** f3p) {
  size_t ret_l        = 0;
  face3d_point *ret_p = (*f3p);


  for (; *(ptr) != '\n' && *(ptr) != '\0'; ret_l++) {
    int len = 0;
    ptr++;
    for (; *(ptr + len) != ' ' && *(ptr + len) != '\t' &&
           *(ptr + len) != '\n' && *(ptr + len) != '\0';)
      len++;


    uint8_t *p1e = ptr + len;
    //printf("%d %d %.*s ", ret_l, len, len, ptr);

    ret_p[ret_l] = (strtod((const char *)ptr, (char **)&p1e)) - 1;

    //printf("-- %2d:(%2d) %.*s\n", ret_p[ret_l], len, len, ptr);

    ptr          = ptr + len;
  }

  //printf("``` %ld\n", ret_l);
  (*f3p) += ret_l;
  return (face3d){
      .l = ret_l,
      .p = ret_p,
  };
}

model import_obj(uint8_t* ptr, uint8_t* end) {
  printf("[+]\tImporting Wavefront OBJ of %ld bytes\n", end-ptr);
  assert(end != NULL && ptr != NULL);
  assert(ptr != end);
  model ret = {0};

  size_t cnt_v = 0;
  size_t cnt_f = 0;
  printf("[*]\tNOTE: only supports 'v' vertexes and 'f' faces\n");
  import_obj_count(ptr, end, &cnt_v, &cnt_f);
  printf("Vertexs: %ld\n", cnt_v);
  printf("Faces:   %ld\n", cnt_f);

  // TODO:FIX: unable to predict amount of room needed for a_faces needed.
  size_t cnt_af = cnt_f * 5;
  ret.aflen     = cnt_af;
  ret.points    = malloc(sizeof(point3d) * cnt_v);
  ret.faces     = malloc(sizeof(face3d) * cnt_f);
  ret.a_faces   = malloc(sizeof(face3d_point) * cnt_af);
  memset(ret.points, 0, cnt_v);
  memset(ret.faces, 0, cnt_f);
  memset(ret.a_faces, 0, cnt_af);

  size_t cur_v = 0;
  size_t cur_f = 0;
  uint8_t* cur = ptr;
  face3d_point* cur_af = ret.a_faces;
  do {
    switch (*cur) {
    case 'v':
      if (++cur != end && *cur == ' ') {
        assert(cur_v < cnt_v);
        ret.points[cur_v++] = import_obj_getVertex(++cur, end);
      }
      break;

    case 'f':
      if (++cur != end && *cur == ' ') {
        assert(cur_f < cnt_f);
        ret.faces[cur_f++] = import_obj_getFace(cur, end, &cur_af);
        break;
      }
      break;

    default:
      break;
    }

    while (cur != end && *cur != '\n') cur++;

  } while (cur++ != end);

  ret.plen             = cur_v;
  ret.flen             = cur_f;

  return ret;
}

model import_objF(const char *file) {
  FILE *fp = fopen(file, "rb");
  assert(fp != NULL);
  fseek(fp, 0, SEEK_END);
  long fsize = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  uint8_t *buffer = malloc(fsize + 1);
  fread(buffer, fsize, 1, fp);
  fclose(fp);

  return import_obj(buffer, &(buffer[fsize]));
}
/* }}} */

// vim: shiftwidth=2 tabstop=2 expandtab
