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
#include "hessian_common.h"



ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_client_construct, 0, 0, 2)
	ZEND_ARG_INFO(0, url) /* string */
	ZEND_ARG_INFO(0, options) /* array */
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_client_add_dtomap, 0, 0, 2)
	ZEND_ARG_INFO(0, local) /* string */
	ZEND_ARG_INFO(0, remote) /* array */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_client_hessian_call, 0, 0, 2)
	ZEND_ARG_INFO(0, method) /* string */
	ZEND_ARG_INFO(0, arguments) /* array */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_client_call, 0, 0, 2)
	ZEND_ARG_INFO(0, method) /* string */
	ZEND_ARG_INFO(0, arguments) /* array */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_client_get_options, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_client_get_typemap, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_client_calltest, 0, 0, 1)
	ZEND_ARG_INFO(0, func) /* string */
ZEND_END_ARG_INFO()


zend_class_entry *hessian_client_entry;


/**
**/
zval* __handleCallbacks(zval* self, zval* callbacks, zval* arguments){
	if (!i_zend_is_true(callbacks))
		return;

	//@todo:encoding
	/*
	if(is_callable($callbacks)){
			return call_user_func_array($callbacks, $arguments);
		}
		if(!is_array($callbacks))
			return;
		foreach($callbacks as $call){
			if(is_callable($call)){
				return call_user_func_array($call, $arguments);
			}
		}
	*/

	
}


/*
	HessianClient::addDtoMap
*/
 void hessian_client_add_dto_map(zval *self, zval *local, zval *remote)
{
	zval function_name, retval;
	zval *params[2];
	zval *typemap;
	
	typemap = zend_read_property(hessian_client_entry, self, ZEND_STRL("typemap"), 1 TSRMLS_DC);
	params[0] = local;
	params[1] = remote;
	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "mapType", 1);
	call_user_function(NULL, &typemap, &function_name, &retval, 2, params TSRMLS_CC);

}


/*
	HessianClient::__hessianCall
*/
void hessian_client__hessianCall(zval *self, zval *method, zval *arguments, zval *return_value)
{
	zval *params[3];
	zval function_name, args;
	zval *factory, *transport, *options, *writer, *z_null;
	zval *typemap, *ctx, *call, *url, *payload, *interceptors;
	zval *before, *stream, *parser, result, *after;
	char *str_method;
	HashPosition *pos;
	zval **src_entry;
	zval retval;
	

	factory = zend_read_property(hessian_client_entry, self, ZEND_STRL("factory"), 0 TSRMLS_DC);
	options = zend_read_property(hessian_client_entry, self, ZEND_STRL("options"), 0 TSRMLS_DC);
	typemap = zend_read_property(hessian_client_entry, self, ZEND_STRL("typemap"), 0 TSRMLS_DC);

	ALLOC_ZVAL(transport);
	hessian_factory_get_transport(factory, options, transport);

	ALLOC_ZVAL(z_null);
	INIT_ZVAL(*z_null);
	Z_TYPE_P(z_null) = IS_NULL;
	params[0]= z_null;
	params[1] = options;
	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "getWriter", 1);
	ALLOC_ZVAL(writer);
	if (SUCCESS  != call_user_function(NULL, &factory, &function_name, writer, 2, params TSRMLS_CC)){
		FREE_ZVAL(z_null);
		php_error_docref(NULL, E_WARNING, "call factory->getWriter error");
		return;
	}
	
	FREE_ZVAL(z_null);
	typemap = zend_read_property(hessian_client_entry, self, ZEND_STRL("typemap"), 0 TSRMLS_DC);
	params[0]= typemap;
	ZVAL_STRING(&function_name, "setTypeMap", 1);
	if (SUCCESS  != call_user_function(NULL, &writer, &function_name, writer, 1, params TSRMLS_CC)){
		php_error_docref(NULL, E_WARNING, "call writer->setTypeMap error");
		return;
	}

	/*
	$ctx = new HessianCallingContext();
		$ctx->writer = $writer;
		$ctx->transport = $transport;
		$ctx->options = $this->options;
		$ctx->typemap = $this->typemap;
		$ctx->call = new HessianCall($method, $arguments);
		$ctx->url = $this->url;
		$ctx->payload = $writer->writeCall($method, $arguments);
		$args = array($ctx);
	*/
	ALLOC_ZVAL(ctx);
	object_init_ex(ctx, hessian_calling_context_entry);
	zend_update_property(NULL, ctx, ZEND_STRL("writer"), writer TSRMLS_DC);
	zend_update_property(NULL, ctx, ZEND_STRL("transport"), transport TSRMLS_DC);

	zend_update_property(NULL, ctx, ZEND_STRL("options"), options TSRMLS_DC);
	zend_update_property(NULL, ctx, ZEND_STRL("typemap"), typemap TSRMLS_DC);

	object_init_ex(call, hessian_call_entry);
	ZVAL_STRING(&function_name, "__construct", 1);
	params[0] = method;
	params[1] = arguments;
	if (SUCCESS  != call_user_function(NULL, &call, &function_name, call, 2, params TSRMLS_CC)){
		php_error_docref(NULL, E_WARNING, "call HessianClient::getWriter error");
		return;
	}
	zend_update_property(NULL, ctx, ZEND_STRL("call"), call TSRMLS_DC);
	url = zend_read_property(hessian_client_entry, self, ZEND_STRL("url"), 0 TSRMLS_DC);
	zend_update_property(NULL, ctx, ZEND_STRL("url"), url TSRMLS_DC);


	//writeCall
	ALLOC_ZVAL(payload);
	hessian2_service_writer_write_call(writer, method, arguments, payload);

	zend_update_property(NULL, ctx, ZEND_STRL("payload"), payload TSRMLS_DC);
	INIT_ZVAL(args);
	array_init_size(&args, 1);
	zend_hash_next_index_insert(Z_ARRVAL(args), &ctx, sizeof(zval *), NULL);

	/*
	foreach($this->options->interceptors as $interceptor){
			$interceptor->beforeRequest($ctx);
		}
	*/
	interceptors = zend_read_property(hessian_options_entry, options, ZEND_STRL("interceptors"), 0 TSRMLS_DC);

	if (interceptors && Z_TYPE_P(interceptors) == IS_ARRAY){
		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(interceptors), pos);
		ZVAL_STRING(&function_name, "beforeRequest", 1);
		while (zend_hash_get_current_data_ex(Z_ARRVAL_P(interceptors), (void **)&src_entry, pos) == SUCCESS) {
			params[0] = ctx;
			if (SUCCESS  != call_user_function(NULL, src_entry, &function_name, NULL, 1, params TSRMLS_CC)){
				php_error_docref(NULL, E_WARNING, "call $interceptor->beforeRequest error");
				return;
			}
			zend_hash_move_forward_ex(Z_ARRVAL_P(interceptors), pos);
		}
	}

	//$this->__handleCallbacks($this->options->before, $args);
	before = zend_read_property(Z_OBJCE_P(options), options, ZEND_STRL("before"), 0 TSRMLS_DC);
	__handleCallbacks(self, before, &args);

	/*
		$stream = $transport->getStream($this->url, $ctx->payload, $this->options);
		$parser = $this->factory->getParser($stream, $this->options);
		$parser->setTypeMap($this->typemap);
		// TODO deal with headers, packets and the rest of aditional stuff
		$ctx->parser = $parser;
		$ctx->stream = $stream;
	*/

	ALLOC_ZVAL(stream);
	hessian_curl_transport_get_stream(transport, url, payload, options, stream);
	if (EG(exception)){
		php_error_docref(NULL, E_WARNING, "call $transport->getStream error");
		return;
	}

	factory = zend_read_property(Z_OBJCE_P(options), options, ZEND_STRL("factory"), 0 TSRMLS_DC);
	ZVAL_STRING(&function_name, "getParser", 1);
	params[0] = stream ;
	params[1] = options;

	
	ALLOC_ZVAL(parser);
	if (SUCCESS  != call_user_function(NULL, &factory, &function_name, parser, 2, params TSRMLS_CC)){
		php_error_docref(NULL, E_WARNING, "call factory->getParser error");
		return;
	}
	ZVAL_STRING(&function_name, "setTypeMap", 1);
	params[0] =  typemap;
	if (SUCCESS  != call_user_function(NULL, &parser, &function_name, &retval, 1, params TSRMLS_CC)){
		php_error_docref(NULL, E_WARNING, "call parser->setTypeMap error");
		return;
	}
	zend_update_property(NULL, ctx, ZEND_STRL("parser"), parser TSRMLS_DC);
	zend_update_property(NULL, ctx, ZEND_STRL("stream"), stream TSRMLS_DC);

	/*
	try{
			$result = $parser->parseTop();
		} catch(Exception $e){
			$ctx->error = $e;
		}
		foreach($this->options->interceptors as $interceptor){
			$interceptor->afterRequest($ctx);
		}
		$this->__handleCallbacks($this->options->after, $args);
		return $result;
	*/

	ZVAL_STRING(&function_name, "parseTop", 1);
	if (SUCCESS  != call_user_function(NULL, &parser, &function_name, &result, 0, params TSRMLS_CC)){
		//@todo seterror
		php_error_docref(NULL, E_WARNING, "call parser->parseTop error");
		return;
	}


	if (interceptors && Z_TYPE_P(interceptors) == IS_ARRAY){
		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(interceptors), pos);
		ZVAL_STRING(&function_name, "afterRequest", 1);
		while (zend_hash_get_current_data_ex(Z_ARRVAL_P(interceptors), (void **)&src_entry, pos) == SUCCESS) {
			params[0] = ctx;
			if (SUCCESS  != call_user_function(NULL, src_entry, &function_name, NULL, 1, params TSRMLS_CC)){
				php_error_docref(NULL, E_WARNING, "call $interceptor->afterRequest error");
				return;
			}
			zend_hash_move_forward_ex(Z_ARRVAL_P(interceptors), pos);
		}
	}
	
	after = zend_read_property(Z_OBJCE_P(options), options, ZEND_STRL("after"), 0 TSRMLS_DC);
	__handleCallbacks(self, after, &args);

	/*
	if($ctx->error instanceof Exception)
			throw $ctx->error;
	*/

	FREE_ZVAL(ctx);
	RETURN_ZVAL(&result, 0, 0);
}




/*
	HessianClient::__construct
*/
void hessian_client_construct(zval *self, zval *url, zval *options)
{
	zval function_name;
	zval *params[1];
	zval *self_options, *type_map, *factory;
	zval *self_type_map;
	zval retval;
	


	zend_update_property(hessian_client_entry, self, ZEND_STRL("url"), url TSRMLS_DC);

	//@todo:encode
	/*
	$this->options = HessianOptions::resolveOptions($options);
		$this->typemap = new HessianTypeMap($this->options->typeMap);
		$this->factory = new HessianFactory();
	*/
	ALLOC_ZVAL(self_options);
	hessian_options_resolve_options(options, self_options);


	
	zend_update_property(NULL, self, ZEND_STRL("options"), self_options TSRMLS_CC);

	type_map = zend_read_property(NULL, self_options, ZEND_STRL("typemap"), 1 TSRMLS_CC);
	ALLOC_ZVAL(self_type_map);
	object_init_ex(self_type_map, hessian_type_map_entry);
	ZVAL_STRING(&function_name, "__construct", 1);
	params[0] = type_map;
	call_user_function(NULL, &self_type_map, &function_name, &retval, 1, params TSRMLS_CC);
	zend_update_property(NULL, self, ZEND_STRL("typemap"), self_type_map TSRMLS_CC);


	ALLOC_ZVAL(factory);
	object_init_ex(factory, hessian_factory_entry);
	hessian_factory_construct(factory);
	zend_update_property(NULL, self, ZEND_STRL("factory"), factory TSRMLS_CC);
}



/*
	HessianClient::__construct
*/
static PHP_METHOD(HessianClient, __construct)
{
	zval *url, *options;
	zval *self;

	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &url, &options)) {
		return;
	}
	if (Z_TYPE_P(url) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "url must be a string");
		return;
	}
	self = getThis();

	hessian_client_construct(self, url,  options);
}




/*
	HessianClient::addDtoMap
*/
static PHP_METHOD(HessianClient, addDtoMap)
{
	zval *local, *remote;
	zval *self;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &local, &remote)) {
		return;
	}
	self = getThis();
	hessian_client_add_dto_map(self, local, remote);
}


/*
	HessianClient::__hessianCall
*/
static PHP_METHOD(HessianClient, __hessianCall)
{
	zval *method, *arguments;
	zval *self;
	char *str_method;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &method, &arguments)) {
		return;
	}
	if (Z_TYPE_P(method) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "method must be a string");
		return;
	}
	self = getThis();
	if (!arguments){
		 ALLOC_ZVAL(arguments);
		 INIT_ZVAL(*arguments);
		 array_init(arguments);
	}
	
	if (Z_STRLEN_P(method) > 1){
		str_method = Z_STRVAL_P(method);
		if ((str_method[0] == '_') && (str_method[1] == '_')){
			php_error_docref(NULL, E_WARNING, "Cannot call methods that start with __");
			return;
		}
	}

	hessian_client__hessianCall(self, method, arguments, return_value);
}



/*
	HessianClient::call
*/
static PHP_METHOD(HessianClient, __call)
{
	zval *method, *arguments;
	zval *self;
	zval *params[2];
	zval function_name;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &method, &arguments)) {
		return;
	}
	self = getThis();

	ZVAL_STRING(&function_name, "__hessianCall", 1);
	params[0] = method;
	params[1] = arguments;
	call_user_function(NULL, &self, &function_name, return_value, 2, params TSRMLS_CC);

	return;
}


/*
	HessianClient::__getOptions
*/
static PHP_METHOD(HessianClient, __getOptions)
{
	zval *self;
	self = getThis();

	return_value = zend_read_property(hessian_client_entry, self, ZEND_STRL("options"), 0 TSRMLS_DC);
}

/*
	HessianClient::__getTypeMap
*/
static PHP_METHOD(HessianClient, __getTypeMap)
{
	zval *self;
	self = getThis();

	return_value = zend_read_property(hessian_client_entry, self, ZEND_STRL("typemap"), 0 TSRMLS_DC);
}

//calltest
//how to write a call method
static PHP_METHOD(HessianClient, calltest)
{
	zval *fun;
	zval *params, *retval_ptr = NULL;
	zend_fcall_info *fci;
	zend_fcall_info_cache *fci_cache;
	char *is_callable_error;
	zval *format;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &fun)) {
		return;
	}

	fci = emalloc(sizeof(zend_fcall_info));
	fci_cache = emalloc(sizeof(zend_fcall_info_cache));
	fci->function_table = EG(function_table);
	if (zend_fcall_info_init(fun, 0, fci, fci_cache, NULL, &is_callable_error TSRMLS_CC) == SUCCESS){
	}else{
		return;
	}

	ALLOC_ZVAL(params);
	array_init_size(params, 1);
	ALLOC_ZVAL(format);
	INIT_ZVAL(*format);
	ZVAL_STRING(format, "Y-m-d H:i:s", 1);
	zend_hash_next_index_insert(Z_ARRVAL_P(params), &format, sizeof(zval**), NULL);

	zend_fcall_info_args(fci, params TSRMLS_CC);
	fci->retval_ptr_ptr = &retval_ptr;

	if (zend_call_function(fci, fci_cache TSRMLS_CC) == SUCCESS && fci->retval_ptr_ptr && *(fci->retval_ptr_ptr)) {
		COPY_PZVAL_TO_ZVAL(*return_value, *(fci->retval_ptr_ptr));
	}

	zend_fcall_info_args_clear(fci, 1);
	FREE_ZVAL(params);
	FREE_ZVAL(format);
}



//HessianClient functions
const zend_function_entry hessian_client_functions[] = {
	PHP_ME(HessianClient, __construct,		arginfo_hessian_client_construct,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianClient, addDtoMap,			arginfo_hessian_client_add_dtomap,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianClient, __hessianCall,			arginfo_hessian_client_hessian_call,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianClient, __call,			arginfo_hessian_client_call,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianClient, __getOptions,			arginfo_hessian_client_get_options,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianClient, __getTypeMap,			arginfo_hessian_client_get_typemap,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianClient, calltest,			arginfo_hessian_client_calltest,		ZEND_ACC_PUBLIC)
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
