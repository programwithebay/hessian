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
#include "Zend/zend_exceptions.h"
#include "ext/standard/info.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


zend_object_handlers dubbo_client_object_handlers;

struct _dubbo_client_entity{
	char *version;
	long connectTimeout;
	long executeTimeout;
	char retries;
	char *dubbo;
	char *loadbalance;
	char *methods;
	char *interface;
	char *owner;
	char *protocol;
	char *side;
	char *timestamp;
	HashTable *serviceConfig;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
};

typedef struct _dubbo_client_entity dubbo_client_entity;

typedef struct _dubbo_client_object dubbo_client_object;
struct _dubbo_client_object {
    zend_object std;
	dubbo_client_entity entity;
};




void dubbo_client_free_storage(void *object TSRMLS_DC)
{
    dubbo_client_object *obj = (dubbo_client_object *)object;
    //delete obj->client; 

    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);

    efree(obj);
}

zend_object_value dubbo_client_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    dubbo_client_object *obj = (dubbo_client_object *)emalloc(sizeof(dubbo_client_object));
    memset(obj, 0, sizeof(dubbo_client_entity));
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
set option with 2 params
*/
static void set_option_2param(zval *service_instance, zval *this_instance, char *param_name, zval* property_value){
	zval function_name;
	zval param;
	zval *params[2];
	zval *property;
	zval retval;
	
	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "setOption", 1);
	INIT_ZVAL(param);
	ZVAL_STRING(&param, param_name, 1);
	params[0] = &param;
	if (!property_value){
		params[1] = property_value;
	}else{
		property = zend_read_property(dubbo_client_class_entry, this_instance, ZEND_STRL(param_name), 1 TSRMLS_DC);
		params[1] = property;
	}
	//dont check is error
	call_user_function(NULL, &service_instance, &function_name, &retval, 2, params TSRMLS_CC);
	zval_dtor(&function_name);
	zval_dtor(&param);
}


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
	dubbo_client_entity *client_entity;
	
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

	dubbo_client_object *obj = (dubbo_client_object *)zend_object_store_get_object(self TSRMLS_CC);
    obj->entity.connectTimeout = timeout;

	return;
}

/**
set log callback
**/

static PHP_METHOD(DubboClient, setLogCallback)
{
	zval *retval_ptr = NULL;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
	zval *self;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "f", &fci, &fci_cache) == FAILURE) {
		return;
	}
	self = getThis();
	dubbo_client_object *obj = (dubbo_client_object *)zend_object_store_get_object(self TSRMLS_CC);
	obj->entity.fci =  fci;
	obj->entity.fci_cache = fci_cache;
}



/*
call service
*/
static PHP_METHOD(DubboClient, callService)
{
	//call function
	zval *arg_service_name, *arg_method, *arg_params;
	zval *cls_service;
	zval *service_config;
	zval *self;
	zval *storage;
	zval **value_ptr;
	zval *val;
	zval *dtomap;
	zval *params[2];
	zval param, function_name;
	zval function_failed;
	zval *property;
	int pid;
	int i, retries;
	zval *exception;
	zval *log_call_back;
	char *error;
	zend_bool is_callable;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
	zval log_param;
	char *err_msg;
	zval *error_message;
	zval *log_param_ptr;
	zval retval;
	zval *retval_ptr;
	dubbo_client_object *obj;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &arg_service_name, &arg_method, &arg_params)) {
		php_error_docref(NULL, E_WARNING, "parse DubboClient::callService param error");
	}
	self = getThis();
	obj = (dubbo_client_object *)zend_object_store_get_object(self TSRMLS_CC);
	
	storage = zend_read_property(dubbo_client_class_entry, self, ZEND_STRL("storage"), 1 TSRMLS_DC);
	if (Z_TYPE_P(storage) != IS_OBJECT){
		php_error_docref(NULL, E_WARNING, "DubboClient::storage is not a object");
		
	}
	//是否父子类关系
	if (!instanceof_function_ex(zend_get_class_entry(storage), idubbo_storage_interface_entry, 0 TSRMLS_CC)){
		php_error_docref(NULL, E_WARNING, "DubboClient::storage does not implements IDubboStorage");
	}
	
	cls_service = get_service_by_name(arg_service_name, storage);
	//test is is serviceConfig
	service_config = zend_read_property(dubbo_client_class_entry, self, ZEND_STRL("serviceConfig"), 1 TSRMLS_DC);
	if(Z_TYPE_P(service_config) != IS_ARRAY){
		php_error_docref(NULL, E_WARNING, "DubboClient::serviceConfig is not an array");
	}
	if (SUCCESS != zend_hash_find(HASH_OF(service_config), Z_STRVAL_P(arg_service_name),Z_STRLEN_P(arg_service_name), (void **)&value_ptr)){
		php_error_docref(NULL, E_ERROR, "serviceConfig:%s is not set", Z_STRLEN_P(arg_service_name));
		return;
	}
	service_config = *value_ptr;
	//ser version and group
	if (SUCCESS != zend_hash_find(HASH_OF(service_config), ZEND_STRL("version"), (void **)&value_ptr)){
		php_error_docref(NULL, E_ERROR, "serviceConfig['version'] is not set");
		return;
	}
	zend_update_property(dubbo_client_class_entry, self, ZEND_STRL("version"), *value_ptr TSRMLS_DC);
	if (SUCCESS != zend_hash_find(HASH_OF(service_config), ZEND_STRL("group"), (void **)&value_ptr)){
		php_error_docref(NULL, E_ERROR, "serviceConfig['group'] is not set");
		return;
	}
	zend_update_property(dubbo_client_class_entry, self, ZEND_STRL("group"), *value_ptr TSRMLS_DC);

	//dtomap
	dtomap= zend_read_property(dubbo_client_class_entry, self, ZEND_STRL("dtoMapConfig"), 1 TSRMLS_DC);
	if (Z_TYPE_P(dtomap) ==  IS_ARRAY){
		//$service->setDtoMap($this->dtoMapConfig);
		INIT_ZVAL(function_name);
		ZVAL_STRING(&function_name, "setDtoMap", 1);
		params[0] = dtomap;
		if (call_user_function(NULL, &cls_service, &function_name, &retval, 1, params TSRMLS_CC) == FAILURE) {
			zval_dtor(&function_name);
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Error calling constructor");
		}
		zval_dtor(&function_name);
	}
	
	//option
	set_option_2param(cls_service,self, "version", 0);
	set_option_2param(cls_service, self, "group", 0);
	set_option_2param(cls_service, self, "connectTimeout", 0);
	set_option_2param(cls_service, self, "executeTimeout", 0);
	set_option_2param(cls_service, self,  "dubbo", 0);
	set_option_2param(cls_service, self,  "loadbalance", 0);
	set_option_2param(cls_service, self, "owner", 0);
	set_option_2param(cls_service, self, "protocol", 0);
	set_option_2param(cls_service, self, "side", 0);
	//set pid
	pid = getpid();
	ZVAL_LONG(property, pid);
	set_option_2param(cls_service, self, "pid", property);

	//default return is false
	RETVAL_FALSE;

	property = zend_read_property(dubbo_client_class_entry, self, ZEND_STRL("retries"), 1 TSRMLS_DC);
	retries = Z_LVAL_P(property);

	//init function name
	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "call", 1);
	params[0] = arg_method;
	params[1]= arg_params;
	INIT_ZVAL(function_failed);
	ZVAL_STRING(&function_failed, "failed", 1);
	log_call_back = zend_read_property(dubbo_client_class_entry, self, ZEND_STRL("logCallback"), 1 TSRMLS_DC);
	is_callable = zend_is_callable_ex(log_call_back, NULL, 0, NULL, NULL, NULL, &error TSRMLS_CC);

	//for log callback
	fci = obj->entity.fci;
	fci_cache = obj->entity.fci_cache;
	
	for(i=0; i<retries; i++){
		call_user_function(NULL, &cls_service, &function_name, return_value, 2, params TSRMLS_CC);
		if (i_zend_is_true(return_value)){
			break;
		}
		//catch exception
		exception = EG(exception);
		if (exception){
			//add log
			if (is_callable){
				/*
				$msg   = 'call service error:' . $ex->getMessage() . ', service:' . $serviceName
                    		. ',method:' . $method . ', params:' . var_export($params, 1) . ', stack:' . json_encode($stack);
                    		*/
                error_message = zend_read_property(Z_OBJCE_P(exception), exception, ZEND_STRL("message"), 0 TSRMLS_CC);
                sprintf(err_msg, "call service error:%s, service:%s, method:%s", Z_STRVAL_P(error_message)
					, Z_STRVAL_P(arg_service_name), Z_STRVAL_P(arg_method));
                fci.param_count = 1;
				INIT_ZVAL(log_param);
				ZVAL_STRING(&log_param, err_msg, 1);
				log_param_ptr = &log_param;
				*(fci.params) = &log_param_ptr;
				retval_ptr = &retval;
				fci.retval_ptr_ptr = &retval_ptr;
                zend_call_function(&fci, &fci_cache TSRMLS_CC);
				zval_dtor(&log_param);
			}
			
		}

		//failed
		call_user_function(NULL, &cls_service, &function_name, return_value, 0, params TSRMLS_CC);
	}
	if (error){
		efree(error);
	}
	zval_dtor(&function_name);
	zval_dtor(&function_failed);
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
