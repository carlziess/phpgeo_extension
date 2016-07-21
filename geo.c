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
 
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "geo.h"
#include "geo_lib.h"

const MAX_HASH_LENGTH = 22;

/* If you declare any globals in php_geo.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(geo)
*/

/* True global resources - no need for thread safety here */
static int le_geo;

/* {{{ geo_functions[]
 *
 * Every user visible function must have an entry in geo_functions[].
 */
const zend_function_entry geo_functions[] = {
	PHP_FE(confirm_geo_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(geo_verify_hash,	NULL)
	PHP_FE(geo_encode,	NULL)
	PHP_FE(geo_decode,	NULL)
	PHP_FE(geo_get_neighbors,	NULL)
	PHP_FE(geo_get_adjacent,	NULL)
	PHP_FE(geo_distance, NULL)
	PHP_FE(geo_simple_distance, NULL)
	PHP_FE_END	/* Must be the last line in geo_functions[] */
};
/* }}} */

/* {{{ geo_module_entry
 */
zend_module_entry geo_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"geo",
	geo_functions,
	PHP_MINIT(geo),
	PHP_MSHUTDOWN(geo),
	PHP_RINIT(geo),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(geo),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(geo),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_GEO
ZEND_GET_MODULE(geo)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("geo.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_geo_globals, geo_globals)
    STD_PHP_INI_ENTRY("geo.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_geo_globals, geo_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_geo_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_geo_init_globals(zend_geo_globals *geo_globals)
{
	geo_globals->global_value = 0;
	geo_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(geo)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(geo)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(geo)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(geo)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(geo)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "geo support", "enabled");
	php_info_print_table_row(2, "functions", "geo_simple_distance,geo_distance,geo_encode,geo_decode,geo_verify_hash,geo_get_neighbors");
	php_info_print_table_row(2, "author", PHP_GEO_AUTHOR);
	php_info_print_table_row(2, "version", PHP_GEO_VERSION);
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_geo_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_geo_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "geo", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/

/* {{{ proto bool geo_verify_hash(string hash)
   ; */
PHP_FUNCTION(geo_verify_hash)
{
	char *hash = NULL;
	int argc = ZEND_NUM_ARGS();
	int hash_len = 0;

	if (zend_parse_parameters(argc TSRMLS_CC, "s", &hash, &hash_len) == FAILURE) {
		RETURN_FALSE;
	}

	if(hash_len > 0 && GEO_verify_hash(hash)) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string geo_encode(double latitude, double longitude, long length)
   ; */
PHP_FUNCTION(geo_encode)
{
	int argc = ZEND_NUM_ARGS();
	long length;
	double latitude;
	double longitude;

	if (zend_parse_parameters(argc TSRMLS_CC, "ddl", &latitude, &longitude, &length) == FAILURE) {
		RETURN_FALSE;
	}
	// check the coordinates
	if(latitude < -90.0 || latitude > 90.0 || longitude < -180.0 || longitude > 180.0) {
		RETURN_FALSE;
	}
	// check the length
	if(length < 1 || length > MAX_HASH_LENGTH) {
		RETURN_FALSE;
	}

	char * hash = GEO_encode(latitude, longitude, length);
	if(hash == NULL) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(hash, strlen(hash), 0);
}
/* }}} */

/* {{{ proto array geo_decode(string hash)
   ; */
PHP_FUNCTION(geo_decode)
{
	char *hash = NULL;
	int argc = ZEND_NUM_ARGS();
	int hash_len = 0;

	if (zend_parse_parameters(argc TSRMLS_CC, "s", &hash, &hash_len) == FAILURE) {
		RETURN_FALSE;
	}

	if(hash_len == 0) {
		RETURN_FALSE;
	}

	GEO_area * area = GEO_decode(hash);
	if(area == NULL) {
		RETURN_FALSE;
	}
	array_init(return_value);

	add_assoc_double(return_value, "lat", (area->latitude.max + area->latitude.min) / 2.0);
	add_assoc_double(return_value, "lng", (area->longitude.max + area->longitude.min) / 2.0);
	add_assoc_double(return_value, "north", area->latitude.max);
	add_assoc_double(return_value, "south", area->latitude.min);
	add_assoc_double(return_value, "east", area->longitude.max);
	add_assoc_double(return_value, "west", area->longitude.min);
	GEO_free_area(area);
}
/* }}} */

/* {{{ proto mixed geo_get_neighbors(string hash)
   ; */
PHP_FUNCTION(geo_get_neighbors)
{
	char *hash = NULL;
	int argc = ZEND_NUM_ARGS();
	int hash_len = 0;

	if (zend_parse_parameters(argc TSRMLS_CC, "s", &hash, &hash_len) == FAILURE) {
		RETURN_FALSE;
	}
	if(hash_len == 0) {
		RETURN_FALSE;
	}

	GEO_neighbors * neighbors = GEO_get_neighbors(hash);
	if(neighbors == NULL) {
		RETURN_FALSE;
	}

#define ADD_ASSOC_STRINGL(return_value, neighbors, name) \
	add_assoc_stringl((return_value), #name, neighbors->name, strlen(neighbors->name), 0)

	array_init(return_value);
	ADD_ASSOC_STRINGL(return_value, neighbors, north);
	ADD_ASSOC_STRINGL(return_value, neighbors, east);
	ADD_ASSOC_STRINGL(return_value, neighbors, west);
	ADD_ASSOC_STRINGL(return_value, neighbors, south);
	ADD_ASSOC_STRINGL(return_value, neighbors, north_east);
	ADD_ASSOC_STRINGL(return_value, neighbors, south_east);
	ADD_ASSOC_STRINGL(return_value, neighbors, north_west);
	ADD_ASSOC_STRINGL(return_value, neighbors, south_west);

	//GEO_free_neighbors(neighbors);
	efree(neighbors);
}
/* }}} */

/* {{{ proto string geo_get_adjacent(string hash, long direction)
   ; */
PHP_FUNCTION(geo_get_adjacent)
{
	char *hash = NULL;
	int argc = ZEND_NUM_ARGS();
	int hash_len;
	long direction;

	if (zend_parse_parameters(argc TSRMLS_CC, "sl", &hash, &hash_len, &direction) == FAILURE) {
		RETURN_FALSE;
	}

	if(direction < GEO_NORTH || direction > GEO_SOUTH) {
		RETURN_FALSE;
	}

	char * _hash = GEO_get_adjacent(hash, direction);
	if(_hash == NULL) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(_hash, strlen(_hash), 0);
}
/* }}} */



/* {{{ proto string geo_simple_distance(long lat1, long lng1, long lat2, long lng2)
   ; */
PHP_FUNCTION(geo_simple_distance)
{
	int argc = ZEND_NUM_ARGS();
	double lat1,lng1,lat2,lng2;
	if (zend_parse_parameters(argc TSRMLS_CC, "dddd",  &lat1, &lng1, &lat2, &lng2) == FAILURE){
		RETURN_FALSE;
	}
    RETURN_DOUBLE(GEO_simple_distance(lat1, lng1, lat2, lng2));
}
/* }}} */

/* {{{ proto string geo_distance(long lat1, long lng1, long lat2, long lng2)
   ; */
PHP_FUNCTION(geo_distance)
{
	int argc = ZEND_NUM_ARGS();
	double lat1,lng1,lat2,lng2;

	if (zend_parse_parameters(argc TSRMLS_CC, "dddd", &lat1, &lng1, &lat2, &lng2) == FAILURE){
		RETURN_FALSE;
	}
	RETURN_DOUBLE(GEO_distance(lat1, lng1, lat2, lng2));
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
