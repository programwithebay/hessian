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

zend_class_entry *hessian_client_entry;



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
	}
	self = getThis();

	zend_update_property(hessian_client_entry, self, ZEND_STRL("url"), url TSRMLS_DC);

	//@todo:encode
	/*
	$this->options = HessianOptions::resolveOptions($options);
		$this->typemap = new HessianTypeMap($this->options->typeMap);
		$this->factory = new HessianFactory();
	*/
	
}



/*
	HessianClient::addDtoMap
*/
static PHP_METHOD(HessianClient, addDtoMap)
{
	zval *local, *remote;
	zval *self;
	zval *typemap;
	zval *params[2];
	zval function_name;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &local, &remote)) {
		return;
	}
	self = getThis();

	typemap = zend_read_property(hessian_client_entry, self, ZEND_STRL("typemap"), 0 TSRMLS_DC);
	params[0] = local;
	params[1] = remote;
	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "mapType", 1);
	call_user_function(NULL, &typemap, &function_name, NULL, 2, params TSRMLS_CC);

}

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
	HessianClient::__hessianCall
*/
static PHP_METHOD(HessianClient, __hessianCall)
{
	zval *method, *arguments;
	zval *self;
	zval *params[3];
	zval function_name, args;
	zval *factory, *transport, *options, *writer, *z_null;
	zval *typemap, *ctx, *call, *url, *payload, *interceptors;
	zval *before, *stream, *parser, *result, *after;
	char *str_method;
	HashPosition *pos;
	zval **src_entry;
	zend_class_entry *ce_hessian_calling_context_entry, *ce_hessian_call_entry, *ce_hessian_options_entry;
	
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

	
	factory = zend_read_property(hessian_client_entry, self, ZEND_STRL("factory"), 0 TSRMLS_DC);
	options = zend_read_property(hessian_client_entry, self, ZEND_STRL("options"), 0 TSRMLS_DC);
	params[0] = options;
	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "getTransport", 1);
	if (SUCCESS  != call_user_function(NULL, &typemap, &function_name, transport, 1, params TSRMLS_CC)){
		php_error_docref(NULL, E_WARNING, "call factory->getTransport error");
		return;
	}

	ALLOC_ZVAL(z_null);
	INIT_ZVAL(*z_null);
	Z_TYPE_P(z_null) = IS_NULL;
	params[0]= z_null;
	params[1] = options;
	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "getWriter", 1);
	if (SUCCESS  != call_user_function(NULL, &typemap, &function_name, writer, 2, params TSRMLS_CC)){
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

	ce_hessian_calling_context_entry = zend_fetch_class("HessianCallingContext", strlen("HessianCallingContext")-1, 0 TSRMLS_DC);
	if (!ce_hessian_calling_context_entry){
		php_error_docref(NULL, E_ERROR, "HessianCallingContext not define");
		return;
	}
	object_init_ex(ctx, ce_hessian_calling_context_entry);
	zend_update_property(ce_hessian_calling_context_entry, ctx, ZEND_STRL("writer"), writer TSRMLS_DC);
	zend_update_property(ce_hessian_calling_context_entry, ctx, ZEND_STRL("transport"), transport TSRMLS_DC);
	zend_update_property(ce_hessian_calling_context_entry, ctx, ZEND_STRL("options"), options TSRMLS_DC);
	zend_update_property(ce_hessian_calling_context_entry, ctx, ZEND_STRL("typemap"), typemap TSRMLS_DC);

	ce_hessian_call_entry = zend_fetch_class("HessianCall", strlen("HessianCall")-1, 0 TSRMLS_DC);
	if (!ce_hessian_call_entry){
		php_error_docref(NULL, E_ERROR, "HessianCall not define");
		return;
	}
	object_init_ex(call, ce_hessian_call_entry);
	ZVAL_STRING(&function_name, "__construct", 1);
	params[0] = method;
	params[1] = arguments;
	if (SUCCESS  != call_user_function(NULL, &call, &function_name, call, 2, params TSRMLS_CC)){
		php_error_docref(NULL, E_WARNING, "call factory->getWriter error");
		return;
	}
	zend_update_property(ce_hessian_calling_context_entry, ctx, ZEND_STRL("call"), call TSRMLS_DC);
	url = zend_read_property(hessian_client_entry, self, ZEND_STRL("url"), 0 TSRMLS_DC);
	zend_update_property(ce_hessian_calling_context_entry, ctx, ZEND_STRL("url"), url TSRMLS_DC);

	ZVAL_STRING(&function_name, "writeCall", 1);
	params[0] = method;
	params[1] = arguments;
	if (SUCCESS  != call_user_function(NULL, &writer, &function_name, payload, 2, params TSRMLS_CC)){
		php_error_docref(NULL, E_WARNING, "call writer->writeCall error");
		return;
	}
	zend_update_property(ce_hessian_calling_context_entry, ctx, ZEND_STRL("payload"), payload TSRMLS_DC);
	INIT_ZVAL(args);
	array_init_size(&args, 1);
	zend_hash_next_index_insert(Z_ARRVAL(args), &ctx, sizeof(zval *), NULL);

	/*
	foreach($this->options->interceptors as $interceptor){
			$interceptor->beforeRequest($ctx);
		}
	*/
	ce_hessian_options_entry = zend_fetch_class("HessianOptions", strlen("HessianOptions")-1, 0 TSRMLS_DC);
	interceptors = zend_read_property(ce_hessian_options_entry, options, ZEND_STRL("interceptors"), 0 TSRMLS_DC);

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

	ZVAL_STRING(&function_name, "getStream", 1);
	params[0] =  url;
	params[1] = payload;
	params[2] = options;
	if (SUCCESS  != call_user_function(NULL, &transport, &function_name, stream, 3, params TSRMLS_CC)){
		php_error_docref(NULL, E_WARNING, "call $transport->getStream error");
		return;
	}

	factory = zend_read_property(Z_OBJCE_P(options), options, ZEND_STRL("factory"), 0 TSRMLS_DC);
	ZVAL_STRING(&function_name, "getParser", 1);
	params[0] = stream ;
	params[1] = options;
	if (SUCCESS  != call_user_function(NULL, &factory, &function_name, parser, 2, params TSRMLS_CC)){
		php_error_docref(NULL, E_WARNING, "call factory->getParser error");
		return;
	}
	ZVAL_STRING(&function_name, "setTypeMap", 1);
	params[0] =  typemap;
	if (SUCCESS  != call_user_function(NULL, &parser, &function_name, NULL, 1, params TSRMLS_CC)){
		php_error_docref(NULL, E_WARNING, "call parser->setTypeMap error");
		return;
	}
	zend_update_property(Z_OBJCE_P(ctx), ctx, ZEND_STRL("parser"), parser TSRMLS_DC);
	zend_update_property(Z_OBJCE_P(ctx), ctx, ZEND_STRL("stream"), stream TSRMLS_DC);

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
	if (SUCCESS  != call_user_function(NULL, &parser, &function_name, result, 0, params TSRMLS_CC)){
		//@todo seterror
		php_error_docref(NULL, E_WARNING, "call parser->parseTop error");
		return;
	}


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
	
	after = zend_read_property(Z_OBJCE_P(options), options, ZEND_STRL("after"), 0 TSRMLS_DC);
	__handleCallbacks(self, after, &args);

	/*
	if($ctx->error instanceof Exception)
			throw $ctx->error;
	*/
	
	RETURN_ZVAL(result, 0, 0);
}



/*
	HessianClient::call
*/
static PHP_METHOD(HessianClient, call)
{
	zval *method, *arguments;
	zval *self;
	zval *params[2];
	zval function_name;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &method, &arguments)) {
		return;
	}
	self = getThis();
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




//HessianClient functions
const zend_function_entry hessian_client_functions[] = {
	PHP_ME(HessianClient, __construct,		arginfo_hessian_client_construct,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianClient, addDtoMap,			arginfo_hessian_client_add_dtomap,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianClient, __hessianCall,			arginfo_hessian_client_hessian_call,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianClient, call,			arginfo_hessian_client_call,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianClient, __getOptions,			arginfo_hessian_client_get_options,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianClient, __getTypeMap,			arginfo_hessian_client_get_typemap,		ZEND_ACC_PUBLIC)
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
