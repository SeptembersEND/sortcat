#ifndef SRC_3D__H
#define SRC_3D__H

#include "image.h"
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 3D Data Types {{{ */
/**
 * typedef struct point3d - singular 3d point
 * @x:	x coordinate
 * @y:	y coordinate
 * @z:	z coordinate
 */
typedef struct {
  float x;
  float y;
  float z;
} point3d;
/**
 * typedef face3d_point - datatype for 3d face points
 */
typedef size_t face3d_point;
/**
 * typedef struct face3d - defines a face using a list of points
 * @l:	number of points in face
 * @p:	pointer to start of pointer
 */
typedef struct {
  size_t l;
  face3d_point *p;
} face3d;

/**
 * typedef struct model - defines a model
 * @plen:	number of points
 * @flen:	number of faces
 * @aflen:	maximum number allocated for a_faces
 * @points:	array of points
 * @a_faces:	array of raw faces
 * @faces:	array of indexes into a_faces
 */
typedef struct {
  size_t plen;
  size_t flen;
  size_t aflen;
  point3d *points;
  face3d_point *a_faces;
  face3d *faces;
} model;
/* }}} */
/* 2D Data Types {{{ */
/**
 * typedef struct point - defines a 2d point
 */
typedef struct {
  int x;
  int y;
} point;
/* }}} */

/**
 * define PZ_MOD_X - `put_3dface_none()` X modifier
 */
#ifndef PZ_MOD_X
#define PZ_MOD_X
#endif
/**
 * define PZ_MOD_Y - `put_3dface_none()` Y modifier
 */
#ifndef PZ_MOD_Y
#define PZ_MOD_Y
#endif
/**
 * define PZ_MOD_Z - `put_3dface_none()` Z modifier
 */
#ifndef PZ_MOD_Z
#define PZ_MOD_Z
#endif


#include "3d.c"

#endif
// vim: shiftwidth=2 tabstop=2 expandtab
