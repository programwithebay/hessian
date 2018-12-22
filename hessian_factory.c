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





zend_class_entry *ihessian_object_factory_entry;
zend_class_entry *hessian_object_factory_entry;
zend_class_entry *hessian_datetime_adapter_entry;
zend_class_entry *hessian_factory_entry;



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

	zend_update_property(hessian_client_entry, self, ZEND_STRL("options"), options TSRMLS_DC);
}

/*
	HessianObjectFactory::getObject
*/
static PHP_METHOD(HessianObjectFactory, getObject)
{
	zval *type;
	zval *self;
	zend_class_entry ce;
	
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
		options = zend_read_property(hessian_client_entry, self, ZEND_STRL("options"), 0 TSRMLS_DC);
		if (options){
			zval *strict_type;
			strict_type = zend_read_property(hessian_client_entry, options, ZEND_STRL("strictType"), 0 TSRMLS_DC);
			if (i_zend_is_true(strict_type){
				php_error_docref(NULL, E_ERROR, "Type %s cannot be found for object instantiation, check your type mappings", Z_STRVAL_P(type));
			}
		}
		zend_lookup_class(ZEND_STRL("stdClass"), &ce TSRMLS_CC);
		object_init_ex(return_value, &ce);
		Z_TYPE_P(return_value) = IS_OBJECT;
		return;
	}
	object_init_ex(return_value, &ce);
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

	string = php_format_date("c", 1, ts, 1 TSRMLS_CC);
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
	zend_hash_next_index_insert(Z_ARRVAL(arr1),self, sizeof(zval*), NULL);
	ALLOC_ZVAL(version1);
	ZVAL_STRING(version1, "loadVersion1", 1);
	zend_hash_next_index_insert(Z_ARRVAL(arr1),version1, sizeof(zval*), NULL);
	ALLOC_ZVAL(arr2);
	ZVAL_STRING(version1, "loadVersion2", 1);
	zend_hash_next_index_insert(Z_ARRVAL(arr2),self, sizeof(zval*), NULL);
	zend_hash_next_index_insert(Z_ARRVAL(arr2),version2, sizeof(zval*), NULL);


	ALLOC_ZVAL(protocols);
	array_init_size(protocols, 2);
	zend_hash_add(Z_ARRVAL(protocols), ZEND_STRL("2"), arr2, sizeof(zval*), NULL);
	zend_hash_add(Z_ARRVAL(protocols), ZEND_STRL("1"), arr1, sizeof(zval*), NULL);
	
	zend_update_property(hessian_factory_entry, self, ZEND_STRL("protocols"),  protocols TSRMLS_DC);

	//transports
	ALLOC_ZVAL(transports);
	array_init_size(transports, 3);
	ALLOC_ZVAL(curl);
	ZVAL_STRING(curl, "HessianCURLTransport", 1);
	zend_hash_add(Z_ARRVAL(transports), ZEND_STRL("CURL"), curl, sizeof(zval*), NULL);
	zend_hash_add(Z_ARRVAL(transports), ZEND_STRL("curl"), curl, sizeof(zval*), NULL);
	ALLOC_ZVAL(curl);
	ZVAL_STRING(http_stream, "HessianHttpStreamTransport", 1);
	zend_hash_add(Z_ARRVAL(transports), ZEND_STRL("http"), http_stream, sizeof(zval*), NULL);

	zend_update_property(hessian_factory_entry, self, ZEND_STRL("transports"),  transports TSRMLS_DC);
}



//IDubboStorage interface
const zend_function_entry hessian_object_factory_functions[] = {
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
