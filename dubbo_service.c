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


//select provider
zval* hessian_dubbo_service_select_provider(zval* self){
	/*
		if (false === $this->initProvider) {
            $this->delInvalidProvider();
            $this->initProvider = true;
        }
        */


	zval *init;
	zval *providers;
	zval function_name;
	zval *params[2];
	HashPosition pos;

	init = zend_read_property(NULL, self, ZEND_STRL("initProvider"), 1 TSRMLS_CC);
	if (Z_TYPE_P(init) == IS_BOOL && Z_BVAL_P(init) < 1){
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
        zval *self_providers, **src_entry;
		char *buf;
		char flag=0;
		int i;

		ALLOC_ZVAL(providers);
		array_init_size(providers, 2);
		self_providers = zend_read_property(NULL, self, ZEND_STRL("providers"), 1 TSRMLS_CC);

		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(self_providers), &pos);

		//todo:this function cause a memory blcok gc many times
		ZVAL_STRING(&function_name, "urldecode", 1);

		zval *provider;
		zval decode_res;
		while (zend_hash_get_current_data_ex(Z_ARRVAL_P(self_providers), (void **)&src_entry, &pos) == SUCCESS) {
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
					ZVAL_STRINGL(new_provider, Z_STRVAL(decode_res), Z_STRLEN(decode_res), 1);
					zend_hash_next_index_insert(Z_ARRVAL_P(providers), &new_provider, sizeof(zval *), NULL);
				}
			}
			zend_hash_move_forward_ex(Z_ARRVAL_P(self_providers), &pos);
		}

		zval_dtor(&function_name);

		//zval_addref_p(providers);
		zend_update_property(NULL, self, ZEND_STRL("providers"), providers TSRMLS_CC);

		//$this->initProvider = true;
		ZVAL_BOOL(init , 1);
	}else{
		providers = zend_read_property(NULL, self, ZEND_STRL("providers"), 1 TSRMLS_CC);
	}


	/*
		  if (empty($this->providers)) {
            throw new Exception('providers is empty');
        }

       */

	if (zend_hash_num_elements(Z_ARRVAL_P(providers)) < 1){
		zend_error(E_WARNING, "providers is empty");
		return;
	}

	/*
	        $index = array_rand($this->providers, 1);
	        $this->curProviderIndex = $index;
	        return $this->providers[$index];
        */

	zval *index, **res;
	zval param1;

	ZVAL_STRING(&function_name, "array_rand", 1);
	params[0] = providers;
	INIT_ZVAL(param1);
	ZVAL_LONG(&param1, 1);
	params[1] = &param1;

	index = zend_read_property(NULL, self, ZEND_STRS("curProviderIndex"), 1 TSRMLS_CC);
	call_user_function(EG(function_table), NULL, &function_name, index, 2, params TSRMLS_CC);
	zval_dtor(&function_name);
	
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(providers), &pos);
	zend_hash_index_find(Z_ARRVAL_P(providers), Z_LVAL_P(index), (void **)&res);

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
	zval *options;
	zval **option_version, **params_version;
	char *key;
	char *str_version, *str_group;
	zval *res;

	//php_error_docref(NULL, E_NOTICE, "provider:%s", Z_STRVAL_P(provider));


	ALLOC_ZVAL(res);
	ZVAL_STRING(&function_name, "str_replace", 1);
	ZVAL_STRING(&param1, "hessian:", 1);
	ZVAL_STRING(&param2, "http:", 1);
	arg_params[0] = &param1;
	arg_params[1] = &param2;
	arg_params[2] = provider;

	call_user_function(EG(function_table), NULL, &function_name, res, 3, arg_params TSRMLS_CC);
	zval_dtor(&function_name);
	//zval_dtor(&param1);
	//zval_dtor(&param2);

		

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
	//zval_dtor(&param1);

	
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

	options = zend_read_property(NULL, self, ZEND_STRL("options"), 1 TSRMLS_CC);
	if (SUCCESS != zend_hash_find(Z_ARRVAL_P(options), "version", strlen("version")+1, (void **)&option_version TSRMLS_CC)){
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

	options = zend_read_property(NULL, self, ZEND_STRL("options"), 1 TSRMLS_CC);
	if (Z_TYPE_P(options) != IS_ARRAY){
		MAKE_STD_ZVAL(options);
		array_init(options);
		zend_update_property(NULL, self, ZEND_STRL("options"), options TSRMLS_CC);
	}
	dto_map = zend_read_property(NULL, self, ZEND_STRL("dtoMap"), 1 TSRMLS_CC);
	if (Z_TYPE_P(dto_map) != IS_ARRAY){
		MAKE_STD_ZVAL(dto_map);
		array_init(dto_map);
		zend_update_property(NULL, self, ZEND_STRL("dtoMap"), dto_map TSRMLS_CC);
	}
	
	zend_update_property(NULL, self, ZEND_STRL("name"), name TSRMLS_CC);
	zend_update_property(NULL, self, ZEND_STRL("providers"), providers TSRMLS_CC);
	zend_update_property(NULL, self, ZEND_STRL("consumers"), consumers TSRMLS_CC);
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
	zval *self_options;
	zval function_name;
	zval *params[2];


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
	array_init_size(transport_options, 2);


	//timeout set
	self_options = zend_read_property(NULL, self, ZEND_STRL("options"), 1 TSRMLS_CC);
	if (Z_TYPE_P(self_options) != IS_ARRAY){
		array_init_size(self_options, 2);
		zend_update_property(NULL, self, ZEND_STRL("options"), self_options TSRMLS_CC);
	}

	//timeout
	zval **connect_timeout, **execute_timeout;
	if (SUCCESS == (zend_hash_find(Z_ARRVAL_P(self_options), ZEND_STRS("connectTimeout")
		, (void **)&connect_timeout))){
		//CURLOPT_CONNECTTIMEOUT_MS
		Z_LVAL_PP(connect_timeout) *= 1000;
		zend_hash_index_update(Z_ARRVAL_P(transport_options), 156, connect_timeout, sizeof(zval *), NULL);
	}

	if (SUCCESS == (zend_hash_find(Z_ARRVAL_P(self_options), ZEND_STRS("executeTimeout")
		, (void **)&execute_timeout))){
		//CURLOPT_CONNECTTIMEOUT_MS
		Z_LVAL_PP(execute_timeout) *= 1000;
		zend_hash_index_update(Z_ARRVAL_P(transport_options), 155, execute_timeout, sizeof(zval *), NULL);
	}

	zend_hash_add(Z_ARRVAL_P(self_options), "transportOptions", sizeof("transportOptions"), &transport_options
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
	zval *dto_map;

	ALLOC_ZVAL(client);

	object_init_ex(client, hessian_client_entry);
	hessian_client_construct(client, provider, self_options);


	dto_map = zend_read_property(NULL, self, ZEND_STRL("dtoMap"), 1 TSRMLS_DC);
	if (Z_TYPE_P(dto_map) == IS_ARRAY){
		HashPosition pos;
		zval **src_entry;
		char *string_key;
		uint string_key_len;
		ulong num_key;
		zval *param1, retval;

		//ZVAL_STRING(&function_name, "addDtoMap", 1);
		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(dto_map), &pos);
		while (zend_hash_get_current_data_ex(Z_ARRVAL_P(dto_map), (void **)&src_entry, &pos) == SUCCESS) {
			zval *local;
			zend_hash_get_current_key_ex(Z_ARRVAL_P(dto_map), &string_key, &string_key_len, &num_key, 0, &pos);

			ALLOC_ZVAL(local);
			ZVAL_STRINGL(local, string_key, string_key_len, 1);
			hessian_client_add_dto_map(client, local, *src_entry);

			/*
			ZVAL_STRING(&param1, string_key, 1);
			params[0] = &param1;
			params[1] = *src_entry;
			call_user_function(NULL, &p_client, &function_name, &retval, 2, params  TSRMLS_DC);
			*/
			
			zend_hash_move_forward_ex(Z_ARRVAL_P(dto_map), &pos);
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
	zend_update_property(dubbo_service_class_entry, self, ZEND_STRL("storage"), storage TSRMLS_DC);
}


/*
DubboService construct
*/
void dubbo_service_set_option(zval *self, zval *name, zval *value)
{
	zval *options;
	zval *pvalue;
	
	options = zend_read_property(NULL, self, ZEND_STRL("options"), 1 TSRMLS_CC);
	if (Z_TYPE_P(options) != IS_ARRAY){
		//init array
		INIT_PZVAL(options);
		array_init_size(options, 8);
		zend_update_property(NULL, self, ZEND_STRL("options"), options TSRMLS_CC);
	}
	zend_hash_update(Z_ARRVAL_P(options), Z_STRVAL_P(name), Z_STRLEN_P(name)+1, (void **)&value, sizeof(zval *), NULL);
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
		zend_update_property(NULL, self, ZEND_STRL("dtoMap"), dtomap TSRMLS_CC);
		//zval_ptr_dtor(&dtomap);
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
	zval *providers, *consumers, *storage, *name;
	zval *self;
	zval *params[2];
	zval function_name, retval;

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

	zval *self, *provider;
	zval *method, *arg_params;
	zval options, *transport_options;
	zval *self_options;
	zval function_name;
	zval *params[2];


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
