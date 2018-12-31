/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_HESSIAN_H
#define PHP_HESSIAN_H

struct _hessian_parsing_rule{
	char *type;
	char *func;
	char *desc;
};

typedef struct _hessian_parsing_rule hessian_parsing_rule;

extern zend_module_entry hessian_module_entry;
#define phpext_hessian_ptr &hessian_module_entry

#define PHP_HESSIAN_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_HESSIAN_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_HESSIAN_API __attribute__ ((visibility("default")))
#else
#	define PHP_HESSIAN_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     
*/
ZEND_BEGIN_MODULE_GLOBALS(hessian)
	hessian_parsing_rule hessian2_rules[44];
	char	hessian2_symbols[256];
ZEND_END_MODULE_GLOBALS(hessian)

/* In every utility function you add that needs to use variables 
   in php_hessian_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as HESSIAN_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define HESSIAN_G(v) TSRMG(hessian_globals_id, zend_hessian_globals *, v)
#else
#define HESSIAN_G(v) (hessian_globals.v)
#endif

#endif	/* PHP_HESSIAN_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
