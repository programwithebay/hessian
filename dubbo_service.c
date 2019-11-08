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


zend_object_handlers dubbo_service_object_handlers;
typedef struct _dubbo_service_object dubbo_service_object;
struct _dubbo_service_object {
	//for constructor?
	zend_object std;
	char *name;
	int cur_provider_index;
	int init_provider;
	zval *storage;
	HashTable *options;
	HashTable *dto_map;
	HashTable *providers;
	HashTable *consumers;
};


void dubbo_service_free_storage(void *object TSRMLS_DC)
{
    dubbo_service_object *obj = (dubbo_service_object *)object;

	//free str
	if (obj->name != NULL){
		efree(obj->name);
	}
	

	//free hash table
	zend_hash_destroy(obj->options);
	zend_hash_destroy(obj->dto_map);
	zend_hash_destroy(obj->providers);
	zend_hash_destroy(obj->consumers);

	if (obj->storage != NULL){
		zval_dtor(obj->storage);
		FREE_ZVAL(obj->storage);
	}


	//free string memory
    efree(obj);
}



zend_object_value dubbo_service_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    dubbo_service_object *obj = (dubbo_service_object *)emalloc(sizeof(dubbo_service_object));
    memset(obj, 0, sizeof(dubbo_service_object));

	//for normal class
	obj->std.ce = type;
	zend_object_std_init(&obj->std, type TSRMLS_CC);
	object_properties_init(&obj->std, type);


	//hash table
	/*
	obj->options = (HashTable *) emalloc(sizeof(HashTable));
	zend_hash_init(obj->options, 0, NULL, ZVAL_PTR_DTOR, 0);

	obj->dto_map = (HashTable *) emalloc(sizeof(HashTable));
	zend_hash_init(obj->dto_map, 0, NULL, ZVAL_PTR_DTOR, 0);

	obj->providers = (HashTable *) emalloc(sizeof(HashTable));
	zend_hash_init(obj->providers, 0, NULL, ZVAL_PTR_DTOR, 0);

	obj->consumers = (HashTable *) emalloc(sizeof(HashTable));
	zend_hash_init(obj->consumers, 0, NULL, ZVAL_PTR_DTOR, 0);
	*/
	
	
    retval.handle = zend_objects_store_put(obj, NULL, dubbo_service_free_storage, NULL TSRMLS_CC);
    retval.handlers = &dubbo_client_object_handlers;

    return retval;
}


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


//select provider
zval* hessian_dubbo_service_select_provider(zval *self){
	/*
		if (false === $this->initProvider) {
            $this->delInvalidProvider();
            $this->initProvider = true;
        }
        */


	zval *init;
	HashTable *providers;
	zval function_name;
	zval *params[2];
	HashPosition pos;
	dubbo_service_object *c_obj;
	
	c_obj = (dubbo_service_object *)zend_object_store_get_object(self TSRMLS_CC);

	if (c_obj->init_provider < 1){
		/*
			 $providers = [];
       		 foreach ($this->providers as $provider) {
            		$provider = urldecode($provider);
           		 $pos      = strpos($provider, ':');
            		if (false === $pos) {
                		continue;
            		}
           		 $prefix = strtolower(substr($provider, 0, $pos));
            		if ('hessian' === $prefix) {
              	  $providers[] = $provider;
            		}
        	}

        	$this->providers = $providers;
        	*/
        HashTable *self_providers;
		zval **src_entry;
		char *buf;
		char flag=0;
		int i;

		providers = (HashTable *) emalloc(sizeof(HashTable));
		zend_hash_init(providers, 0, NULL, ZVAL_PTR_DTOR, 0);
	
		self_providers = c_obj->providers;

		zend_hash_internal_pointer_reset_ex(self_providers, &pos);

		//todo:this function cause a memory blcok gc many times
		ZVAL_STRING(&function_name, "urldecode", 1);

		zval *provider;
		zval decode_res;
		while (zend_hash_get_current_data_ex(self_providers, (void **)&src_entry, &pos) == SUCCESS) {
			flag = 0;
			params[0]= *src_entry;
			
			call_user_function(EG(function_table), NULL, &function_name, &decode_res, 1, params TSRMLS_CC);
			//provider = &decode_res;
			buf = Z_STRVAL(decode_res);
			for(i=0; i<Z_STRLEN(decode_res); i++){
				if (buf[i] == ':'){
					flag = 1;
					break;
				}
			}
			if (1 == flag){
				if (strncasecmp(buf, "hessian", 7) == 0){
					//provider must copy
					zval *new_provider;

					ALLOC_ZVAL(new_provider);
					INIT_PZVAL(new_provider);
					ZVAL_STRINGL(new_provider, Z_STRVAL(decode_res), Z_STRLEN(decode_res), 1);
					zend_hash_next_index_insert(providers, &new_provider, sizeof(zval *), NULL);
				}
			}
			zend_hash_move_forward_ex(self_providers, &pos);
		}

		zval_dtor(&function_name);

		//zval_addref_p(providers);
		c_obj->providers = providers;


		//$this->initProvider = true;
		c_obj->init_provider = 1;
	}else{
		providers = c_obj->providers;
	}


	/*
		  if (empty($this->providers)) {
            throw new Exception('providers is empty');
        }

       */

	if (zend_hash_num_elements(providers) < 1){
		zend_error(E_WARNING, "providers is empty");
		return;
	}

	/*
	        $index = array_rand($this->providers, 1);
	        $this->curProviderIndex = $index;
	        return $this->providers[$index];
        */

	zval **res;
	zval param1;
	zval z_providers;
	zval z_index;

	Z_TYPE(z_providers)= IS_ARRAY;
	Z_ARRVAL(z_providers) = providers;
	ZVAL_STRING(&function_name, "array_rand", 1);
	params[0] = &z_providers;
	INIT_ZVAL(param1);
	ZVAL_LONG(&param1, 1);
	params[1] = &param1;

	call_user_function(EG(function_table), NULL, &function_name, &z_index, 2, params TSRMLS_CC);
	zval_dtor(&function_name);
	
	zend_hash_internal_pointer_reset_ex(providers, &pos);
	zend_hash_index_find(providers, Z_LVAL(z_index), (void **)&res);

	return *res;
	//RETURN_ZVAL(res, 1, NULL);
}


//format provider
zval* hessian_dubbo_service_format_provider(zval *self, zval *provider){
	/*
		 $provider = str_replace('hessian:', 'http:', $provider);
        //处理选项
        $urlInfo = parse_url($provider);
        $arr     = explode('&', $urlInfo['query']);
        $params  = array();
        foreach ($arr as $item) {
            $arrItem             = explode('=', $item);
            $params[$arrItem[0]] = $arrItem[1];
        }
        */

	zval function_name;
	zval param1, param2;
	zval *arg_params[3];
	zval url_info, **query, arr_query_param, *params;
	zval arr_options;
	HashTable *options;
	zval **option_version, **params_version;
	char *key;
	char *str_version, *str_group;
	zval *res;

	//php_error_docref(NULL, E_NOTICE, "provider:%s", Z_STRVAL_P(provider));
	dubbo_service_object *c_obj;
	c_obj = (dubbo_service_object *)zend_object_store_get_object(self TSRMLS_CC);


	ALLOC_ZVAL(res);
	ZVAL_STRING(&function_name, "str_replace", 1);
	ZVAL_STRING(&param1, "hessian:", 1);
	ZVAL_STRING(&param2, "http:", 1);
	INIT_ZVAL(param1);
	INIT_ZVAL(param2);
	arg_params[0] = &param1;
	arg_params[1] = &param2;
	arg_params[2] = provider;

	call_user_function(EG(function_table), NULL, &function_name, res, 3, arg_params TSRMLS_CC);
	zval_dtor(&function_name);
	zval_dtor(&param1);
	zval_dtor(&param2);

		

	ZVAL_STRING(&function_name, "parse_url", 1);
	//Z_ADDREF_P(provider);
	arg_params[0] = provider;
	call_user_function(EG(function_table), NULL, &function_name, &url_info, 1, arg_params TSRMLS_CC);
	zval_dtor(&function_name);

	
	if (zend_hash_find(Z_ARRVAL(url_info), "query", strlen("query")+1, (void **)&query) != SUCCESS){
		php_error_docref(NULL, E_WARNING, "hessian url no query param");
		return;
	}
	Z_TYPE_PP(query) = IS_STRING;


	//here has problem, cause memory
	ZVAL_STRING(&function_name, "explode", 1);
	ZVAL_STRING(&param1, "&", 1);
	//Z_ADDREF_P(*query);
	arg_params[0]  = &param1;
	arg_params[1] = *query;
	call_user_function(EG(function_table), NULL, &function_name, &arr_query_param, 2, arg_params TSRMLS_CC);
	zval_dtor(&function_name);
	zval_dtor(&param1);

	
	HashPosition pos;
	zval **src_entry;
	char *p;
	
	
	
	//ALLOC_ZVAL(params);
	//array_init_size(params, zend_hash_num_elements(Z_ARRVAL(arr_query_param)));
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL(arr_query_param), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL(arr_query_param), (void **)&src_entry, &pos) == SUCCESS) {
		/*
			  $arrItem             = explode('=', $item);
            		$params[$arrItem[0]] = $arrItem[1];
            */
            
        zval arr_item;
		zval *new_value;
		zval **key, **value;
		
		p = Z_STRVAL_PP(src_entry);
		if (strncmp(p, "version", 7) == 0){
			str_version = (p+7+1);
		}else if (strncmp(p, "group", 5) == 0){
			str_group = (p+5+1);
		}


		//free array
		//zval_copy_ctor(zvalue)
		zend_hash_move_forward_ex(Z_ARRVAL(arr_query_param), &pos);
	}
	//zval_dtor(&function_name);


	/*
		//是否匹配
        if (($this->options['version'] !== $params['version'])
        ) {
            throw new Exception('version dont match service requirement');
        }
        if (isset($this->options['group'])) {
            if ($this->options['version'] !== $params['version']) {
                throw new Exception('group dont match service requirement');
            }
        }
        */

	options = c_obj->options;
	//zend_read_property(NULL, self, ZEND_STRL("options"), 1 TSRMLS_CC);
	if (SUCCESS != zend_hash_find(options, "version", strlen("version")+1, (void **)&option_version TSRMLS_CC)){
		zend_error(E_ERROR, "hessian_dubbo_service_format_provider provider version empty");
		return;
	}


	if ( (0 != strncmp(Z_STRVAL_PP(option_version), str_version, Z_STRLEN_PP(option_version)))
		){
		zend_error(E_ERROR, "version dont match service requirement");
		return;
	}


	/*
	//dont checout group
	zval **option_group, **params_group;
	if (SUCCESS == (zend_hash_find(Z_ARRVAL_P(options), "group", strlen("group")+1, (void **)&option_group TSRMLS_DC))){
		//match group
		if (SUCCESS != zend_hash_find(Z_ARRVAL_P(params), "group", strlen("group")+1, (void **)&params_group TSRMLS_DC)){
			zend_error(E_ERROR, "hessian_dubbo_service_format_provider params group:%s empty", Z_STRVAL_PP(query));
			return;
		}

		if (0 != strncmp(Z_STRVAL_PP(option_group), Z_STRVAL_PP(params_group), Z_STRLEN_PP(option_group))){
			zend_class_entry **ce_exception;
			zend_hash_find(CG(class_table), "exception", sizeof("exception"), (void **) &ce_exception);
			zend_throw_exception(*ce_exception, "group dont match service requirement", 0);
			return;
		}
	}

    */


	return res;
}


/*
	DubboService construct
*/
void dubbo_service_construct(zval *self, zval *name, zval *providers, zval *consumers)
{
	if (Z_TYPE_P(name) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "name is not a string");
		return;
	}
	if (Z_TYPE_P(providers) != IS_ARRAY){
		php_error_docref(NULL, E_WARNING, "providers is not a array");
		return;
	}
	if (Z_TYPE_P(consumers) != IS_ARRAY){
		php_error_docref(NULL, E_WARNING, "consumers is not a array");
		return;
	}

	//for $providers, $options, $dtoMap
	zval *options, *dto_map;
	dubbo_service_object *c_obj;
	
	c_obj = (dubbo_service_object *)zend_object_store_get_object(self TSRMLS_CC);
	SET_CLASS_STR_MEMBER(c_obj, name, name);
	c_obj->providers = Z_ARRVAL_P(providers);
	c_obj->consumers = Z_ARRVAL_P(consumers);
}



/*
	get service by name
*/
void get_service_by_name(zval *name, zval *storage, zval* retval){
	zval config, **providers, **consumers;

	dubbo_file_storage_get(storage, name, &config);
	if (Z_TYPE(config) != IS_ARRAY){
		php_error_docref(NULL, E_WARNING, "config file is not json format");
		return;
	}

	object_init_ex(retval, dubbo_service_class_entry);
	zend_hash_find(Z_ARRVAL(config), ZEND_STRS("providers"), (void **)&providers);
	zend_hash_find(Z_ARRVAL(config), ZEND_STRS("consumers"), (void **)&consumers);
	
	dubbo_service_construct(retval ,name, *providers, *consumers);
}



/*
DubboService::call
*/
void dubbo_service_call(zval *self, zval *method, zval *arg_params, zval *return_value)
{
	/*
		 $provider = $this->selectProvider();
        	$provider = $this->formatProvider($provider);
        */

	zval  *provider;
	zval *transport_options;
	HashTable *self_options;
	zval function_name;
	zval *params[2];
	dubbo_service_object *c_obj;

	
	c_obj = (dubbo_service_object *)zend_object_store_get_object(self TSRMLS_CC);

	provider = hessian_dubbo_service_select_provider(self);
	provider = hessian_dubbo_service_format_provider(self, provider);
	//ALLOC_ZVAL(provider);
	//Z_STRVAL_P(provider) = "http://www.baidu.com";
	//Z_STRLEN_P(provider) = strlen( "http://www.baidu.com");

	/*
		 $options                     = array();
        $options['transportOptions'] = [];
        if (isset($this->options['connectTimeout'])) {
            $options['transportOptions'][CURLOPT_CONNECTTIMEOUT_MS] = $this->options['connectTimeout'] * 1000;
        }
        if (isset($this->options['executeTimeout'])) {
            $options['transportOptions'][CURLOPT_TIMEOUT_MS] = $this->options['executeTimeout'] * 1000;
        }
       */

	ALLOC_ZVAL(transport_options);
	INIT_PZVAL(transport_options);
	array_init_size(transport_options, 2);


	//timeout set
	self_options = c_obj->options;

	//timeout
	zval **connect_timeout, **execute_timeout;
	if (SUCCESS == (zend_hash_find(self_options, ZEND_STRS("connectTimeout")
		, (void **)&connect_timeout))){
		//CURLOPT_CONNECTTIMEOUT_MS
		Z_LVAL_PP(connect_timeout) *= 1000;
		zend_hash_index_update(Z_ARRVAL_P(transport_options), 156, connect_timeout, sizeof(zval *), NULL);
	}

	if (SUCCESS == (zend_hash_find(self_options, ZEND_STRS("executeTimeout")
		, (void **)&execute_timeout))){
		//CURLOPT_CONNECTTIMEOUT_MS
		Z_LVAL_PP(execute_timeout) *= 1000;
		zend_hash_index_update(Z_ARRVAL_P(transport_options), 155, execute_timeout, sizeof(zval *), NULL);
	}

	zend_hash_add(self_options, "transportOptions", sizeof("transportOptions"), &transport_options
		, sizeof(zval *), NULL);


	/*
		 $client = new HessianClient($provider, $options);

	        //添加DtoMap
	        if (!empty($this->dtoMap)) {
	            foreach ($this->dtoMap as $k => $v) {
	                $client->addDtoMap($k, $v);
	            }
	        }

	      
        */

	zval *client, retval;
	HashTable *dto_map;
	zval z_options;

	ALLOC_ZVAL(client);

	Z_TYPE(z_options) = IS_ARRAY;
	Z_ARRVAL(z_options) = self_options;
	object_init_ex(client, hessian_client_entry);
	hessian_client_construct(client, provider, &z_options);


	dto_map = c_obj->dto_map;
	if (zend_hash_num_elements(dto_map) > 0){
		HashPosition pos;
		zval **src_entry;
		char *string_key;
		uint string_key_len;
		ulong num_key;
		zval *param1, retval;

		//ZVAL_STRING(&function_name, "addDtoMap", 1);
		zend_hash_internal_pointer_reset_ex(dto_map, &pos);
		while (zend_hash_get_current_data_ex(dto_map, (void **)&src_entry, &pos) == SUCCESS) {
			zval *local;
			zend_hash_get_current_key_ex(dto_map, &string_key, &string_key_len, &num_key, 0, &pos);

			ALLOC_ZVAL(local);
			ZVAL_STRINGL(local, string_key, string_key_len, 1);
			hessian_client_add_dto_map(client, local, *src_entry);

			/*
			ZVAL_STRING(&param1, string_key, 1);
			params[0] = &param1;
			params[1] = *src_entry;
			call_user_function(NULL, &p_client, &function_name, &retval, 2, params  TSRMLS_DC);
			*/
			
			zend_hash_move_forward_ex(dto_map, &pos);
		}
	}


	/*
		  $res = call_user_func_array(array($client, $method), $params);

	        //返回参数中的data有可能是stdClass的，也有array的
	        return $res;
	*/

	zval res;
	hessian_client__hessianCall(client, method, arg_params, &res);

	RETURN_ZVAL(&res, 1, NULL);
}




/*
DubboService construct
*/
static PHP_METHOD(DubboService, __construct)
{
	zval *name, *providers, *consumers;
	zval *self;
	
	self = getThis();
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &name, &providers, &consumers)) {
		php_error_docref(NULL, E_WARNING, "parse DubboService::__construct param error");
		return;
	}

	
	dubbo_service_construct(self, name, providers, consumers);
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
	dubbo_service_object *c_obj;
	c_obj = (dubbo_service_object *)zend_object_store_get_object(self TSRMLS_CC);
	c_obj->storage = storage;
}


/*
DubboService construct
*/
void dubbo_service_set_option(zval *self, zval *name, zval *value)
{
	HashTable *options;
	zval *pvalue;
	dubbo_service_object *c_obj;
	
	c_obj = (dubbo_service_object *)zend_object_store_get_object(self TSRMLS_CC);

	if (c_obj->options == NULL){
		c_obj->options = (HashTable *) emalloc(sizeof(HashTable));
		zend_hash_init(c_obj->options, 0, NULL, ZVAL_PTR_DTOR, 0);
	}

	zend_hash_update(c_obj->options, Z_STRVAL_P(name), Z_STRLEN_P(name)+1, (void **)&value, sizeof(zval *), NULL);
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
	dubbo_service_set_option(self, name, value);
}

//set dtomap
void dubbo_service_set_dtomap(zval *self, zval *dtomap){
	if (Z_TYPE_P(dtomap) == IS_ARRAY){
		dubbo_service_object *c_obj;
		c_obj = (dubbo_service_object *)zend_object_store_get_object(self TSRMLS_CC);
		c_obj->dto_map= Z_ARRVAL_P(dtomap);
	}
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
		return;
	}
	self = getThis();
	dubbo_service_set_dtomap(self, dtomap);
}


/*
DubboService construct
*/
static PHP_METHOD(DubboService, save)
{
	zval *value;
	HashTable *providers, *consumers;
	zval *storage;
	char *name;
	zval *self;
	zval *params[2];
	zval function_name, retval;
	dubbo_service_object *c_obj;
	zval z_providers, z_consumers, z_name;

	
	self = getThis();
	c_obj = (dubbo_service_object *)zend_object_store_get_object(self TSRMLS_CC);
	
	providers = c_obj->providers;
	consumers = c_obj->consumers;
	storage = c_obj->storage;
	
	ALLOC_ZVAL(value);
	array_init_size(value, 2);

	//todo strlen or strlen+1
	Z_TYPE(z_providers) =  IS_ARRAY;
	Z_ARRVAL(z_providers) = providers;
	Z_TYPE(z_consumers) =  IS_ARRAY;
	Z_ARRVAL(z_consumers) = consumers;
	zend_hash_update(Z_ARRVAL_P(value), "providers", strlen("providers"), &z_providers, sizeof(zval *), NULL);
    zend_hash_update(Z_ARRVAL_P(value), "consumers", strlen("consumers"), &z_consumers, sizeof(zval *), NULL);

	//set storage
	name = c_obj->name;
	ZVAL_STRING(&z_name, name, 0);
	params[0] = &z_name;
	params[1] = value;
	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "set", 1);
	hessian_call_class_function_helper(storage, &function_name, 2, params, &retval);
	zval_dtor(&function_name);
}

/*
DubboService construct
*/
static PHP_METHOD(DubboService, call)
{
	/*
		 $provider = $this->selectProvider();
        	$provider = $this->formatProvider($provider);
        */

	zval *self;
	zval *method, *arg_params;


	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &method, &arg_params)) {
		php_error_docref(NULL, E_WARNING, "DubboService call parse param error");
		return;
	}

	self = getThis();
	dubbo_service_call(self, method, arg_params, return_value);
}


/*
DubboService construct
*/
static PHP_METHOD(DubboService, failed)
{
	zval z_index;
	long index;
	zval *self;
	HashTable *ht_providers;
	ulong hval;
	ulong h;
	dubbo_service_object *c_obj;

	self = getThis();
	c_obj = (dubbo_service_object *)zend_object_store_get_object(self TSRMLS_CC);
	

	index = c_obj->cur_provider_index;
	//unset($this->providers[$index]);
	ht_providers = c_obj->providers;
	ZVAL_LONG(&z_index, index);
	convert_to_string(&z_index);
	h = zend_inline_hash_func(Z_STRVAL(z_index), Z_STRLEN(z_index));
	hval = h & ht_providers->nTableMask;
	zend_hash_index_del(ht_providers, hval);
}


const zend_function_entry dubbo_service_functions[] = {
	PHP_ME(DubboService, __construct,		arginfo_dubbo_service_construct,		ZEND_ACC_PUBLIC)
	PHP_ME(DubboService, setStorage,		arginfo_dubbo_service_setstorage,		ZEND_ACC_PUBLIC)
	PHP_ME(DubboService, setOption,			arginfo_dubbo_service_setoption,		ZEND_ACC_PUBLIC)
	PHP_ME(DubboService, setDtoMap,			arginfo_dubbo_service_setdtomap,		ZEND_ACC_PUBLIC)
	PHP_ME(DubboService, save,				arginfo_dubbo_service_save,				ZEND_ACC_PUBLIC)
	PHP_ME(DubboService, call,				arginfo_dubbo_service_call,				ZEND_ACC_PUBLIC)
	PHP_ME(DubboService, failed,			arginfo_dubbo_service_failed,				ZEND_ACC_PUBLIC)
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
