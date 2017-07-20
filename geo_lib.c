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
   | Authors: carlziess <chengmo9292@126.com>                       |
   +----------------------------------------------------------------------+
 */
 
#include <ctype.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "geo_lib.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX_HASH_LENGTH 22


#define PI 3.141592653
#define dutorad(X) ((X)/180*PI)
#define radtodu(X) ((X)/PI*180)
#define EARTH_RADIUS 6378137

double radian(double d)
{
	return d * PI / 180.0;
}

#define REFINE_RANGE(range, bits, offset) \
    if (((bits) & (offset)) == (offset)) \
        (range)->min = ((range)->max + (range)->min) / 2.0; \
    else \
        (range)->max = ((range)->max + (range)->min) / 2.0;

#define SET_BIT(bits, mid, range, value, offset) \
    mid = ((range)->max + (range)->min) / 2.0; \
    if ((value) >= mid) { \
        (range)->min = mid; \
        (bits) |= (0x1 << (offset)); \
    } else { \
        (range)->max = mid; \
        (bits) |= (0x0 << (offset)); \
    }

static const char BASE32_ENCODE_TABLE[33] = "0123456789bcdefghjkmnpqrstuvwxyz";
static const char BASE32_DECODE_TABLE[44] = {
    /* 0 */   0, /* 1 */   1, /* 2 */   2, /* 3 */   3, /* 4 */   4,
    /* 5 */   5, /* 6 */   6, /* 7 */   7, /* 8 */   8, /* 9 */   9,
    /* : */  -1, /* ; */  -1, /* < */  -1, /* = */  -1, /* > */  -1,
    /* ? */  -1, /* @ */  -1, /* A */  -1, /* B */  10, /* C */  11,
    /* D */  12, /* E */  13, /* F */  14, /* G */  15, /* H */  16,
    /* I */  -1, /* J */  17, /* K */  18, /* L */  -1, /* M */  19,
    /* N */  20, /* O */  -1, /* P */  21, /* Q */  22, /* R */  23,
    /* S */  24, /* T */  25, /* U */  26, /* V */  27, /* W */  28,
    /* X */  29, /* Y */  30, /* Z */  31
};

static const char NEIGHBORS_TABLE[8][33] = {
    "p0r21436x8zb9dcf5h7kjnmqesgutwvy", /* NORTH EVEN */
    "bc01fg45238967deuvhjyznpkmstqrwx", /* NORTH ODD  */
    "bc01fg45238967deuvhjyznpkmstqrwx", /* EAST EVEN  */
    "p0r21436x8zb9dcf5h7kjnmqesgutwvy", /* EAST ODD   */
    "238967debc01fg45kmstqrwxuvhjyznp", /* WEST EVEN  */
    "14365h7k9dcfesgujnmqp0r2twvyx8zb", /* WEST ODD   */
    "14365h7k9dcfesgujnmqp0r2twvyx8zb", /* SOUTH EVEN */
    "238967debc01fg45kmstqrwxuvhjyznp"  /* SOUTH ODD  */
};

static const char BORDERS_TABLE[8][9] = {
    "prxz",     /* NORTH EVEN */
    "bcfguvyz", /* NORTH ODD */
    "bcfguvyz", /* EAST  EVEN */
    "prxz",     /* EAST  ODD */
    "0145hjnp", /* WEST  EVEN */
    "028b",     /* WEST  ODD */
    "028b",     /* SOUTH EVEN */
    "0145hjnp"  /* SOUTH ODD */
};

bool
GEO_verify_hash(const char *hash)
{
    const char *p;
    unsigned char c;
    p = hash;
    while (*p != '\0') {
        c = toupper(*p++);
        if (c < 0x30)
            return false;
        c -= 0x30;
        if (c > 43)
            return false;
        if (BASE32_DECODE_TABLE[c] == -1)
            return false;
    }
    return true;
}

GEO_area*
GEO_decode(const char *hash)
{
    const char *p;
    unsigned char c;
    char bits;
    GEO_area *area;
    GEO_range *range1, *range2, *range_tmp;

    area = (GEO_area *)emalloc(sizeof(GEO_area));
    if (area == NULL)
        return NULL;

    area->latitude.max   =   90;
    area->latitude.min   =  -90;
    area->longitude.max =  180;
    area->longitude.min = -180;

    range1 = &area->longitude;
    range2 = &area->latitude;

    p = hash;

    while (*p != '\0') {

        c = toupper(*p++);
        if (c < 0x30) {
            efree(area);
            return NULL;
        }
        c -= 0x30;
        if (c > 43) {
            efree(area);
            return NULL;
        }
        bits = BASE32_DECODE_TABLE[c];
        if (bits == -1) {
            efree(area);
            return NULL;
        }

        REFINE_RANGE(range1, bits, 0x10);
        REFINE_RANGE(range2, bits, 0x08);
        REFINE_RANGE(range1, bits, 0x04);
        REFINE_RANGE(range2, bits, 0x02);
        REFINE_RANGE(range1, bits, 0x01);

        range_tmp = range1;
        range1    = range2;
        range2    = range_tmp;
    }
    return area;
}

char*
GEO_encode(double lat, double lon, unsigned int len)
{
    unsigned int i;
    char *hash;
    unsigned char bits = 0;
    double mid;
    GEO_range lat_range = {  90,  -90 };
    GEO_range lon_range = { 180, -180 };

    double val1, val2, val_tmp;
    GEO_range *range1, *range2, *range_tmp;

    assert(lat >= -90.0);
    assert(lat <= 90.0);
    assert(lon >= -180.0);
    assert(lon <= 180.0);
    assert(len <= MAX_HASH_LENGTH);

    hash = (char *)emalloc(sizeof(char) * (len + 1));
    if (hash == NULL)
        return NULL;

    val1 = lon; range1 = &lon_range;
    val2 = lat; range2 = &lat_range;

    for (i=0; i < len; i++) {

        bits = 0;
        SET_BIT(bits, mid, range1, val1, 4);
        SET_BIT(bits, mid, range2, val2, 3);
        SET_BIT(bits, mid, range1, val1, 2);
        SET_BIT(bits, mid, range2, val2, 1);
        SET_BIT(bits, mid, range1, val1, 0);

        hash[i] = BASE32_ENCODE_TABLE[bits];

        val_tmp   = val1;
        val1      = val2;
        val2      = val_tmp;
        range_tmp = range1;
        range1    = range2;
        range2    = range_tmp;
    }

    hash[len] = '\0';
    return hash;
}

void
GEO_free_area(GEO_area *area)
{
    efree(area);
}

GEO_neighbors*
GEO_get_neighbors(const char* hash)
{
    GEO_neighbors *neighbors;

    neighbors = (GEO_neighbors*)emalloc(sizeof(GEO_neighbors));
    if (neighbors == NULL)
        return NULL;

    neighbors->north = GEO_get_adjacent(hash, GEO_NORTH);
    neighbors->east  = GEO_get_adjacent(hash, GEO_EAST);
    neighbors->west  = GEO_get_adjacent(hash, GEO_WEST);
    neighbors->south = GEO_get_adjacent(hash, GEO_SOUTH);

    neighbors->north_east = GEO_get_adjacent(neighbors->north, GEO_EAST);
    neighbors->north_west = GEO_get_adjacent(neighbors->north, GEO_WEST);
    neighbors->south_east = GEO_get_adjacent(neighbors->south, GEO_EAST);
    neighbors->south_west = GEO_get_adjacent(neighbors->south, GEO_WEST);

    return neighbors;
}

char*
GEO_get_adjacent(const char* hash, GEO_direction dir)
{
    int len, idx;
    const char *border_table, *neighbor_table;
    char *base, *refined_base, *ptr, last;

    assert(hash != NULL);

    len  = strlen(hash);
    last = tolower(hash[len - 1]);
    idx  = dir * 2 + (len % 2);

    border_table = BORDERS_TABLE[idx];

    base = (char *)emalloc(sizeof(char) * (len + 1));
    if (base == NULL)
        return NULL;
    memset(base, '\0', sizeof(char) * (len + 1));

    strncpy(base, hash, len - 1);

    if (strchr(border_table, last) != NULL) {
        refined_base = GEO_get_adjacent(base, dir);
        if (refined_base == NULL) {
            efree(base);
            return NULL;
        }
        strncpy(base, refined_base, strlen(refined_base));
        efree(refined_base);
    }

    neighbor_table = NEIGHBORS_TABLE[idx];

    ptr = strchr(neighbor_table, last);
    if (ptr == NULL) {
        efree(base);
        return NULL;
    }
    idx = (int)(ptr - neighbor_table);
    len = strlen(base);
    base[len] = BASE32_ENCODE_TABLE[idx];
    return base;
}

void
GEO_free_neighbors(GEO_neighbors *neighbors)
{
    efree(neighbors->north);
    efree(neighbors->east);
    efree(neighbors->west);
    efree(neighbors->south);
    efree(neighbors->north_east);
    efree(neighbors->south_east);
    efree(neighbors->north_west);
    efree(neighbors->south_west);
    efree(neighbors);
}


double 
GEO_simple_distance(double lat1,double lng1,double lat2,double lng2)
{
	double dx,dy,b,lx,ly;
	dx = lat1 - lat2;
	dy = lng1  - lng2;
	b = (lng1 + lng2) / 2.0;
	lx = dutorad(dx) * EARTH_RADIUS * cos( dutorad(b) );
	ly = dutorad(dy) * EARTH_RADIUS;
	return sqrt(lx*lx+ly*ly);
}

double 
GEO_distance(double lat1, double lng1, double lat2, double lng2)
{
	double radLat1,radLat2,a,b,dst;
	radLat1 = radian(lat1);
	radLat2 = radian(lat2);
	a = radLat1 - radLat2;
	b = radian(lng1) - radian(lng2);
	dst = 2 * asin((sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2) )));
	dst = dst * EARTH_RADIUS;
	return dst;
}

