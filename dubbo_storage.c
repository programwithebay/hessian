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
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ext/standard/php_smart_str_public.h"
#include "ext/json/php_json.h"
#include "ext/standard/info.h"
#include "ext/standard/php_filestat.h"
#include "hessian_common.h"



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
		Z_TYPE_P(return_value) = IS_OBJECT;
		object_init_ex(return_value, dubbo_file_storage_class_entry);
		Z_SET_REFCOUNT_P(return_value, 1);
		Z_SET_ISREF_P(return_value);
	
	}else{
		//TODO:抛出异常
		php_error_docref(NULL, E_ERROR, "unsupport storage:%s", type);
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
	config = zend_read_property(dubbo_storage_abstract_class_entry, self, ZEND_STRL("config"), 1 TSRMLS_DC);


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
	options |=  PHP_JSON_OBJECT_AS_ARRAY;

	php_json_decode_ex(return_value, str, str_len, options, depth TSRMLS_CC);
}




/****************************End DubboStorageAbstract**********************************/






/****************************Begin DubboFileStorage**************************/
//check file
//mode 0:file, 1:dir
//return 
//1:file
//2:dir
char stat_file(char *file_name, char mode){
	char res;
	struct stat file_stat;

	if (stat(file_name, &file_stat) < 0)
    {
        php_error_docref(NULL, E_ERROR, "get file:%s info error", file_name);
		return;
    }
	switch(mode){
		case 0:
			//test is a file
			res = S_ISREG(file_stat.st_mode);
			break;
		case 1:
			//test ia a dir
			res = S_ISDIR(file_stat.st_mode);
			break;
	}
	return res;
}


/*
get class basePath
*/
zval* get_dubbo_file_storage_basepath(zval *this){
	zval *config;
	zval **value_ptr;
	char *file_name;
	zval *value;
	int len, i;
	

	config = zend_read_property(dubbo_file_storage_class_entry, this, ZEND_STRL(BASE_PATH), 1 TSRMLS_DC);
	if (i_zend_is_true(config)){
		return config;
	}

	//read property from array
	config = zend_read_property(dubbo_file_storage_class_entry, this, ZEND_STRL("config"), 1 TSRMLS_DC);
	if (SUCCESS != zend_hash_find(HASH_OF(config), ZEND_STRS(BASE_PATH), (void **)&value_ptr)){
		php_error_docref(NULL, E_ERROR, "base_path is not set");
		return;
	}

	//是否为目录
	file_name = Z_STRVAL_PP(value_ptr);
    if (stat_file(file_name, 1) < 0)
    {
        php_error_docref(NULL, E_ERROR, "%s is not a dir", file_name);
		return;
    }
	
	len = Z_STRLEN_PP(value_ptr);
	i = len - 1;
	while(i >= 0){
		if ('/' == *(file_name+i)){
			*(file_name+i) = 0;
			i--;
			len--;
			continue;
		}else{
			break;
		}
	}
	Z_STRLEN_PP(value_ptr) = len;
	zend_update_property(Z_OBJCE_P(this), this, ZEND_STRL(BASE_PATH), *value_ptr TSRMLS_CC);

	return *value_ptr;
}

/*
get by name
*/
static PHP_METHOD(DubboFileStorage, get)
{
	char *str;
	zval* self;
	int str_len;
	zval *base_path;
	char *path;
	zval *value;
	int fd;
	int nread;
	char buf[8192];	//max length is 8k
	long options = 0;
	long depth = 512;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}

	self=getThis();
	
	base_path = get_dubbo_file_storage_basepath(self);
	path = pemalloc(256, 0);
	sprintf(path,"%s/%s", Z_STRVAL_P(base_path), str);
	str_efree_rel(base_path->value.str.val);
	Z_STRVAL_P(base_path) = path;
	Z_STRLEN_P(base_path) = sizeof(path) - 1;	//todo -1 or not?

	//is a file?
	if (stat_file(path, 0) < 0){
        php_error_docref(NULL, E_ERROR, "file:%s is not a file", path);
		return;
    }

	//get content
	fd = open(path, O_RDONLY);
	if (-1 == fd){
		php_error_docref(NULL, E_ERROR, "open file %s error", path);
	}
	nread = read(fd, buf, 8191);
	if (-1 == nread){
		
		php_error_docref(NULL, E_ERROR, "read file %s error", path);
		close(fd);
	}
	close(fd);
	options |=  PHP_JSON_OBJECT_AS_ARRAY;
	php_json_decode_ex(return_value, buf, nread, options, depth TSRMLS_CC);
	
}

/*
set
*/
static PHP_METHOD(DubboFileStorage, set)
{
	char *key, *value;
	zval* self;
	int key_len, value_len;
	zval *base_path;
	char *path;
	int fd;
	int nwrite;
	long options = 0;
	long depth = 512;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &key, &key_len, &value, &value_len) == FAILURE) {
		return;
	}

	self=getThis();
	base_path = get_dubbo_file_storage_basepath(self);
	path = pemalloc(256, 0);
	sprintf(path,"%s/%s", Z_STRVAL_P(base_path), key);
	str_efree_rel(base_path->value.str.val);
	Z_STRVAL_P(base_path) = path;
	Z_STRLEN_P(base_path) = sizeof(path) - 1;	//todo -1 or not?


	//get content
	fd = open(path, O_WRONLY|O_CREAT);
	if (-1 == fd){
		php_error_docref(NULL, E_ERROR, "open file %s error", path);
	}
	nwrite = write(fd, value, value_len);
	if (value_len != nwrite){
		
		php_error_docref(NULL, E_ERROR, "write file %s error", path);
		close(fd);
	}

	close(fd);
	
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
