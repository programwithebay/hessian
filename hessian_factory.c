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



ZEND_BEGIN_ARG_INFO_EX(arginfo_ihessian_object_factory_get_object, 0, 0, 1)
	ZEND_ARG_INFO(0, type) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ihessian_object_factory_set_options, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, options, "HessianOptions", 0) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_object_factory_set_options, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, options, "HessianOptions", 0) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_object_factory_get_object, 0, 0, 1)
	ZEND_ARG_INFO(0, type) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_datetime_adapter_to_object, 0, 0, 1)
	ZEND_ARG_INFO(0, ts) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_datetime_adapter_write_time, 0, 0, 2)
	ZEND_ARG_INFO(0, date) /* string */
	ZEND_ARG_INFO(0, writer) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_factory_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_factory_get_parser, 0, 0, 2)
	ZEND_ARG_INFO(0, stream) /* string */
	ZEND_ARG_OBJ_INFO(0, options, "HessianOptions", 0) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_factory_get_writer, 0, 0, 2)
	ZEND_ARG_INFO(0, stream) /* string */
	ZEND_ARG_OBJ_INFO(0, options, "HessianOptions", 0) /* string */
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_factory_get_transport, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, options, "HessianOptions", 0) /* string */
ZEND_END_ARG_INFO()


zend_class_entry *ihessian_object_factory_entry;
zend_class_entry *hessian_object_factory_entry;
zend_class_entry *hessian_datetime_adapter_entry;
zend_class_entry *hessian_factory_entry;


//load parser
zval* hessian_object_factory_load_version2_parser(zval *self, zval *stream, zval *options){
	//$parser = new Hessian2ServiceParser($resolver, $stream, $options);
	zval *parser, function_name;
	zval *params[2];
	zval *filters, *date_array, *parse_filters;
	zval *date_param1, *date_param2, *callback_handler;

	ALLOC_ZVAL(parser);
	INIT_ZVAL(*parser);
	object_init_ex(parser, hessian2_service_parser_entry);
	params[0] = stream;
	params[1]= options;
	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "__construct", 1);
	call_user_function(NULL, &parser, &function_name, NULL, 2, params TSRMLS_CC);

	/*
		$filters['date'] = array('HessianDatetimeAdapter','toObject');
		$filters = array_merge($filters, $options->parseFilters);
		$parser->setFilters(new HessianCallbackHandler($filters));
	*/

	ALLOC_ZVAL(date_array);
	array_init_size(date_array, 2);

	ALLOC_ZVAL(date_param1);
	INIT_ZVAL(*date_param1);
	ZVAL_STRING(date_param1, "HessianDatetimeAdapter", 1);
	ALLOC_ZVAL(date_param2);
	INIT_ZVAL(*date_param2);
	ZVAL_STRING(date_param2, "toObject", 1);
	zend_hash_next_index_insert(Z_ARRVAL_P(date_array), &date_param1, sizeof(zval**), NULL);
	zend_hash_next_index_insert(Z_ARRVAL_P(date_array), &date_param2, sizeof(zval**), NULL);
	
	ALLOC_ZVAL(filters);
	array_init_size(filters, 1);
	zend_hash_add(Z_ARRVAL_P(filters), "date", 4, &date_array, sizeof(zval**), NULL);

	parse_filters = zend_read_property(NULL, options, "parseFilters", strlen("parseFilters")-1, 1 TSRMLS_DC);
	php_array_merge(Z_ARRVAL_P(filters), Z_ARRVAL_P(parse_filters), 0 TSRMLS_CC);

	ALLOC_ZVAL(callback_handler);
	object_init_ex(callback_handler, hessian_callback_handler_entry);
	ZVAL_STRING(&function_name, "__construct", 1);
	params[0] = filters;
	call_user_function(NULL, &callback_handler, &function_name, NULL, 1, params TSRMLS_CC);
	
	//$parser->setFilters(new HessianCallbackHandler($filters));
	ZVAL_STRING(&function_name, "setFilters", 1);
	params[0] = callback_handler;
	call_user_function(NULL, &parser, &function_name, NULL, 1, params TSRMLS_CC);

	
	return parser;

}

//load version2 writer
zval* hessian_object_factory_load_version2_writer(zval *self, zval *stream, zval *options){
	/*
		$writer = new Hessian2ServiceWriter($options);
		$filters['@DateTime'] = array('HessianDatetimeAdapter','writeTime');
		$filters['@Iterator'] = array( new Hessian2IteratorWriter(), 'write');
		$filters = array_merge($filters, $options->writeFilters);
		$writer->setFilters(new HessianCallbackHandler($filters));
		return $writer;
	*/
	zval *hessian2_service_writer, *filters;
	zval *time_arr, *time_arr_params1, *time_arr_params2;
	zval *iterator_arr, *iterator_arr_param1, *iterator_arr_param2;
	zval *parse_filters, *callback_handler;
	zval function_name;
	zval *params[2];

	ALLOC_ZVAL(hessian2_service_writer);
	INIT_ZVAL(*hessian2_service_writer);
	object_init_ex(hessian2_service_writer, hessian2_service_writer_entry);
	INIT_ZVAL(function_name);
	ZVAL_STRING(function_name, "__construct", 1);
	params[0] = options;
	call_user_function(NULL, &hessian2_service_writer, &function_name, NULL, 1, params TSRMLS_CC);


	
	ALLOC_ZVAL(time_arr_params1);
	ZVAL_STRING(&time_arr_params1, "HessianDatetimeAdapter", 1);
	ALLOC_ZVAL(time_arr_params2);
	ZVAL_STRING(&time_arr_params2, "writeTime", 1);
	ALLOC_ZVAL(time_arr);
	array_init_size(time_arr, 2);
	zend_hash_next_index_insert(Z_ARRVAL_P(time_arr), &time_arr_params1, sizeof(zval**), NULL);
	zend_hash_next_index_insert(Z_ARRVAL_P(time_arr), &time_arr_params2, sizeof(zval**), NULL);
	ALLOC_ZVAL(filters);
	array_init_size(filters, 2);
	zend_hash_add(Z_ARRVAL_P(filters), "@DateTime", strlen("@DateTime")-1, &time_arr, NULL);

	//add Iterator
	ALLOC_ZVAL(iterator_arr_param1);
	object_init_ex(iterator_arr_param1, hessian2_iterator_writer_entry);
	ZVAL_STRING(function_name, "__construct", 1);
	call_user_function(NULL, &iterator_arr_param1, &function_name, NULL, 0, params TSRMLS_CC);
	ALLOC_ZVAL(iterator_arr_param1);
	ZVAL_STRING(&iterator_arr_param1, "write", 1);
	ALLOC_ZVAL(iterator_arr);
	array_init_size(iterator_arr, 2);
	zend_hash_next_index_insert(Z_ARRVAL_P(iterator_arr), &iterator_arr_param1, sizeof(zval**), NULL);
	zend_hash_next_index_insert(Z_ARRVAL_P(iterator_arr), &iterator_arr_param2, sizeof(zval**), NULL);
	zend_hash_add(Z_ARRVAL_P(filters), "@Iterator", strlen("@Iterator")-1, &iterator_arr, NULL);
	
	parse_filters = zend_read_property(NULL, options, "parseFilters", strlen("parseFilters")-1, 1 TSRMLS_DC);
	php_array_merge(Z_ARRVAL_P(filters), Z_ARRVAL_P(parse_filters), 0 TSRMLS_CC);

	ALLOC_ZVAL(callback_handler);
	object_init_ex(callback_handler, hessian_callback_handler_entry);
	ZVAL_STRING(&function_name, "__construct", 1);
	params[0] = filters;
	call_user_function(NULL, &callback_handler, &function_name, NULL, 1, params TSRMLS_CC);

	ZVAL_STRING(&function_name, "setFilters", 1);
	params[0] = callback_handler;
	call_user_function(NULL, &hessian2_service_writer, &function_name, NULL, 1, params TSRMLS_CC);

	return hessian2_service_writer;
}



//load version 2 class config
zval* hessian_object_factory_load_version2(zval *self, char *mode, zval *stream, zval *options){
	if (0 == strcmp(mode, "parser")){
		return hessian_object_factory_load_version2_parser();
	}else if (0 == strcmp(mode, "writer")){
		return hessian_object_factory_load_version2_writer();
	}else if (0 == strcmp(mode, "detect")){
		//return Hessian2ServiceParser::detectVersion($stream);
		zval *retval;
		zval *params[1];
		zval function_name;
		int error_code;
		
		INIT_ZVAL(function_name);
		ZVAL_STRING(&function_name, "Hessian2ServiceParser::detectVersion", 1);
		params[0] = stream;
		error_code = call_user_function(EG(function_table), NULL, &function_name, retval, 1, params TSRMLS_DC);
		if (SUCCESS != error_code){
			php_error_docref(NULL, E_WARNING, "call Hessian2ServiceParser::detectVersion error:%d", error_code);
			return;
		}
		return retval;
	}
}

/*
	HessianObjectFactory::setOptions
*/
static PHP_METHOD(HessianObjectFactory, setOptions)
{
	zval *options;
	zval *self;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z",&options)) {
		return;
	}
	self = getThis();

	zend_update_property(hessian_object_factory_entry, self, ZEND_STRL("options"), options TSRMLS_DC);
}

/*
	HessianObjectFactory::getObject
*/
static PHP_METHOD(HessianObjectFactory, getObject)
{
	zval *type;
	zval *self;
	zend_class_entry **ce;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z",&type)) {
		return;
	}
	if (Z_TYPE_P(type) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "type must be an string");
		return;
	}
	self = getThis();

	/*
	if(!class_exists($type)) {
			if(isset($this->options->strictType) && $this->options->strictType)
				throw new Exception("Type $type cannot be found for object instantiation, check your type mappings");
			$obj = new stdClass();
			//$obj->__type = $type;
			return $obj;
		}
		return new $type();
	*/
	if (zend_lookup_class(Z_STRVAL_P(type), Z_STRLEN_P(type), &ce TSRMLS_CC) != SUCCESS) {
		zval  *options;
		options = zend_read_property(hessian_object_factory_entry, self, ZEND_STRL("options"), 0 TSRMLS_DC);
		if (options){
			zval *strict_type;
			strict_type = zend_read_property(NULL, options, ZEND_STRL("strictType"), 0 TSRMLS_DC);
			if (i_zend_is_true(strict_type)){
				php_error_docref(NULL, E_ERROR, "Type %s cannot be found for object instantiation, check your type mappings", Z_STRVAL_P(type));
			}
		}
		zend_lookup_class(ZEND_STRL("stdClass"), &ce TSRMLS_CC);
		object_init_ex(return_value, *ce);
		Z_TYPE_P(return_value) = IS_OBJECT;
		return;
	}
	object_init_ex(return_value, *ce);
	Z_TYPE_P(return_value) = IS_OBJECT;
}


/*
	HessianDatetimeAdapter::toObject
*/
static PHP_METHOD(HessianDatetimeAdapter, toObject)
{
	/*
	$date = date('c', $ts);
		//$date = gmdate('c', $ts);
		return new Datetime($date);	
	*/

	long    ts;
	char   *string;
	zend_class_entry *ce_datetime;
	zval *params[1];
	zval date;
	zval function_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &ts) == FAILURE) {
		RETURN_FALSE;
	}

	string = php_format_date("c", 1, ts, 1 TSRMLS_DC);
	ce_datetime = zend_fetch_class("Datetime", 8,0 TSRMLS_DC);
	object_init_ex(return_value, ce_datetime);

	INIT_ZVAL(date);
	ZVAL_STRING(&date, string, 0);
	params[0] = &date;
	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "__construct", 1);
	call_user_function(NULL, &return_value, &function_name, NULL, 1, params TSRMLS_CC);
}



/*
	HessianDatetimeAdapter::writeTime
*/
static PHP_METHOD(HessianDatetimeAdapter, writeTime)
{
	/*
		$ts = $date->format('U');
		$stream = $writer->writeDate($ts);
		return new HessianStreamResult($stream);
	*/

	zval *date, *writer, *stream;
	zval function_name;
	zval *params[2];
	zval format;
	zend_class_entry *ce_hessian_stream_result;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &date, &writer) == FAILURE) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(date) != IS_OBJECT){
		php_error_docref(NULL, E_WARNING, "date must be an object");
		return;
	}
	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "format", 1);
	INIT_ZVAL(format);
	ZVAL_STRING(&format, "U", 1);
	params[0] = &format;
	call_user_function(NULL, &return_value, &function_name, stream, 1, params TSRMLS_CC);

	ce_hessian_stream_result = zend_fetch_class("HessianStreamResult", strlen("HessianStreamResult")-1, 0 TSRMLS_DC);
	object_init_ex(return_value, ce_hessian_stream_result);

	//call construct 
	params[0] = stream;
	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "__construct", 1);
	call_user_function(NULL, &return_value, &function_name, NULL, 1, params TSRMLS_CC);
	Z_TYPE_P(return_value) = IS_OBJECT;
}


/*
	HessianFactory::__construct
*/
static PHP_METHOD(HessianFactory, __construct)
{
	/*
		$this->protocols = array(
			'2'=>array($this,'loadVersion2'), '1'=>array($this,'loadVersion1')
		);
		$this->transports = array(
			'CURL' => 'HessianCURLTransport',
			'curl' => 'HessianCURLTransport',
			'http' => 'HessianHttpStreamTransport'
		);
	*/

	zval *self, *arr1, *arr2, *version1, *version2;
	zval *transports, *curl, *http_stream;
	zval *protocols;
	
	self = getThis();
	
	ALLOC_ZVAL(arr1);
	array_init_size(protocols, 2);
	zend_hash_next_index_insert(Z_ARRVAL_P(arr1),self, sizeof(zval*), NULL);
	ALLOC_ZVAL(version1);
	ZVAL_STRING(version1, "loadVersion1", 1);
	zend_hash_next_index_insert(Z_ARRVAL_P(arr1),version1, sizeof(zval*), NULL);
	ALLOC_ZVAL(arr2);
	ZVAL_STRING(version1, "loadVersion2", 1);
	zend_hash_next_index_insert(Z_ARRVAL_P(arr2),self, sizeof(zval*), NULL);
	zend_hash_next_index_insert(Z_ARRVAL_P(arr2),version2, sizeof(zval*), NULL);


	ALLOC_ZVAL(protocols);
	array_init_size(protocols, 2);
	zend_hash_add(Z_ARRVAL_P(protocols), "2", 1, arr2, sizeof(zval*), NULL);
	zend_hash_add(Z_ARRVAL_P(protocols), "1", 1, arr1, sizeof(zval*), NULL);
	
	zend_update_property(hessian_factory_entry, self, ZEND_STRL("protocols"),  protocols TSRMLS_DC);

	//transports
	ALLOC_ZVAL(transports);
	array_init_size(transports, 3);
	ALLOC_ZVAL(curl);
	ZVAL_STRING(curl, "HessianCURLTransport", 1);
	zend_hash_add(Z_ARRVAL_P(transports), "CURL", 4, &curl, sizeof(zval*), NULL);
	zend_hash_add(Z_ARRVAL_P(transports), "curl",4, &curl, sizeof(zval*), NULL);
	ALLOC_ZVAL(curl);
	ZVAL_STRING(http_stream, "HessianHttpStreamTransport", 1);
	zend_hash_add(Z_ARRVAL_P(transports), "http", 4, &http_stream, sizeof(zval**), NULL);

	zend_update_property(hessian_factory_entry, self, "transports", strlen("transports")-1,  transports TSRMLS_DC);
}



/*
	HessianFactory::getParser
*/
static PHP_METHOD(HessianFactory, getParser)
{
	zval *self;
	zval *stream, *options, *parser;
	zval obj_factory;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &stream, &options) == FAILURE) {
		RETURN_FALSE;
	}
	/*
	$parser = call_user_func_array($callback, array('parser', $stream, $options));
	if($options->objectFactory instanceof IHessianObjectFactory){
		$parser->objectFactory = $options->objectFactory;
	} else {
		$parser->objectFactory = new HessianObjectFactory();
	}
	return $parser;
	*/

	
	//muse be version2
	parser = hessian_object_factory_load_version2(self, "parser", stream, options);
	//parser factory
	INIT_ZVAL(obj_factory);
	//always is HessianObjectFactory
	object_init_ex(&obj_factory, hessian_object_factory_entry);
	zend_update_property(NULL, parser, "objectFactory", strlen("objectFactory")-1, obj_factory TSRMLS_DC);
	RETURN_ZVAL(parser, 0, NULL);
}


/*
	HessianFactory::getWriter
*/
static PHP_METHOD(HessianFactory, getWriter)
{
	zval *self;
	zval *stream, *options, *writer;
	zval obj_factory;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &stream, &options) == FAILURE) {
		RETURN_FALSE;
	}
	/*
		$version = $options->version;
		if($options->detectVersion && $stream)
			$version = $this->detectVersion($stream, $options);
		$callback = $this->getConfig($version);
		$writer = call_user_func_array($callback, array('writer', $stream, $options));
		return $writer;
	*/

	//muse be version2
	//@todo:init 2 times
	writer = hessian_object_factory_load_version2(self, "writer", stream, options);
	RETURN_ZVAL(writer, 0, NULL);
}



/*
	HessianFactory::getTransport
*/
static PHP_METHOD(HessianFactory, getTransport)
{
	zval *self, *type, *transports;
	zval *options, *writer;
	zval obj_factory;
	zval *class;
	zval function_name;
	zend_class_entry *ce_hessian_exception, *ce_class;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z",&options) == FAILURE) {
		RETURN_FALSE;
	}
	/*
		$type = $options->transport;
		if(is_object($type))
			return $type;
		if(!isset($this->transports[$type]))
			throw new HessianException("The transport of type $type cannot be found");
		$class = $this->transports[$type];
		$trans = new $class();
		$trans->testAvailable();
		return $trans; 
	*/

	type = zend_read_property(NULL, options, "transport", strlen("transport")-1, 1 TSRMLS_DC);
	if (Z_TYPE_P(type) == IS_OBJECT){
		RETURN_ZVAL(type, 0, NULL);
	}

	transports = zend_read_property(NULL, self, "transports", strlen("transports")-1, 1 TSRMLS_DC);
	if (SUCCESS != zend_hash_find(Z_ARRVAL_P(transports), Z_STRVAL_P(type), Z_STRLEN_P(type), &class)){
		zend_throw_exception(ce_hessian_exception, sprintf("The transport of type %s cannot be found", Z_STRVAL_P(type))
			, 8TSRMLS_DC);
	}

	ce_class = zend_fetch_class(Z_STRVAL_P(class), Z_STRLEN_P(class), 0 TSRMLS_DC);
	if (!ce_class){
		php_error_docref(NULL, E_WARNING, "class %s not found", Z_STRVAL_P(class));
		return;
	}

	object_init_ex(return_value,class);
	INIT_ZVAL(function_name);
	ZVAL_STRING(function_name, "testAvailable", 1);
	call_user_function(NULL, return_value, &function_name, NULL, 0, NULL TSRMLS_CC);
}



//IDubboStorage interface
const zend_function_entry ihessian_object_factory_functions[] = {
	PHP_ABSTRACT_ME(IHessianObjectFactory, getObject, arginfo_ihessian_object_factory_get_object)
	PHP_ABSTRACT_ME(IHessianObjectFactory, setOptions, arginfo_ihessian_object_factory_set_options)
	PHP_FE_END
};



//HessianObjectFactory functions
const zend_function_entry hessian_object_factory_functions[] = {
	PHP_ME(HessianObjectFactory, setOptions, 	arginfo_hessian_object_factory_set_options,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianObjectFactory, getObject,		arginfo_hessian_object_factory_get_object,		ZEND_ACC_PUBLIC)
	PHP_FE_END	/* Must be the last line in hessian_functions[] */
};


//HessianClient functions
const zend_function_entry hessian_datetime_adapter_functions[] = {
	PHP_ME(HessianDatetimeAdapter, toObject, 	arginfo_hessian_datetime_adapter_to_object,		ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(HessianDatetimeAdapter, toObject, 	arginfo_hessian_datetime_adapter_write_time,		ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_FE_END	/* Must be the last line in hessian_functions[] */
};

//HessianClient functions
const zend_function_entry hessian_factory_functions[] = {
	PHP_ME(HessianFactory, __construct, 	arginfo_hessian_factory_construct,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianFactory, getParser, 	arginfo_hessian_factory_get_parser,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianFactory, getWriter, 	arginfo_hessian_factory_get_writer,		ZEND_ACC_PUBLIC)
	//PHP_ME(HessianFactory, getRulesResolver, 	arginfo_hessian_factory_get_rules_resolver,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianFactory, getTransport, 	arginfo_hessian_factory_get_transport,		ZEND_ACC_PUBLIC)
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
