/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: lizhenglin <lizhenglin@huoyunren.com>                       |
   +----------------------------------------------------------------------+
 */

#ifndef _LIB_GEO_H_
#define _LIB_GEO_H_

#include <stdbool.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum {
    GEO_NORTH = 0,
    GEO_EAST,
    GEO_WEST,
    GEO_SOUTH
} GEO_direction;

typedef struct {
    double max;
    double min;
} GEO_range;

typedef struct {
    GEO_range latitude;
    GEO_range longitude;
} GEO_area;

typedef struct {
    char* north;
    char* east;
    char* west;
    char* south;
    char* north_east;
    char* south_east;
    char* north_west;
    char* south_west;
} GEO_neighbors;

bool GEO_verify_hash(const char *hash);
char* GEO_encode(double latitude, double longitude, unsigned int hash_length);
GEO_area* GEO_decode(const char* hash);
GEO_neighbors* GEO_get_neighbors(const char *hash);
void GEO_free_neighbors(GEO_neighbors *neighbors);
char* GEO_get_adjacent(const char* hash, GEO_direction dir);
void GEO_free_area(GEO_area *area);

double GEO_simple_distance(double lat1, double lng1, double lat2, double lng2);
double GEO_distance(double lat1, double lng1, double lat2, double lng2);

#if defined(__cplusplus)
}
#endif

#endif
