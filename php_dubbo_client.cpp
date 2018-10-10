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
extern "C" {
	#include "php.h"
	#include "php_ini.h"
	#include "php_hessian_int.h"
	#include "ext/standard/info.h"
	#include <fcntl.h>
	#include <sys/types.h>
	#include <sys/stat.h>
}

#include "dubbo_client.h"

zend_object_handlers dubbo_client_object_handlers;

void dubbo_client_free_storage(void *object TSRMLS_DC)
{
    dubbo_client_object *obj = (dubbo_client_object *)object;
    delete obj->client; 

    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);

    efree(obj);
}

zend_object_value dubbo_client_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    dubbo_client_object *obj = (dubbo_client_object *)emalloc(sizeof(dubbo_client_object));
    memset(obj, 0, sizeof(DubboClient));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
    zend_hash_copy(obj->std.properties, Z_ARRVAL_P(*(type->default_properties_table)),
        (copy_ctor_func_t)zval_add_ref, (void *)&tmp, sizeof(zval *));

    retval.handle = zend_objects_store_put(obj, NULL,
        dubbo_client_free_storage, NULL TSRMLS_CC);
    retval.handlers = &dubbo_client_object_handlers;

    return retval;
}



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
	//read storage 
	zval *array;
	zval* self;
	zval *storage, *config_file, *config_array, *servic_config, *dto_map, *redis_config;
	zval *new_config;
	zval** value_ptr;
	zval *type, *param;
	zval *function_name;
	zval retval;
	zval *params[2];
	long depth = 512;
	long options = 0;
	int fd;
	int nread;
	char buf[8192];	//max length is 8k
	DubboClient *client;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &array)) {
		php_error_docref(NULL, E_ERROR, "config must be an array");
		return;
	}

	if (FAILURE == zend_hash_find(HASH_OF(array), ZEND_STRS("configFile"), (void **)&value_ptr)){
		php_error_docref(NULL, E_ERROR, "configFile is not set");
	}
	config_file = *value_ptr;
	if (FAILURE == zend_hash_find(HASH_OF(array), ZEND_STRS("storage"), (void **)&value_ptr)){
		php_error_docref(NULL, E_ERROR, "storage is not set");
	}
	storage = *value_ptr;
	if (FAILURE == zend_hash_find(HASH_OF(storage), ZEND_STRS("type"), (void **)&value_ptr)){
		php_error_docref(NULL, E_ERROR, "['storage']['type'] is not set");
	}
	type = *value_ptr;
	if (FAILURE == zend_hash_find(HASH_OF(storage), ZEND_STRS("param"), (void **)&value_ptr)){
		php_error_docref(NULL, E_ERROR, "['storage']['param'] is not set");
	}
	param = *value_ptr;


		
	
	//init
	//options |=  PHP_JSON_OBJECT_AS_ARRAY;
	//read file
	fd = open(Z_STRVAL_P(config_file), O_RDONLY);
	if (-1 == fd){
		php_error_docref(NULL, E_ERROR, "open file %s error", Z_STRVAL_P(config_file));
	}
	nread = read(fd, buf, 8191);
	if (-1 == nread){
		close(fd);
		php_error_docref(NULL, E_ERROR, "read file %s error", Z_STRVAL_P(config_file));
	}
	close(fd);

	/*
	php_json_decode_ex(config_array, buf, nread, options, depth TSRMLS_CC);
	if (JSON_G(error_code) > 0){
		php_error_docref(NULL, E_ERROR, "file %s is not a json file", Z_STRVAL_P(config_file));
	}
	if (FAILURE == zend_hash_find(HASH_OF(config_array), ZEND_STRS("zk"), (void **)&value_ptr)){
		php_error_docref(NULL, E_ERROR, "configFile zk is not set");
	}
	servic_config = * value_ptr;
	//isset redis?
	if (SUCCESS == zend_hash_find(HASH_OF(config_array), ZEND_STRS("redis"), (void **)&value_ptr)){
		redis_config = *value_ptr;
		php_array_merge(Z_ARRVAL_P(servic_config), Z_ARRVAL_P(redis_config), 0 TSRMLS_CC);
		//zend_hash_merge(Z_ARRVAL_P(redis_config), Z_ARRVAL_P(redis_config), (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval *), 1);
	}else{
		zend_update_property(dubbo_client_class_entry, self, ZEND_STRL("serviceConfig"),  servic_config);
	}
	if (FAILURE == zend_hash_find(HASH_OF(config_array), ZEND_STRS("dtoMap"), (void **)&value_ptr)){
		php_error_docref(NULL, E_ERROR, "configFile dtoMap is not set");
	}
	dto_map = *value_ptr;
	zend_update_property(dubbo_client_class_entry, self, ZEND_STRL("dtoMapConfig"),  dto_map);
	*/
	
	//init c++ class
	client = new DubboClient();
    dubbo_client_object *obj = (dubbo_client_object *)zend_object_store_get_object(self TSRMLS_CC);
    obj->client= client;
	
}


/*
setConnectTimeout
*/
static PHP_METHOD(DubboClient, setConnectTimeout)
{
	zval* self=getThis();
	long timeout;
	DubboClient *client;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &timeout)) {
		return;
	}

	dubbo_client_object *obj = (dubbo_client_object *)zend_object_store_get_object(self TSRMLS_CC);
    client = obj->client;
	client->setConnectTimeout(timeout);

	return;
}


/*
call service
*/
static PHP_METHOD(DubboClient, callService)
{
	//call function
	zval *function_name;
	zval *params;
	zval retval;
	zval *cls_service;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zval *retval_ptr;
	
	

	
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
