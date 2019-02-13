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
#include "SAPI.h"
#include "php_ini.h"
#include "hessian_common.h"
#include "php_hessian_int.h"


//params
ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_service_construct, 0, 0, 1)
	ZEND_ARG_INFO(0, serviceObject) /* string */
	ZEND_ARG_INFO(0, options) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_service_register_object, 0, 0, 1)
	ZEND_ARG_INFO(0, service) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_service_write_fault, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, "ex", "Exception", 0)
	ZEND_ARG_INFO(0, writer) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_service_handle, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_service_display_info, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_service_call_method, 0, 0, 2)
	ZEND_ARG_INFO(0, method) /* string */
	ZEND_ARG_INFO(0, params) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_service_is_method_callable, 0, 0, 1)
	ZEND_ARG_INFO(0, method) /* string */
ZEND_END_ARG_INFO()


//entry
zend_class_entry *hessian_service_entry;



/*
	HessianService::__construct
*/
static PHP_METHOD(HessianService, __construct)
{
	zval *service_object, *self, *options, *arg_options;
	zval *type_map, *factory;
	zval  *hessian_type_map;
	zval function_name;
	zval *params[1], *timezone;
	zval z_true;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &service_object, &arg_options) == FAILURE) {
		RETURN_FALSE;
	}

	self = getThis();
	
	/*
	$this->options = HessianOptions::resolveOptions($options);
		if($serviceObject)
			$this->registerObject($serviceObject);
		$this->options->detectVersion = true;
		$this->typemap = new HessianTypeMap($this->options->typeMap);
		$this->factory = new HessianFactory();
		
		// timezones
		HessianUtils::setTimeZone($this->options->timeZone);
	*/


	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "HessianOptions::resolveOptions", 1);
	params[0] = arg_options;
	call_user_function(EG(function_table), NULL, &function_name, options, 1, params TSRMLS_CC);

	zend_update_property(hessian_service_entry, self, ZEND_STRL("options"), options TSRMLS_DC);
	if (i_zend_is_true(service_object)){
		//$this->registerObject($serviceObject);
	}
	ZVAL_BOOL(&z_true, 1);
	//@todo memory ok?
	zend_update_property(NULL, options, ZEND_STRL("detectVersion"), &z_true TSRMLS_DC);

	//$this->typemap = new HessianTypeMap($this->options->typeMap);
	type_map = zend_read_property(NULL, options, ZEND_STRL("typeMap"), 1 TSRMLS_DC);
	ALLOC_ZVAL(hessian_type_map);	
	object_init_ex(hessian_type_map, hessian_type_map_entry);
	ZVAL_STRING(&function_name, "__construct", 1);
	params[0] = type_map;
	call_user_function(NULL, &hessian_type_map, &function_name, NULL, 1, params TSRMLS_CC);
	zend_update_property(hessian_service_entry, self, ZEND_STRL("typemap"), hessian_type_map TSRMLS_DC);

	//$this->factory = new HessianFactory();
	ALLOC_ZVAL(factory);
	object_init_ex(factory, hessian_factory_entry);
	ZVAL_STRING(&function_name, "__construct", 1);
	call_user_function(NULL, &factory, &function_name, NULL, 0, params TSRMLS_CC);
	zend_update_property(hessian_service_entry, self, ZEND_STRL("factory"), factory TSRMLS_DC);


	//HessianUtils::setTimeZone($this->options->timeZone);
	timezone = zend_read_property(NULL, options, ZEND_STRL("timeZone"), 1 TSRMLS_DC);
	params[0] = timezone;
	ZVAL_STRING(&function_name, "HessianUtils::setTimeZone", 1);
	call_user_function(NULL, &factory, &function_name, NULL, 1, params TSRMLS_CC);
}


/*
	HessianService::registerObject
*/
static PHP_METHOD(HessianService, registerObject)
{
	zval *self, *service;
	char *name="HessianPHP_Service";
	int name_len;

	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &service, &service) == FAILURE) {
		RETURN_FALSE;
	}

	self = getThis();

	/*
		if(is_object($service)){
			$name = get_class($service);
			$this->service = $service;
		}
	*/
	if (Z_TYPE_P(service) == IS_OBJECT){
		zend_get_object_classname(service, (const char**)&name, &name_len TSRMLS_CC);
		zend_update_property(hessian_service_entry, self, ZEND_STRL("service"), service TSRMLS_DC);
	}

	/*
	if(is_string($service)){
			$name = $service;
			//TODO use object factory
			$this->service = new $service();
		}
	*/

	if (Z_TYPE_P(service) == IS_STRING){
		zend_class_entry *cls_service;
		name = Z_STRVAL_P(service);
		cls_service = zend_fetch_class(name, strlen(name)-1, 0 TSRMLS_DC);
		ALLOC_ZVAL(service);
		object_init_ex(service, cls_service);
		zend_update_property(hessian_service_entry, self, ZEND_STRL("service"), service TSRMLS_DC);
	}

	/*
	if(!is_object($this->service))
			throw new Exception("Error registering service object");
	*/
	
	if (IS_OBJECT != Z_TYPE_P(service)){
		//@todo error code
		zend_class_entry *cls_exception;
		cls_exception = zend_fetch_class("Exception", strlen("Exception")-1, 0 TSRMLS_DC);
		zend_throw_exception(cls_exception, "Error registering service object",8 TSRMLS_DC);
		return;
	}

	/*
	$this->reflected = new ReflectionObject($this->service);
		if($this->options->serviceName == '')
			$this->options->serviceName = $name;
	*/
	zval *reflection_object, *reflected;
	zend_class_entry *zce_reflection_object;
	zval *service_name;
	zval *options;
	
	zce_reflection_object = zend_fetch_class("ReflectionObject", strlen("ReflectionObject")-1, 0 TSRMLS_DC);
	ALLOC_ZVAL(reflection_object);
	object_init_ex(reflection_object, zce_reflection_object);

	options = zend_read_property(NULL, self, ZEND_STRL("options"), 1 TSRMLS_DC);
	service_name = zend_read_property(NULL, options, ZEND_STRL("serviceName"), 1 TSRMLS_DC);
	if (0 == Z_STRLEN_P(service_name)){
		ALLOC_ZVAL(service_name);
		ZVAL_STRING(service_name, name, 1);
		zend_update_property(NULL, options, ZEND_STRL("serviceName"), service_name TSRMLS_DC);
	}
}


/*
	HessianService::writeFault
*/
static PHP_METHOD(HessianService, writeFault)
{
	zval *self, *ex, *writer, ex_dump, detail;
	zval *message, *trace_as_string;
	zval header,  function_name;
	zend_class_entry *ce_hessia_fault;
	zval *params[2];
	char *str_dump;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &ex, &writer) == FAILURE) {
		RETURN_FALSE;
	}
	ZVAL_STRING(&header, "HTTP/1.1 500 Hessian service error", 1);
	sapi_header_op(SAPI_HEADER_ADD, (void*)&header TSRMLS_CC);

	ce_hessia_fault = zend_fetch_class("HessianFault", strlen("HessianFault")-1, 0 TSRMLS_DC);

	if (instanceof_function(Z_OBJCE_P(ex), ce_hessia_fault TSRMLS_DC)){
		INIT_ZVAL(function_name);
		ZVAL_STRING(&function_name, "detail", 1);
		call_user_function(NULL, &ex, &function_name, &detail, 0, params TSRMLS_DC);
	}else{
		ZVAL_NULL(&detail);
	}
	/*
	$exDump = $ex->getMessage()."\n".$ex->getTraceAsString();
		$detail = null;
		if($ex instanceof HessianFault)
			$detail = $ex->detail;
		if(isset($writer)){
			return $writer->writeFault($ex, $detail);
		} else{
			die($exDump);		
		}
	*/
	if (i_zend_is_true(writer)){
		zval *res;
		INIT_ZVAL(function_name);
		ZVAL_STRING(&function_name, "writeFault", 1);
		params[0] = ex;
		params[1] = writer;
		call_user_function(NULL, &ex, &function_name, res, 2, params TSRMLS_DC);
		RETURN_ZVAL(res, 0, NULL);
	}
	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "getMessage", 1);
	call_user_function(NULL, &ex, &function_name, message, 0, params TSRMLS_DC);

	ZVAL_STRING(&function_name, "getTraceAsString", 1);
	call_user_function(NULL, &ex, &function_name, trace_as_string, 0, params TSRMLS_DC);

	
	INIT_ZVAL(ex_dump);
	str_dump = pemalloc(Z_STRLEN_P(message) + Z_STRLEN_P(trace_as_string) + 1, 0);
	strcat(str_dump, Z_STRVAL_P(message));
	strcat(str_dump+Z_STRLEN_P(message), Z_STRVAL_P(trace_as_string));
	str_dump[Z_STRLEN_P(message) + Z_STRLEN_P(trace_as_string)] = 0;
	ZVAL_STRING(&ex_dump, str_dump, 1);
	zend_print_variable(&ex_dump);
	zend_bailout();

	//efree(str_dump);
}


/*
	HessianService::handle
*/
static PHP_METHOD(HessianService, handle)
{
	zval *self;
	zval *service;
	zval *params[2];
	zval function_name;
	zval *payload;
	
	self = getThis();
	/*
		if(!is_object($this->service)){
			header("HTTP/1.1 500 Hessian not configured");
			die('Serviced object not registered!');
		}
	*/
	service = zend_read_property(hessian_service_entry, self, ZEND_STRL("service"), 1 TSRMLS_DC);
	if (Z_TYPE_P(service) != IS_OBJECT){
		zval header, str;
		ZVAL_STRING(&header, "HTTP/1.1 500 Hessian not configured", 1);
		sapi_header_op(SAPI_HEADER_ADD, &header TSRMLS_CC);
		ZVAL_STRING(&str, "Serviced object not registered!", 1);
		zend_print_variable(&str);
		zend_bailout();
	}

	/*
		if($_SERVER['REQUEST_METHOD'] != 'POST') {
			if($this->options->displayInfo) {
				$this->displayInfo();
				exit();
			} else {
				header("HTTP/1.1 500 Hessian Requires POST");
				die('<h1>Hessian Requires POST</h1>');
			}
		}
	*/

	ulong hash_value;
	zval varname;
	zval *server, *request_method;
	
	ZVAL_STRING(&varname, "_SERVER", 1);
	hash_value = Z_HASH_P(&varname);
	zend_hash_quick_find(&EG(symbol_table), Z_STRVAL(varname), Z_STRLEN(varname)+1, hash_value, (void **) &server);
	zend_hash_find(Z_ARRVAL_P(server), "REQUEST_METHOD", strlen("REQUEST_METHOD")-1, (void **)&request_method);
	if (strcmp(Z_STRVAL_P(request_method), "POST") == 0){
		zval *options, *display_info;
		
		options = zend_read_property(NULL , self, ZEND_STRL("options"), 1 TSRMLS_DC);
		display_info = zend_read_property(NULL , options, ZEND_STRL("displayInfo"), 1 TSRMLS_DC);
		if (i_zend_is_true(display_info)){
			ZVAL_STRING(&function_name, "displayInfo", 1);
			call_user_function(NULL, &self, &function_name, NULL, 0, params TSRMLS_DC);
			zend_bailout();
		}else{
			zval header, error_str;
			
			ZVAL_STRING(&header, "HTTP/1.1 500 Hessian Requires POST", 1);
			sapi_header_op(SAPI_HEADER_ADD, &header TSRMLS_CC);
			ZVAL_STRING(&error_str, "<h1>Hessian Requires POST</h1>", 1);
			zend_print_variable(&error_str);
			zend_bailout();
		}
	}

	//set_error_handler('hessianServiceErrorHandler');
	if (EG(user_error_handler)) {
		zend_stack_push(&EG(user_error_handlers_error_reporting), &EG(user_error_handler_error_reporting), sizeof(EG(user_error_handler_error_reporting)));
		zend_ptr_stack_push(&EG(user_error_handlers), EG(user_error_handler));
	}

	zval *error_handle;
	ALLOC_ZVAL(error_handle);
	ZVAL_STRING(error_handle, "hessianServiceErrorHandler", 1);
	ALLOC_ZVAL(EG(user_error_handler));
	MAKE_COPY_ZVAL(&error_handle, EG(user_error_handler));


	/*
		$ph = fopen("php://input", "rb");
		$instream = new HessianBufferedStream($ph);
		$ofp = fopen("php://output","wb+");
		$outstream = new HessianBufferedStream($ofp);
	*/

	php_stream *ph, *ofp;
	zval *pz_ph, *pz_ofp;
	zval *in_stream, *out_stream;

	ph = php_stream_open_wrapper_ex("php://input", "rb", 0 | REPORT_ERRORS, NULL, NULL);
	ALLOC_ZVAL(in_stream);
	object_init_ex(in_stream, hessian_buffered_stream_entry);
	ZVAL_STRING(&function_name, "__construct", 1);
	php_stream_to_zval(ph, pz_ph);
	params[0] = pz_ph;
	call_user_function(NULL, &self, &function_name, NULL, 1, params TSRMLS_DC);
	ofp = php_stream_open_wrapper_ex("php://output", "wb+", 0 | REPORT_ERRORS, NULL, NULL);
	ALLOC_ZVAL(out_stream);
	object_init_ex(out_stream, hessian_buffered_stream_entry);
	ZVAL_STRING(&function_name, "__construct", 1);
	php_stream_to_zval(ofp, pz_ofp);
	params[0] = pz_ofp;
	call_user_function(NULL, &self, &function_name, NULL, 1, params TSRMLS_DC);


	/*
		$writer = null;
		$payload = '';
		ob_start(); 
	*/

	if (php_output_start_user(NULL, 0, 0 TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "failed to create buffer");
		RETURN_FALSE;
	}
	
	//exception handle
	//exception = EG(exception);
	ZVAL_STRING(&function_name, "readAll", 1);
	php_stream_to_zval(ofp, pz_ofp);
	params[0] = pz_ofp;
	call_user_function(NULL, &in_stream, &function_name, NULL, 0, params TSRMLS_DC);

	zval *factory, *parser, *options;
	zval *type_map;
	
	factory = zend_read_property(NULL, self, ZEND_STRL("factory"), 1 TSRMLS_DC);
	options = zend_read_property(NULL, self, ZEND_STRL("options"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "getParser", 1);
	params[0] = in_stream;
	params[1] = options;

	//$parser->setTypeMap($this->typemap);
	call_user_function(NULL, &self, &function_name, parser, 2, params TSRMLS_DC);
	type_map = zend_read_property(NULL, self, ZEND_STRL("typemap"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "setTypeMap", 1);
	params[0] = type_map;
	call_user_function(NULL, &parser, &function_name, NULL, 0, params TSRMLS_DC);

	/*
		$writer = $this->factory->getWriter($instream, $this->options);
		$writer->setTypeMap($this->typemap);
	*/
	zval *writer;
	ZVAL_STRING(&function_name, "setTypeMap", 1);
	params[0] = writer;
	params[1] = options;
	call_user_function(NULL, &parser, &function_name, writer, 2, params TSRMLS_DC);
	//ZVAL_STRING(&function_name, "setTypeMap", 1);
	params[0] = type_map;
	call_user_function(NULL, &writer, &function_name, NULL, 0, params TSRMLS_DC);


	/*
		$ctx = new HessianCallingContext();
		$ctx->isClient = false;
		$ctx->writer = $writer;
		$ctx->parser = $parser;
		$ctx->options = $this->options;
		$ctx->typemap = $this->typemap;
	*/

	zval *ctx;
	zval z_false;
	
	ALLOC_ZVAL(ctx);
	object_init_ex(ctx, hessian_calling_context_entry);
	ZVAL_BOOL(&z_false, 0);
	zend_update_property(NULL, ctx, ZEND_STRL("isClient"), &z_false TSRMLS_DC);
	zend_update_property(NULL, ctx, ZEND_STRL("writer"), writer TSRMLS_DC);
	zend_update_property(NULL, ctx, ZEND_STRL("parser"), parser TSRMLS_DC);
	zend_update_property(NULL, ctx, ZEND_STRL("options"), options TSRMLS_DC);
	zend_update_property(NULL, ctx, ZEND_STRL("typemap"), type_map TSRMLS_DC);

	/*
		$call = $parser->parseTop();
		$instream->close();	
		$ctx->call = $call;
	*/
	zval *call;
	ZVAL_STRING(&function_name, "parseTop", 1);
	call_user_function(NULL, &parser, &function_name, call, 0, params TSRMLS_DC);
	ZVAL_STRING(&function_name, "close", 1);
	call_user_function(NULL, &in_stream, &function_name, NULL, 0, params TSRMLS_DC);
	zend_update_property(NULL, ctx, ZEND_STRL("call"), call TSRMLS_DC);

	/*
		foreach($this->options->interceptors as $interceptor){
			$interceptor->beforeRequest($ctx);
		}
	*/
	zval *interceptors;
	HashPosition pos;
	zval *src_entry;
	params[0] = ctx;
	ZVAL_STRING(&function_name, "beforeRequest", 1);
	
	interceptors = zend_read_property(NULL, options, ZEND_STRL("interceptors"), 1 TSRMLS_DC);
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(interceptors), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(interceptors), (void **)&src_entry, &pos) == SUCCESS) {
		zend_hash_move_forward_ex(Z_ARRVAL_P(interceptors), &pos);
		call_user_function(NULL, &src_entry, &function_name, NULL, 0, params TSRMLS_DC);
	}

	/*
		$result = $this->callMethod($call->method,	$call->arguments);
		$payload = $writer->writeReply($result);
		$ctx->payload = $payload;
	*/
	zval *method, *arguments, *result;
	method = zend_read_property(NULL, call, ZEND_STRL("method"), 1 TSRMLS_DC);
	arguments = zend_read_property(NULL, call, ZEND_STRL("arguments"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "callMethod", 1);
	params[0] = method;
	params[1] = arguments;
	call_user_function(NULL, &self, &function_name, result, 2, params TSRMLS_DC);
	ZVAL_STRING(&function_name, "writeReply", 1);
	params[0] = result;
	call_user_function(NULL, &writer, &function_name, payload, 1, params TSRMLS_DC);
	zend_update_property(NULL, ctx, ZEND_STRL("payload"), payload TSRMLS_DC);


	/*
		foreach($this->options->interceptors as $interceptor){
				$interceptor->afterRequest($ctx);
			}
	*/
	ZVAL_STRING(&function_name, "afterRequest", 1);
	params[0] = ctx;
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(interceptors), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(interceptors), (void **)&src_entry, &pos) == SUCCESS) {
		zend_hash_move_forward_ex(Z_ARRVAL_P(interceptors), &pos);
		call_user_function(NULL, &src_entry, &function_name, NULL, 1, params TSRMLS_DC);
	}


	/*
		catch (Exception $ex){
			$payload = $this->writeFault($ex, $writer);
		}
	*/

	/*
		$stdOutput = ob_get_contents();
		ob_end_clean();
	*/

	zval *std_output;
	if (php_output_get_contents(std_output TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	php_output_discard(TSRMLS_C);

	/*
		if($stdOutput != '' && !$this->options->ignoreOutput){
			$ex = new HessianFault('Unclean output detected', 0);
			$ex->detail = $stdOutput;
			$payload = $this->writeFault($ex, $writer);	
		}
	*/

	zval *ignore_output;
	ignore_output = zend_read_property(NULL, options, ZEND_STRL("ignoreOutput"), 1 TSRMLS_DC);
	if (Z_STRLEN_P(std_output) > 0 && !i_zend_is_true(ignore_output)){
		zval *ex;
		zval param1, param2;
		
		ALLOC_ZVAL(ex);
		object_init_ex(ex, hessian_fault_entry);
		ZVAL_STRING(&function_name, "__construct", 1);
		ZVAL_STRING(&param1, "Unclean output detected", 1);
		ZVAL_LONG(&param2, 0);
		params[0] = &param1;
		params[1] = &param2;
		call_user_function(NULL, &src_entry, &function_name, NULL, 2, params TSRMLS_DC);
		zend_update_property(NULL, ex, ZEND_STRL("detail"), std_output TSRMLS_DC);

		//$payload = $this->writeFault($ex, $writer);	
		ZVAL_STRING(&function_name, "writeFault", 1);
		params[0] = ex;
		params[1] = writer;
		call_user_function(NULL, &writer, &function_name, payload, 2, params TSRMLS_DC);
	}


	/*
	if(extension_loaded("mbstring"))
			$size = mb_strlen($payload, 'latin1');
		else{
			$size = count(str_split($payload));
		}
	*/
	long size;
	zval *mb_res;
	if (zend_hash_exists(&module_registry, "mbstring", strlen("mbstring"))) {
		zval latin1;
		ZVAL_STRING(&function_name, "mb_strlen", 1);
		ZVAL_STRING(&latin1, "latin1", 1);
		params[0] = payload;
		params[1] = &latin1;
		call_user_function(NULL, &writer, &function_name, mb_res, 2, params TSRMLS_DC);
		size = Z_LVAL_P(mb_res);
	}else{
		size = Z_STRLEN_P(payload);
	}


	/*
		header('Content-type: application/binary');
		header('Content-length: ' . $size);
		header('Connection: close');
	*/
	zval header1, header2;
	ZVAL_STRING(&header1, "Content-type: application/binary", 1);
	sapi_header_op(SAPI_HEADER_ADD, &header1 TSRMLS_CC);
	ZVAL_STRING(&header2, "Connection: close", 1);
	sapi_header_op(SAPI_HEADER_ADD, &header2 TSRMLS_CC);

	/*
		$outstream->write($payload);			
		$outstream->flush();
		$outstream->close();
	*/

	ZVAL_STRING(&function_name, "write", 1);
	params[0] = payload;
	
	call_user_function(NULL, &out_stream, &function_name, mb_res, 1, params TSRMLS_DC);
	ZVAL_STRING(&function_name, "flush", 1);
	
	call_user_function(NULL, &out_stream, &function_name, mb_res, 0, params TSRMLS_DC);
	ZVAL_STRING(&function_name, "close", 1);
	call_user_function(NULL, &out_stream, &function_name, mb_res, 0, params TSRMLS_DC);
}



/*
	HessianService::displayInfo
*/
static PHP_METHOD(HessianService, displayInfo)
{
	zval *self;
	
	self = getThis();
	
}

/*
	HessianService::callMethod
*/
static PHP_METHOD(HessianService, callMethod)
{
	zval *self;
	zval *method, *arg_params;
	zval *reflected, *method_obj;
	zval function_name;
	zval *params[2];
	
	self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &method, &arg_params) == FAILURE) {
		RETURN_FALSE;
	}

	/*
		$methodObj = $this->reflected->getMethod($method);
		if($this->isMethodCallable($methodObj)){
			return call_user_func_array(array($this->service,$method),$params);
		}
		else 
			throw new Exception("Method $method is not callable");
	*/
	reflected = zend_read_property(NULL, self, ZEND_STRL("reflected"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "getMethod", 1);
	params[1] = method;
	call_user_function(NULL, &reflected, &function_name, method_obj, 1, params TSRMLS_DC);

	zval *callable;
	ZVAL_STRING(&function_name, "isMethodCallable", 1);
	params[1] = method_obj;
	call_user_function(NULL, &self, &function_name, callable, 1, params TSRMLS_DC);
	if (i_zend_is_true(callable)){
		zval *ret_val;
		zval *fun;

		ALLOC_ZVAL(fun);
		array_init_size(fun, 2);
		zend_hash_next_index_insert(Z_ARRVAL_P(fun), self, sizeof(zval *), NULL);
		zend_hash_next_index_insert(Z_ARRVAL_P(fun), method, sizeof(zval *), NULL);
		call_user_function_helper(fun, arg_params, ret_val, EG(function_table));
		FREE_ZVAL(fun);
	}else{
		zend_class_entry *ce_exception;

		ce_exception = zend_fetch_class("Exception", strlen("Exception")-1, 0 TSRMLS_DC);
		zend_throw_exception(ce_exception, "Method $method is not callable", 8);
	}
}


/*
	HessianService::isMethodCallable
*/
static PHP_METHOD(HessianService, isMethodCallable)
{
	zval *self, *method, *name, *service;
	zval function_name;
	zval *params[2];
	
	self = getThis();
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &method) == FAILURE) {
		RETURN_FALSE;
	}

	ZVAL_STRING(&function_name, "getName", 1);
	call_user_function(NULL, &method, &function_name, name, 0, params TSRMLS_DC);

	/*
		if($name == '__construct' || $name == '__destruct' || strcasecmp($name, get_class($this->service)) == 0) 
			// always exclude constructors and destructors
			return false;
	*/
	if ( (0 == strcmp("__construct", Z_STRVAL_P(name)) )
		|| (0 == strcmp("__destruct", Z_STRVAL_P(name))) ){
		RETURN_TRUE;
	}

	service = zend_read_property(NULL, self, ZEND_STRL("service"), 1 TSRMLS_DC);
	if (0 == strcmp(Z_OBJCE_P(service)->name, Z_STRVAL_P(name)) ){
		RETURN_TRUE;
	}

	zval *res;
	ZVAL_STRING(&function_name, "isPublic", 1);
	call_user_function(NULL, &method, &function_name, res, 0, params TSRMLS_DC);
	if (!i_zend_is_true(res)){
		RETURN_FALSE;
	}

	RETURN_TRUE;;
}


//hessianServiceErrorHandler
PHP_FUNCTION(hessianServiceErrorHandler)
{
	long err_no, err_line;
	zval *err_str, *err_file, ex, z_err_msg;
	int err_str_len, err_file_len;
	char err_msg[512];
	int len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lssl", &err_no, &err_str, &err_str_len, &err_file, &err_file_len, &err_line) == FAILURE) {
		RETURN_FALSE;
	}

	/*
	switch ($errno) {
	    case E_USER_ERROR:
	        $msg = "<b>Fatal error</b> [$errno] $errstr<br />\n";
	        break;
	    case E_USER_WARNING:
	        $msg = "<b>WARNING</b> [$errno] $errstr<br />\n";
	        break;

	    case E_USER_NOTICE:
	        $msg = "<b>NOTICE</b> [$errno] $errstr<br />\n";
	        break;
	    default:
	        $msg = "Unknown error type: [$errno] $errstr<br />\n";
	        break;
	    }
    */

	switch(err_no){
		case 256:	//E_USER_ERROR
			len = sprintf(err_msg, "<b>Fatal error</b> [%d] %s<br />\n", err_no, err_str);
			break;
		case 512:	//E_USER_WARNING
			len = sprintf(err_msg, "<b>WARNING</b> [%d] %s<br />\n", err_no, err_str);
			break;
		case 1024:	//E_USER_NOTICE
			len = sprintf(err_msg, "<b>NOTICE</b> [%d] %s<br />\n", err_no, err_str);
			break;
		default:
			len = sprintf(err_msg, "Unknown error type: [%d] %s<br />\n", err_no, err_str);
			break;
	}

	/*
		$msg .= "on line $errline in file $errfile";
	    $ex = new HessianFault($msg, $errno);
	    $ex->detail = $msg;
	    throw $ex;
	    return true;
    */
    sprintf(err_msg + len, "on line %d in file %s", err_line, err_file);
	object_init_ex(&ex, hessian_fault_entry);
	INIT_ZVAL(ex);
	ZVAL_STRING(&z_err_msg, err_msg, 1);
	zend_update_property(NULL, &ex, ZEND_STRL("detail"), &z_err_msg TSRMLS_DC);
	zend_throw_exception(hessian_fault_entry, err_msg, err_no TSRMLS_DC);
	RETURN_TRUE;
}


//HessianService functions
const zend_function_entry hessian_service_functions[] = {
	PHP_ME(HessianService, __construct, 	arginfo_hessian_service_construct,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianService, registerObject, 	arginfo_hessian_service_register_object,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianService, writeFault, 	arginfo_hessian_service_write_fault,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianService, handle, 	arginfo_hessian_service_handle,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianService, displayInfo, 	arginfo_hessian_service_display_info,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianService, callMethod, 	arginfo_hessian_service_call_method,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianService, isMethodCallable, 	arginfo_hessian_service_is_method_callable,		ZEND_ACC_PROTECTED)
	PHP_FE(hessianServiceErrorHandler,  NULL)
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
