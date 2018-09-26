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
#include "dubbo_storage.h"


ZEND_BEGIN_ARG_INFO_EX(arginfo_dubo_storage_factory_create, 0, 0, 2)
	ZEND_ARG_INFO(0, type) /* string */
	ZEND_ARG_INFO(0, config) /* array */
ZEND_END_ARG_INFO()




zend_class_entry *dubbo_storage_factory_class_entry;


/*
construct
*/
static PHP_METHOD(DubboStorageFactory, create)
{
	RETURN_TRUE;
}


/*
setConnectTimeout
*/
static PHP_METHOD(DubboStorageFactory, create)
{
	zval *array;
	char *type;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa", &type, array)) {
		return;
	}

	if (strncasecmp(type, "file", siezof("file")-1)){
	}else if (strncasecmp(type, "apc", siezof("apc")-1)){
	}else{
		//TODO:Å×³öÒì³£
	}
}



const zend_function_entry dubbo_storage_factory_functions[] = {
	PHP_ME(DubboStorageFactory, create,		arginfo_dubo_storage_factory_create,		ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
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
