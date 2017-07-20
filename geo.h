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
 
#ifndef PHP_GEO_H
#define PHP_GEO_H

extern zend_module_entry geo_module_entry;
#define phpext_geo_ptr &geo_module_entry

#ifdef PHP_WIN32
#	define PHP_GEO_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_GEO_API __attribute__ ((visibility("default")))
#else
#	define PHP_GEO_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#define PHP_GEO_VERSION "0.1.0" 
#define PHP_GEO_AUTHOR "lizhenglin@huoyunren.com"

PHP_MINIT_FUNCTION(geo);
PHP_MSHUTDOWN_FUNCTION(geo);
PHP_RINIT_FUNCTION(geo);
PHP_RSHUTDOWN_FUNCTION(geo);
PHP_MINFO_FUNCTION(geo);

PHP_FUNCTION(confirm_geo_compiled);	/* For testing, remove later. */
PHP_FUNCTION(geo_verify_hash);
PHP_FUNCTION(geo_encode);
PHP_FUNCTION(geo_decode);
PHP_FUNCTION(geo_get_neighbors);
PHP_FUNCTION(geo_get_adjacent);
PHP_FUNCTION(geo_simple_distance);
PHP_FUNCTION(geo_distance);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(geo)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(geo)
*/

/* In every utility function you add that needs to use variables 
   in php_geo_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as GEO_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define GEO_G(v) TSRMG(geo_globals_id, zend_geo_globals *, v)
#else
#define GEO_G(v) (geo_globals.v)
#endif

#endif	/* PHP_GEO_H */

