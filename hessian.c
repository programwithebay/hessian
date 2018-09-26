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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_hessian.h"


ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_construct, 0, 0, 1)
	ZEND_ARG_INFO(0, config) /* array */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_setConnectTimeout, 0, 0, 1)
	ZEND_ARG_INFO(0, timeout) /* long */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_callService, 0, 0, 3)
	ZEND_ARG_INFO(0, serviceName) /* string serviceName */
	ZEND_ARG_INFO(0, method) /* string method */
	ZEND_ARG_INFO(0, params) /* array params */
ZEND_END_ARG_INFO()



/* If you declare any globals in php_hessian.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(hessian)
*/

/* True global resources - no need for thread safety here */
static int le_hessian;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("hessian.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_hessian_globals, hessian_globals)
    STD_PHP_INI_ENTRY("hessian.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_hessian_globals, hessian_globals)
PHP_INI_END()
*/
/* }}} */

zend_class_entry *dubbo_client_class_entry;


/*
construct
*/
static PHP_METHOD(DubboClient, __construct)
{
	RETURN_TRUE;
}


/*
setConnectTimeout
*/
static PHP_METHOD(DubboClient, setConnectTimeout)
{
	zval* self=getThis();
	long timeout;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &timeout)) {
		return;
	}

	zend_update_property_long(dubbo_client_class_entry, self, "connectTimeout", sizeof("connectTimeout")-1,  timeout);

	return;
}


/*
call service
*/
static PHP_METHOD(DubboClient, callService)
{
	RETURN_TRUE;
}


const zend_function_entry hessian_functions[] = {
	PHP_ME(DubboClient, __construct,		arginfo_hessian_construct,		ZEND_ACC_PUBLIC)
	PHP_ME(DubboClient, setConnectTimeout,		arginfo_hessian_setConnectTimeout,		ZEND_ACC_PUBLIC)
	PHP_ME(DubboClient, callService,		arginfo_hessian_callService,		ZEND_ACC_PUBLIC)
	PHP_FE_END	/* Must be the last line in hessian_functions[] */
};



/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_hessian_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_hessian_init_globals(zend_hessian_globals *hessian_globals)
{
	hessian_globals->global_value = 0;
	hessian_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(hessian)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/

	zend_class_entry ce;

	//init DubboClient Class
	INIT_CLASS_ENTRY(ce, "DubboClient", hessian_functions);
	dubbo_client_class_entry = zend_register_internal_class(&ce TSRMLS_CC);
	zend_declare_property_long(dubbo_client_class_entry, "connectTimeout", sizeof("connectTimeout")-1, 0, ZEND_ACC_PROTECTED TSRMLS_CC);
	
	
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(hessian)
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
PHP_RINIT_FUNCTION(hessian)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(hessian)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(hessian)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "hessian support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ hessian_functions[]
 *
 * Every user visible function must have an entry in hessian_functions[].
 */



/* }}} */

/* {{{ hessian_module_entry
 */
zend_module_entry hessian_module_entry = {
	STANDARD_MODULE_HEADER,
	"hessian",
	hessian_functions,
	PHP_MINIT(hessian),
	PHP_MSHUTDOWN(hessian),
	PHP_RINIT(hessian),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(hessian),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(hessian),
	PHP_HESSIAN_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_HESSIAN
ZEND_GET_MODULE(hessian)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
