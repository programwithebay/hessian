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
#include "ext/standard/php_smart_str_public.h"
#include "ext/json/php_json.h"
#include "ext/standard/info.h"
#include "ext/standard/php_filestat.h"



ZEND_BEGIN_ARG_INFO_EX(arginfo_dubo_storage_factory_create, 0, 0, 2)
	ZEND_ARG_INFO(0, type) /* string */
	ZEND_ARG_INFO(0, config) /* array */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dubo_file_storage_get, 0, 0, 1)
	ZEND_ARG_INFO(0, key) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dubo_file_storage_set, 0, 0, 2)
	ZEND_ARG_INFO(0, key) /* string */
	ZEND_ARG_INFO(0, value) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dubo_file_storage_get_base_path, 0, 0, 0)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_dubo_storage_abstract_construct, 0, 0, 1)
	ZEND_ARG_INFO(0, config) /* array */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dubo_storage_abstract_getconfig, 0, 0, 1)
	ZEND_ARG_INFO(0, key) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dubo_storage_abstract_encode, 0, 0, 1)
	ZEND_ARG_INFO(0, key) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dubo_storage_abstract_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, value) /* string */
ZEND_END_ARG_INFO()



ZEND_BEGIN_ARG_INFO_EX(idubbo_storage_get_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, key) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(idubbo_storage_set_arginfo, 0, 0, 2)
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

	zend_update_property(Z_OBJCE_P(self), self, ZEND_STRL("config"), array TSRMLS_CC);
}

//get config
static PHP_METHOD(DubboStorageAbstract, getConfig)
{
	char *key;
	zval* self;
	zval* config;
	zval** value_ptr;
	int key_len;
	int res;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len)) {
		return;
	}

	self=getThis();
	config = zend_read_property(Z_OBJCE_P(self), self, ZEND_STRL("config"), 1 TSRMLS_DC);


	res = zend_hash_find(HASH_OF(config), key, key_len, (void **)&value_ptr);
	if (SUCCESS == res){
		RETURN_ZVAL(*value_ptr, 1, 1);
		return;
	}
	
	RETURN_FALSE;
}


//encode
static PHP_METHOD(DubboStorageAbstract, encode)
{
	zval *val;
	smart_str buf = {0};
	long options = 0;
	char has_error = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &val) == FAILURE) {
		return;
	}

	switch (Z_TYPE_P(val))
	{
		case IS_NULL:
			smart_str_appendl(&buf, "null", 4);
			break;

		case IS_BOOL:
			if (Z_BVAL_P(val)) {
				smart_str_appendl(&buf, "true", 4);
			} else {
				smart_str_appendl(&buf, "false", 5);
			}
			break;

		case IS_LONG:
			smart_str_append_long(&buf, Z_LVAL_P(val));
			break;

		case IS_DOUBLE:
			{
				char *d = NULL;
				int len;
				double dbl = Z_DVAL_P(val);

				if (!zend_isinf(dbl) && !zend_isnan(dbl)) {
					len = spprintf(&d, 0, "%.*k", (int) EG(precision), dbl);
					smart_str_appendl(&buf, d, len);
					efree(d);
				} else {
					smart_str_appendc(&buf, '0');
					has_error = 1;
				}
			}
			break;

		case IS_STRING:
			json_escape_string(&buf, Z_STRVAL_P(val), Z_STRLEN_P(val), options TSRMLS_CC);
			break;

		case IS_OBJECT:
			if (instanceof_function(Z_OBJCE_P(val), php_json_serializable_ce TSRMLS_CC)) {
				json_encode_serializable_object(&buf, val, options TSRMLS_CC);
				break;
			}
			/* fallthrough -- Non-serializable object */
		case IS_ARRAY:
			json_encode_array(&buf, &val, options TSRMLS_CC);
			break;

		default:
			smart_str_appendl(&buf, "null", 4);
			has_error = 1;
			break;
	}

	if (has_error){
		ZVAL_FALSE(return_value);
	}else{
		ZVAL_STRINGL(return_value, buf.c, buf.len, 1);
	}

	smart_str_free(&buf);

}

//decode
static PHP_METHOD(DubboStorageAbstract, decode)
{
	char *str;
	int str_len;
	zend_bool assoc = 0; /* return JS objects as PHP objects by default */
	long depth = 512;
	long options = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|bll", &str) == FAILURE) {
		return;
	}


	if (!str_len) {
		RETURN_NULL();
	}

	/* For BC reasons, the bool $assoc overrides the long $options bit for PHP_JSON_OBJECT_AS_ARRAY */
	if (assoc) {
		options |=  PHP_JSON_OBJECT_AS_ARRAY;
	} else {
		options &= ~PHP_JSON_OBJECT_AS_ARRAY;
	}

	php_json_decode_ex(return_value, str, str_len, options, depth TSRMLS_CC);
}




/****************************End DubboStorageAbstract**********************************/






/****************************Begin DubboFileStorage**************************/
/*
get by name
*/
static PHP_METHOD(DubboFileStorage, get)
{
	
}

/*
set
*/
static PHP_METHOD(DubboFileStorage, set)
{
	
}

/*
get base path
*/
static PHP_METHOD(DubboFileStorage, getBasePath)
{
	zval* self;
	zval *config;
	zval **value_ptr;
	char *file_name;
	zval *value;


	self=getThis();
	config = zend_read_property(Z_OBJCE_P(self), self, ZEND_STRL("basePath"), 1 TSRMLS_DC);
	if (!i_zend_is_true(config)){
		config = zend_read_property(Z_OBJCE_P(self), self, ZEND_STRL("config"), 1 TSRMLS_DC);
		if (SUCCESS != zend_hash_find(HASH_OF(config), ZEND_STRL("base_path"), (void **)&value_ptr)){
			
		}else{
			php_error_docref(NULL, E_WARNING, "path:  is not set");
		}
		//是否为目录
		//@todo:目前是直接调用PHP函数，性能有点低
		file_name = Z_STRVAL(**value_ptr);
		php_stat(ZEND_STRL(file_name),FS_IS_DIR,value  TSRMLS_CC);
		if (Z_BVAL(*value)< 1){
			php_error_docref(NULL, E_WARNING, "path: %s is not a idr", Z_STRVAL(**value_ptr));
		}
		//todo:rtrim
		zend_update_property(Z_OBJCE_P(self), self, ZEND_STRL("base_path"), *value_ptr TSRMLS_CC);
		RETURN_ZVAL(*value_ptr, 1, 1);
		
	}else{
		RETURN_ZVAL(config, 1, 1);
	}
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
	PHP_ABSTRACT_ME(IDubboStorage, set, idubbo_storage_set_arginfo)
	PHP_ABSTRACT_ME(IDubboStorage, getAllKeys, idubbo_storage_getallkeys_arginfo)
	PHP_FE_END
};

//DubboStorageAbstract functions
const zend_function_entry dubbo_storage_abstract_functions[] = {
	PHP_ME(DubboStorageAbstract, __construct,		arginfo_dubo_storage_abstract_construct,		ZEND_ACC_PUBLIC)
	PHP_ME(DubboStorageAbstract, getConfig, 	arginfo_dubo_storage_abstract_getconfig,		ZEND_ACC_PROTECTED)
	PHP_ME(DubboStorageAbstract, encode,		arginfo_dubo_storage_abstract_encode,		ZEND_ACC_PROTECTED)
	PHP_ME(DubboStorageAbstract, decode,		arginfo_dubo_storage_abstract_decode,		ZEND_ACC_PROTECTED)
	PHP_FE_END	/* Must be the last line in hessian_functions[] */
};


//DubboFileStorage functions
const zend_function_entry dubbo_file_storage_functions[] = {
	PHP_ME(DubboFileStorage, get,		arginfo_dubo_file_storage_get,		ZEND_ACC_PUBLIC)
	PHP_ME(DubboFileStorage, set,		arginfo_dubo_file_storage_set,		ZEND_ACC_PUBLIC)
	PHP_ME(DubboFileStorage, getBasePath,		arginfo_dubo_file_storage_get_base_path,		ZEND_ACC_PROTECTED)
	
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
