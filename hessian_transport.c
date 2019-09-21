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
#include "php_hessian_int.h"
#include <curl/curl.h>
#include <curl/easy.h>


//params
ZEND_BEGIN_ARG_INFO_EX(arginfo_ihessian_transport_get_stream, 0, 0, 3)
	ZEND_ARG_INFO(0, url) /* string */
	ZEND_ARG_INFO(0, data) /* string */
	ZEND_ARG_INFO(0, options) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ihessian_transport_test_available, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ihessian_transport_get_metadata, 0, 0, 0)
ZEND_END_ARG_INFO()


//entry
zend_class_entry *ihessian_transport_entry;
zend_class_entry *hessian_curl_transport_entry;



/*
	HessianCURLTransport::testAvailable
*/
void hessian_curl_transport_test_available(zval *self)
{
	zval function_name, param1, retval;
	zval *params[1];
	
	/*
		if(!function_exists('curl_init'))
			throw new Exception('You need to enable the CURL extension to use the curl transport');
	*/
	ZVAL_STRING(&function_name, "function_exists", 1);
	INIT_ZVAL(param1);
	ZVAL_STRING(&param1, "curl_init", 1);
	params[0] = &param1;

	call_user_function(EG(function_table), NULL, &function_name, &retval, 1, params  TSRMLS_DC);
	zval_dtor(&function_name);
	zval_dtor(&param1);
	
	if (Z_BVAL(retval) < 1){
		zend_error(E_WARNING, "You need to enable the CURL extension to use the curl transport", 0);
	}
}



/*
	HessianCURLTransport::testAvailable
*/
static PHP_METHOD(HessianCURLTransport, testAvailable)
{
	zval *self;

	self = getThis();
	/*
		if(!function_exists('curl_init'))
			throw new Exception('You need to enable the CURL extension to use the curl transport');
	*/
	hessian_curl_transport_test_available(self);
}

/*
	HessianCURLTransport::getMetadata
*/
static PHP_METHOD(HessianCURLTransport, getMetadata)
{
	zval *self, *meta_data;

	self = getThis();
	meta_data = zend_read_property(NULL, self, ZEND_STRL("metadata"), 1 TSRMLS_DC);
	RETURN_ZVAL(meta_data, 1, NULL);
}



/*
	HessianCURLTransport::getStream
*/
void hessian_curl_transport_get_stream(zval *self, zval *url, zval *data, zval *options, zval *return_value)
{
	zval curl_options;
	zval retval;
	zval function_name;
	zval *params[2];
	zval ch;
	zval curl_post;
	zval curl_follow_location;
	zval curl_return_transfer;
	zval curl_opt_header, curl_header_content_type;
	zval *transport_options;
	zval **find_res;
	zval *p;


	transport_options = zend_read_property(NULL, options, ZEND_STRL("transportOptions"), 1 TSRMLS_DC);

	//php_error_docref(NULL TSRMLS_CC, E_NOTICE, "get stream url:%s", Z_STRVAL_P(url) );
	INIT_ZVAL(ch);
	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "curl_init", 1);
	params[0] = url;
	call_user_function(EG(function_table), NULL, &function_name, &ch, 1, params TSRMLS_DC);
	zval_dtor(&function_name);


	
	//CURLOPT_URL
	INIT_ZVAL(curl_options);
	INIT_ZVAL(curl_post);
	array_init_size(&curl_options, 6);
	zend_hash_index_update(Z_ARRVAL(curl_options), 10002, &url, sizeof(zval*), NULL);
	ZVAL_LONG(&curl_post, 1);

	//CURLOPT_POST
	p = &curl_post;
	zend_hash_index_update(Z_ARRVAL(curl_options), 47, &p, sizeof(zval*), NULL);
	//CURLOPT_POSTFIELDS
	zend_hash_index_update(Z_ARRVAL(curl_options), 10015, &data, sizeof(zval*), NULL);
	

	//CURLOPT_FOLLOWLOCATION
	INIT_ZVAL(curl_follow_location);
	ZVAL_BOOL(&curl_follow_location, 1);
	p = &curl_follow_location;
	zend_hash_index_update(Z_ARRVAL(curl_options), 52, &p, sizeof(zval*), NULL); 
	
	

	//CURLOPT_RETURNTRANSFER
	INIT_ZVAL(curl_return_transfer);
	ZVAL_LONG(&curl_return_transfer, 1);
	p = &curl_return_transfer;
	zend_hash_index_update(Z_ARRVAL(curl_options), 19913, &p, sizeof(zval*), NULL);


	//CURLOPT_HTTPHEADER
	INIT_ZVAL(curl_opt_header);
	array_init_size(&curl_opt_header, 1);
	INIT_ZVAL(curl_header_content_type);
	ZVAL_STRING(&curl_header_content_type, "Content-Type: application/binary", 1);
	p = &curl_header_content_type;
	zend_hash_next_index_insert(Z_ARRVAL(curl_opt_header), &p, sizeof(zval *), NULL);
	p = &curl_opt_header;
	zend_hash_index_update(Z_ARRVAL(curl_options), 10023, &p, sizeof(zval*), NULL);

	
	if (Z_TYPE_P(transport_options) == IS_ARRAY){
		if (SUCCESS == zend_hash_index_find(Z_ARRVAL_P(transport_options), 10023, (void **)find_res)){
			//$curlOptions[CURLOPT_HTTPHEADER] = array_merge($curlOptions[CURLOPT_HTTPHEADER]	, $extra[CURLOPT_HTTPHEADER]);
			php_array_merge(Z_ARRVAL(curl_opt_header), Z_ARRVAL_PP(find_res), 0 TSRMLS_DC);
			p = &curl_opt_header;
			zend_hash_index_update(Z_ARRVAL(curl_options), 10023, &p, sizeof(zval*), NULL);
		}
		//zend_hash_index_update(Z_ARRVAL(curl_options), 10023, &p_curl_opt_header, sizeof(zval*), NULL);
		php_array_merge(Z_ARRVAL(curl_options), Z_ARRVAL_P(transport_options), 0 TSRMLS_CC);
	}


	INIT_ZVAL(retval);
	ZVAL_STRING(&function_name, "curl_setopt_array", 1);
	params[0] = &ch;
	params[1] = &curl_options;
	call_user_function(EG(function_table), NULL, &function_name, &retval, 2, params TSRMLS_CC);

	//free mem
	zval_dtor(&function_name);
	zval_dtor(&curl_header_content_type);
	//zval_dtor(&curl_opt_header);
	//zval_dtor(&curl_options);
	

	
	
	/*
	$result = curl_exec($ch);
		$this->metadata = curl_getinfo($ch);
		$error = curl_error($ch);
		$code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
		if($error){
			$this->safeClose($ch);
			throw new Exception("CURL transport error: $error");
		}
	*/

	
	zval *curl_info, curl_code_param, curl_code, curl_error;
	zval *curl_result;

	MAKE_STD_ZVAL(curl_result);
	
	INIT_ZVAL(retval);
	ZVAL_STRING(&function_name, "curl_exec", 1);
	params[0] = &ch;
	call_user_function(EG(function_table), NULL, &function_name, curl_result, 1, params TSRMLS_CC);
	zval_dtor(&function_name);



	ZVAL_STRING(&function_name, "curl_getinfo", 1);
	MAKE_STD_ZVAL(curl_info);
	params[0] = &ch;
	call_user_function(EG(function_table), NULL, &function_name, curl_info, 1, params TSRMLS_CC);
	zend_update_property(NULL, self, ZEND_STRL("metadata"), curl_info TSRMLS_CC);
	zval_dtor(&function_name);


	INIT_ZVAL(curl_error);
	ZVAL_STRING(&function_name, "curl_error", 1);
	params[0] = &ch;
	call_user_function(EG(function_table), NULL, &function_name, &curl_error, 1, params TSRMLS_CC);
	zval_dtor(&function_name);


	INIT_ZVAL(curl_code);
	ZVAL_STRING(&function_name, "curl_getinfo", 1);
	INIT_ZVAL(curl_code_param);
	ZVAL_LONG(&curl_code_param, 2097154);
	params[0] = &ch;
	params[1] = &curl_code_param;
	call_user_function(EG(function_table), NULL, &function_name, &curl_code, 2, params TSRMLS_CC);
	zval_dtor(&function_name);


	if (i_zend_is_true(&curl_error)){
		if (Z_TYPE(ch) == IS_RESOURCE){
			ZVAL_STRING(&function_name, "curl_close", 1);
			params[0] = &ch;
			call_user_function(EG(function_table), NULL, &function_name, &retval, 1, params TSRMLS_CC);
			zval_dtor(&function_name);

			char err_msg[800];
			sprintf(err_msg, "CURL transport error: %s", Z_STRVAL(curl_error));
			zend_error(E_ERROR, err_msg);

			zval_dtor(&curl_error);
			return;
		}
	}



	/*
		
		if($result === false) {
			$this->safeClose($ch);
			throw new Exception("curl_exec error for url $url");
		}

      */

	if ( (Z_TYPE_P(curl_result) == IS_BOOL) && (Z_BVAL_P(curl_result) < 1)){
		if (Z_TYPE(ch) == IS_RESOURCE){
			ZVAL_STRING(&function_name, "curl_close", 1);
			params[0] = &ch;
			call_user_function(EG(function_table), NULL, &function_name, &retval, 1, params TSRMLS_CC);
			zval_dtor(&function_name);
		}

		zval_dtor(&curl_code);

		char err_msg[800];

		sprintf(err_msg, "curl_exec error for url: %s", Z_STRVAL_P(url));
		zend_error(E_ERROR, err_msg);

		
		return;
	}

	/*
	if(!empty($options->saveRaw))
			$this->rawData = $result;
		$this->safeClose($ch);
	*/

	zval *save_raw;
	save_raw = zend_read_property(NULL, options, ZEND_STRL("saveRaw"), 1 TSRMLS_CC);
	if (i_zend_is_true(save_raw)){
		zend_update_property(NULL, self, ZEND_STRL("rawData"), curl_result TSRMLS_CC);
		if (Z_TYPE(ch) == IS_RESOURCE){
			ZVAL_STRING(&function_name, "curl_close", 1);
			params[0] = &ch;
			call_user_function(EG(function_table), NULL, &function_name, NULL, 1, params TSRMLS_CC);
			zval_dtor(&function_name);
		}
	}


	/*
	if($code != 200){
			$this->safeClose($ch);
		}
		$stream = new HessianStream($result);
		return $stream;
	*/

	if (Z_LVAL(curl_code) != 200){
		if (Z_TYPE(ch) == IS_RESOURCE){
			ZVAL_STRING(&function_name, "curl_close", 1);
			params[0] = &ch;
			call_user_function(EG(function_table), NULL, &function_name, &retval, 1, params TSRMLS_CC);
			zval_dtor(&function_name);
		}
		/*
		$msg = "Server error, returned HTTP code: $code";
			if(!empty($options->saveRaw))
				$msg .= " Server sent: ".$result;
			throw new Exception($msg);
		*/
		char str_message[800];

		//@todo:length is ok?
		sprintf(str_message, "Server error, returned HTTP code: %d Server sent:%s"
			, Z_LVAL(curl_code), Z_STRVAL(retval));
		

		zend_error(E_ERROR, str_message);
		zval_dtor(&curl_code);
			
		return;
	}

	zval stream;
	object_init_ex(&stream, hessian_stream_entry);

	//call __construct function
	hessian_stream_set_stream(&stream, curl_result);


	zval_dtor(&curl_error);
		
	RETURN_ZVAL(&stream, 1, NULL);
}






/*
	HessianCURLTransport::getStream
*/
static PHP_METHOD(HessianCURLTransport, getStream)
{
	zval *self;
	zval *url, *data, *options;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &url, &data, &options) == FAILURE) {
		RETURN_FALSE;
	}
	self = getThis();


	hessian_curl_transport_get_stream(self, url, data, options, return_value);
}




//IHessianTransport functions
const zend_function_entry ihessian_transport_functions[] = {
	PHP_ABSTRACT_ME(IHessianTransport, getStream, arginfo_ihessian_transport_get_stream)
	PHP_ABSTRACT_ME(IHessianTransport, testAvailable, arginfo_ihessian_transport_test_available)
	PHP_ABSTRACT_ME(IHessianTransport, getMetadata, arginfo_ihessian_transport_get_metadata)
	PHP_FE_END	/* Must be the last line in hessian_functions[] */
};

//HessianCURLTransport functions
const zend_function_entry hessian_curl_transport_functions[] = {
	PHP_ME(HessianCURLTransport, getStream, arginfo_ihessian_transport_get_stream, ZEND_ACC_PUBLIC)
	PHP_ME(HessianCURLTransport, testAvailable, arginfo_ihessian_transport_test_available, ZEND_ACC_PUBLIC)
	PHP_ME(HessianCURLTransport, getMetadata, arginfo_ihessian_transport_get_metadata, ZEND_ACC_PUBLIC)
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
