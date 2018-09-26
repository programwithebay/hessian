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
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"


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




/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
