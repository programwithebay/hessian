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
#include "php_hessian_int.h"
#include "ext/standard/info.h"


ZEND_BEGIN_ARG_INFO_EX(arginfo_dubbo_service_construct, 0, 0, 3)
	ZEND_ARG_INFO(0, name) /* array */
	ZEND_ARG_INFO(0, providers) /* array */
	ZEND_ARG_INFO(0, consumers) /* array */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dubbo_service_setstorage, 0, 0, 1)
	ZEND_ARG_INFO(0, storage) /* array */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dubbo_service_setoption, 0, 0, 2)
	ZEND_ARG_INFO(0, name) /* array */
	ZEND_ARG_INFO(0, value) /* array */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dubbo_service_setdtomap, 0, 0, 1)
	ZEND_ARG_INFO(0, dtoMap) /* array */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dubbo_service_save, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dubbo_service_call, 0, 0, 2)
	ZEND_ARG_INFO(0, method) /* array */
	ZEND_ARG_INFO(0, params) /* array */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dubbo_service_failed, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dubbo_getproviderscount, 0, 0, 0)
ZEND_END_ARG_INFO()


zend_class_entry *dubbo_service_class_entry;

/*
	get service by name
*/
void get_service_by_name(zval *name, zval *storage, zval* retval){
	/*
	$value = $storage->get($name);
    	$res   = new self($name, $value['providers'], $value['consumers']);
    	return $res;
    */
	zval function_name;
	zval *params[1];
	zval *property;
	
	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "get", 1);
	params[0] = name;
	//dont check is error
	call_user_function(NULL, &storage, &function_name, retval, 1, params TSRMLS_CC);

	zval_dtor(&function_name);
}

/*
DubboService construct
*/
static PHP_METHOD(DubboService, __construct)
{
	
	zval *name, *providers, *consumers;
	zval *self;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &name, &providers, &consumers)) {
		php_error_docref(NULL, E_WARNING, "parse DubboService::__construct param error");
	}

	if (Z_TYPE_P(name) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "name is not a string");
	}
	if (Z_TYPE_P(providers) != IS_ARRAY){
		php_error_docref(NULL, E_WARNING, "providers is not a string");
	}
	if (Z_TYPE_P(consumers) != IS_ARRAY){
		php_error_docref(NULL, E_WARNING, "consumers is not a string");
	}
	
	self = getThis();
	zend_update_property(dubbo_service_class_entry, self, ZEND_STRL("name"), name TSRMLS_DC);
	zend_update_property(dubbo_service_class_entry, self, ZEND_STRL("providers"), providers TSRMLS_DC);
	zend_update_property(dubbo_service_class_entry, self, ZEND_STRL("consumers"), consumers TSRMLS_DC);
}

/*
DubboService construct
*/
static PHP_METHOD(DubboService, setStorage)
{
	zval *storage;
	zval *self;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &storage)) {
		php_error_docref(NULL, E_WARNING, "parse DubboService::setStorage param error");
	}
	//是否父子类关系
	if (!instanceof_function_ex(zend_get_class_entry(storage), idubbo_storage_interface_entry, 0 TSRMLS_CC)){
		php_error_docref(NULL, E_WARNING, "storage does not implements IDubboStorage");
	}

	self = getThis();
	zend_update_property(dubbo_service_class_entry, self, ZEND_STRL("storage"), storage TSRMLS_DC);
}


/*
DubboService construct
*/
static PHP_METHOD(DubboService, setOption)
{
	zval *name, *value;
	zval *self;
	zval *options;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &name, &value)) {
		php_error_docref(NULL, E_WARNING, "parse DubboService::setOption param error");
	}
	if (Z_TYPE_P(name) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "parse name is not string");
	}

	self = getThis();
	options = zend_read_property(dubbo_service_class_entry, self, ZEND_STRL("options"), 1 TSRMLS_DC);
	if (Z_TYPE_P(options) != IS_ARRAY){
		//init array
		ALLOC_ZVAL(options);
		array_init_size(options, 8);
		zend_update_property(dubbo_service_class_entry, self, ZEND_STRL("options"), options TSRMLS_DC);
	}
	zend_hash_update(Z_ARRVAL_P(options), Z_STRVAL_P(name), Z_STRLEN_P(name), value, sizeof(zval *), NULL);
}


/*
DubboService construct
*/
static PHP_METHOD(DubboService, setDtoMap)
{
	zval *dtomap;
	zval *self;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &dtomap)) {
		php_error_docref(NULL, E_WARNING, "parse DubboService::setDtoMap param error");
	}
	if (Z_TYPE_P(dtomap) == IS_ARRAY){
		self = getThis();
		zend_update_property(dubbo_service_class_entry, self, ZEND_STRL("dtoMap"), dtomap TSRMLS_DC);
	}
}


/*
DubboService construct
*/
static PHP_METHOD(DubboService, save)
{
	zval *value;
	zval *providers, *consumers, *storage, *name;
	zval *self;
	zval *params[2];
	zval function_name;

	self = getThis();
	providers = zend_read_property(dubbo_service_class_entry, self, ZEND_STRL("providers"), 1 TSRMLS_DC);
	consumers = zend_read_property(dubbo_service_class_entry, self, ZEND_STRL("consumers"), 1 TSRMLS_DC);
	storage = zend_read_property(dubbo_service_class_entry, self, ZEND_STRL("storage"), 1 TSRMLS_DC);
	ALLOC_ZVAL(value);
	array_init_size(value, 2);

	//todo strlen or strlen+1
	zend_hash_update(Z_ARRVAL_P(value), "providers", strlen("providers"), providers, sizeof(zval *), NULL);
    zend_hash_update(Z_ARRVAL_P(value), "consumers", strlen("consumers"), consumers, sizeof(zval *), NULL);

	//set storage
	name = zend_read_property(dubbo_service_class_entry, self, ZEND_STRL("name"), 1 TSRMLS_DC);
	params[0] = name;
	params[1] = value;
	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "set", 1);
	call_user_function(NULL, &storage, &function_name, NULL, 2, params TSRMLS_CC);
	zval_dtor(&function_name);
}

/*
DubboService construct
*/
static PHP_METHOD(DubboService, call)
{
	
}


/*
DubboService construct
*/
static PHP_METHOD(DubboService, failed)
{
	zval *index, *providers;
	zval *self;
	HashTable *ht_providers;
	ulong hval;
	ulong h;

	self = getThis();
	providers = zend_read_property(dubbo_service_class_entry, self, ZEND_STRL("providers"), 1 TSRMLS_DC);
	if (Z_TYPE_P(providers) != IS_ARRAY){
		return;
	}
	index = zend_read_property(dubbo_service_class_entry, self, ZEND_STRL("curProviderIndex"), 1 TSRMLS_DC);
	//unset($this->providers[$index]);
	ht_providers = Z_ARRVAL_P(providers);
	convert_to_string(index);
	h = zend_inline_hash_func(Z_STRVAL_P(index), Z_STRLEN_P(index));
	hval = h & ht_providers->nTableMask;
	zend_hash_index_del(ht_providers, hval);
}


/*
DubboService construct
*/
static PHP_METHOD(DubboService, getProvidersCount)
{
	zval *providers;
	zval *self;

	self = getThis();
	providers = zend_read_property(dubbo_service_class_entry, self, ZEND_STRL("providers"), 1 TSRMLS_DC);
	if (Z_TYPE_P(providers) == IS_ARRAY){
		RETURN_LONG(Z_ARRVAL_P(providers)->nNumOfElements);
	}else{
		RETURN_LONG(0);
	}
}



const zend_function_entry dubbo_service_functions[] = {
	PHP_ME(DubboService, __construct,		arginfo_dubbo_service_construct,		ZEND_ACC_PUBLIC)
	PHP_ME(DubboService, setStorage,		arginfo_dubbo_service_setstorage,		ZEND_ACC_PUBLIC)
	PHP_ME(DubboService, setOption,			arginfo_dubbo_service_setoption,		ZEND_ACC_PUBLIC)
	PHP_ME(DubboService, setDtoMap,			arginfo_dubbo_service_setdtomap,		ZEND_ACC_PUBLIC)
	PHP_ME(DubboService, save,				arginfo_dubbo_service_save,				ZEND_ACC_PUBLIC)
	PHP_ME(DubboService, call,				arginfo_dubbo_service_call,				ZEND_ACC_PUBLIC)
	PHP_ME(DubboService, failed,			arginfo_dubbo_service_failed,				ZEND_ACC_PUBLIC)
	PHP_ME(DubboService, getProvidersCount,				arginfo_dubbo_getproviderscount,				ZEND_ACC_PUBLIC)
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
