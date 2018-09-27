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



ZEND_BEGIN_ARG_INFO_EX(arginfo_dubo_storage_factory_create, 0, 0, 2)
	ZEND_ARG_INFO(0, type) /* string */
	ZEND_ARG_INFO(0, config) /* array */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dubo_file_storage_get, 0, 0, 1)
	ZEND_ARG_INFO(0, key) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dubo_storage_abstract_construct, 0, 0, 1)
	ZEND_ARG_INFO(0, config) /* array */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dubo_storage_abstract_getconfig, 0, 0, 1)
	ZEND_ARG_INFO(0, key) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(idubbo_storage_get_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, key) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(idubbo_storage_set_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, key) /* string */
	ZEND_ARG_INFO(0, value) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(idubbo_storage_getallkeys_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()





zend_class_entry *dubbo_storage_factory_class_entry;
zend_class_entry *dubbo_storage_abstract_class_entry;
zend_class_entry *dubbo_file_storage_class_entry;
zend_class_entry *idubbo_storage_interface_entry;





/****************************Begin DubboStorageFactory**************************/
/*
create storage
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
		//TODO:暂没有实现
	}else{
		//TODO:抛出异常
	}
}
/*************************End DubboStorageFactory**********************************/



/****************************Begin DubboStorageAbstract**************************/
//construct function
static PHP_METHOD(DubboStorageAbstract, __construct)
{
	zval *array;
	zval* self=getThis();
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &array)) {
		return;
	}
	zend_update_property(Z_OBJCE_P(self), self, "config", sizeof("config")-1, array TSRMLS_CC);
	self->value.obj.handlers = &std_object_handlers;
}

//get config
static PHP_METHOD(DubboStorageAbstract, getConfig)
{
	char *key;
	zval* self;
	zval* config;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key)) {
		return;
	}

	self=getThis();
	config = zend_read_property(Z_OBJCE_P(self), self, "config", sizeof("config")-1, 1 TSRMLS_DC);

//todo:还要加上object handle
	if (FAILURE != zend_hash_find(HASH_OF(config), key, sizeof(key)-1, (void **)&return_value)){
		return;
	}
	
	RETURN_FALSE;
}


/****************************End DubboStorageAbstract**********************************/






/****************************Begin DubboFileStorage**************************/
/*
create storage
*/
static PHP_METHOD(DubboFileStorage, get)
{
	
}
/*************************End DubboFileStorage**********************************/





//DubboStorageFactory functions
const zend_function_entry dubbo_storage_factory_functions[] = {
	PHP_ME(DubboStorageFactory, create,		arginfo_dubo_storage_factory_create,		ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_FE_END	/* Must be the last line in hessian_functions[] */
};

//IDubboStorage interface
const zend_function_entry idubbo_storage_interface_functions[] = {
	PHP_ABSTRACT_ME(IDubboStorage, get, idubbo_storage_get_arginfo)
	PHP_ABSTRACT_ME(IDubboStorage, set, idubbo_storage_get_arginfo)
	PHP_ABSTRACT_ME(IDubboStorage, getAllKeys, idubbo_storage_getallkeys_arginfo)
	PHP_FE_END
};

//DubboStorageAbstract functions
const zend_function_entry dubbo_storage_abstract_functions[] = {
	PHP_ME(DubboStorageAbstract, getConfig, 	arginfo_dubo_storage_abstract_getconfig,		ZEND_ACC_PROTECTED)
	PHP_ME(DubboStorageAbstract, __construct,		arginfo_dubo_storage_abstract_construct,		ZEND_ACC_PUBLIC)
	PHP_FE_END	/* Must be the last line in hessian_functions[] */
};


//DubboFileStorage functions
const zend_function_entry dubbo_file_storage_functions[] = {
	PHP_ME(DubboFileStorage, get,		arginfo_dubo_file_storage_get,		ZEND_ACC_PUBLIC)
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
