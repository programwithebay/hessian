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
static PHP_METHOD(HessianCURLTransport, testAvailable)
{
	zend_function *func;
	/*
		if(!function_exists('curl_init'))
			throw new Exception('You need to enable the CURL extension to use the curl transport');
	*/

	if (zend_hash_find(EG(function_table), "curl_init", strlen("curl_init"), (void **)&func) != SUCCESS){
		zend_class_entry *ce_exception;

		ce_exception = zend_fetch_class("Exception", strlen("Exception") - 1, 0 TSRMLS_DC);
		zend_throw_exception(ce_exception, "You need to enable the CURL extension to use the curl transport", 0);
	}
}

/*
	HessianCURLTransport::getMetadata
*/
static PHP_METHOD(HessianCURLTransport, getMetadata)
{
	zval *self, *meta_data;

	self = getThis();
	meta_data = zend_read_property(NULL, self, ZEND_STRL("metaData"), 1 TSRMLS_DC);
	RETURN_ZVAL(meta_data, 1, NULL);
}

/*
	HessianCURLTransport::getStream
*/
static PHP_METHOD(HessianCURLTransport, getStream)
{
	zval *self;
	zval *url, *data, *options, curl_options;
	zval function_name;
	zval *params[2];
	zval *ch;
	zval curl_post, *p_curl_post;
	zval curl_follow_location, *p_curl_follow_location;
	zval curl_return_transfer, *p_curl_return_transfer;
	zval curl_opt_header, *p_curl_opt_header, curl_header_content_type, *p_curl_header_content_type;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &url, &data, &options) == FAILURE) {
		RETURN_FALSE;
	}
	self = getThis();

	/*
		$ch = curl_init($url);
		if(!$ch)
			throw new Exception("curl_init error for url $url.");
		
		$curlOptions = array(
			CURLOPT_URL => $url,
			CURLOPT_POST => 1,
			CURLOPT_POSTFIELDS => $data,
			CURLOPT_FOLLOWLOCATION => true,
			CURLOPT_RETURNTRANSFER => 1,
			CURLOPT_HTTPHEADER => array("Content-Type: application/binary")
		);
	*/

	INIT_STD_ZVAL(function_name);
	ZVAL_STRING(&function_name, "curl_init", 1);
	params[0] = url;
	call_user_function(EG(function_table), NULL, &function_name, ch, 1, params TSRMLS_DC);


	array_init_size(&curl_options, 6);
	zend_hash_add(Z_ARRVAL(curl_options), "10002", 5, &url, sizeof(zval**), NULL); //CURLOPT_URL
	p_curl_post = &curl_post;
	zend_hash_add(Z_ARRVAL(curl_options), "47", 2, &p_curl_post, sizeof(zval**), NULL); //CURLOPT_POST
	zend_hash_add(Z_ARRVAL(curl_options), "10015", 5, &data, sizeof(zval**), NULL); //CURLOPT_POSTFIELDS
	p_curl_follow_location = &curl_follow_location;
	ZVAL_BOOL(&curl_follow_location, 1);
	zend_hash_add(Z_ARRVAL(curl_options), "52", 2, &p_curl_follow_location, sizeof(zval**), NULL); //CURLOPT_FOLLOWLOCATION

	//CURLOPT_RETURNTRANSFER
	p_curl_return_transfer = &curl_return_transfer;
	zend_hash_add(Z_ARRVAL(curl_options), "19913", 5, &p_curl_return_transfer, sizeof(zval**), NULL); 

	//CURLOPT_HTTPHEADER
	array_init_size(&curl_opt_header, 1);
	ZVAL_STRING(&curl_header_content_type, "Content-Type: application/binary", 1);
	p_curl_header_content_type = &curl_header_content_type;
	zend_hash_next_index_insert(Z_ARRVAL(curl_opt_header), &p_curl_header_content_type, sizeof(zval **), NULL);
	zend_hash_add(Z_ARRVAL(curl_options), "10023", 5, &p_curl_opt_header, sizeof(zval**), NULL); 
	
	/*
	//c style function
	CURL *curl = curl_easy_init();
	struct curl_slist *list = NULL;
	if(!curl) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "curl_easy_init error");
		return;
	}

	
	CURLcode res;
	curl_easy_setopt(curl, CURLOPT_URL, Z_STRVAL_P(url));
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_RETURNTRANSFER, 1);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	list = curl_slist_append(list, "Content-Type: application/binary");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

	//CURLOPT_POSTFIELDS => $data
	hessian_curl_transport_set_postfields(data, curl);
	*/
	//dont perform other options
	//transport_options = zend_read_property(NULL, options, ZEND_STRL("transportOptions"), 1 TSRMLS_DC);
	
	//perform
	//res = curl_easy_perform(curl);

	

	/*
	if(!empty($options->transportOptions)){
			$extra = $options->transportOptions;
			if(isset($extra[CURLOPT_HTTPHEADER])){
				$curlOptions[CURLOPT_HTTPHEADER] = array_merge($curlOptions[CURLOPT_HTTPHEADER]
					, $extra[CURLOPT_HTTPHEADER]);
			}
			// array combine operation, does not overwrite existing keys
			$curlOptions = $curlOptions + $options->transportOptions;
		}
		curl_setopt_array($ch, $curlOptions);
	*/

	zval *transport_options;
	zval **find_res;
	transport_options = zend_read_property(NULL, options, ZEND_STRL("transportOptions"), 1 TSRMLS_DC);
	if (Z_TYPE_P(transport_options) == IS_ARRAY){
		if (SUCCESS == zend_hash_find(Z_ARRVAL_P(transport_options), "10023", 5, (void **)find_res)){
			//$curlOptions[CURLOPT_HTTPHEADER] = array_merge($curlOptions[CURLOPT_HTTPHEADER]	, $extra[CURLOPT_HTTPHEADER]);
			php_array_merge(Z_ARRVAL_P(p_curl_opt_header), Z_ARRVAL_P(*find_res), 0 TSRMLS_DC);
		}
		php_array_merge(Z_ARRVAL(curl_options), Z_ARRVAL_P(transport_options), 0 TSRMLS_DC);
	}

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

	zval *result, *curl_info, curl_code_param, *curl_code, *curl_error;
	ZVAL_STRING(&function_name, "curl_exec", 1);
	params[0] = ch;
	call_user_function(EG(function_table), NULL, &function_name, result, 1, params TSRMLS_DC);

	ZVAL_STRING(&function_name, "curl_getinfo", 1);
	params[0] = ch;
	call_user_function(EG(function_table), NULL, &function_name, curl_info, 1, params TSRMLS_DC);
	zend_update_property(NULL, self, ZEND_STRL("metadata"), curl_info TSRMLS_DC);

	ZVAL_STRING(&function_name, "curl_error", 1);
	params[0] = ch;
	call_user_function(EG(function_table), NULL, &function_name, curl_error, 1, params TSRMLS_DC);
	
	params[0] = ch;
	ZVAL_STRING(&curl_code_param, "2097154", 1);
	params[1] = &curl_code_param;
	call_user_function(EG(function_table), NULL, &function_name, curl_code, 2, params TSRMLS_DC);

	if (i_zend_is_true(curl_error)){
		if (Z_TYPE_P(ch) == IS_RESOURCE){
			zend_class_entry *ce_excetpion;

			ce_excetpion = zend_fetch_class("Exception", strlen("Exception") - 1, 0);
			ZVAL_STRING(&function_name, "curl_close", 1);
			params[0] = ch;
			call_user_function(EG(function_table), NULL, &function_name, NULL, 1, params TSRMLS_DC);
			zend_throw_exception(ce_excetpion, sprintf("CURL transport error: %s", Z_STRVAL_P(curl_error)), 0 TSRMLS_DC);
			return;
		}
	}



	/*
		
		if($result === false) {
			$this->safeClose($ch);
			throw new Exception("curl_exec error for url $url");
		}

      */

	if ( (Z_TYPE_P(result) == IS_BOOL) && (0 == Z_BVAL_P(result) < 1)){
		if (Z_TYPE_P(ch) == IS_RESOURCE){
			params[0] = ch;
			call_user_function(EG(function_table), NULL, &function_name, NULL, 1, params TSRMLS_DC);
		}
		zend_class_entry *ce_excetpion;
		ce_excetpion = zend_fetch_class("Exception", strlen("Exception") - 1, 0);
		zend_throw_exception(ce_excetpion, sprintf("curl_exec error for url: %s", Z_STRVAL_P(url)), 0 TSRMLS_DC);
	}

	/*
	if(!empty($options->saveRaw))
			$this->rawData = $result;
		$this->safeClose($ch);
	*/

	zval *save_raw;
	save_raw = zend_read_property(NULL, options, ZEND_STRL("saveRaw"), 1 TSRMLS_DC);
	if (i_zend_is_true(save_raw)){
		zend_update_property(NULL, self, ZEND_STRL("rawData"), result TSRMLS_DC);
		if (Z_TYPE_P(ch) == IS_RESOURCE){
			params[0] = ch;
			call_user_function(EG(function_table), NULL, &function_name, NULL, 1, params TSRMLS_DC);
		}
	}


	/*
	if($code != 200){
			$this->safeClose($ch);
		}
		$stream = new HessianStream($result);
		return $stream;
	*/

	if (Z_LVAL_P(curl_code) != 200){
		if (Z_TYPE_P(ch) == IS_RESOURCE){
			params[0] = ch;
			call_user_function(EG(function_table), NULL, &function_name, NULL, 1, params TSRMLS_DC);
		}
		/*
		$msg = "Server error, returned HTTP code: $code";
			if(!empty($options->saveRaw))
				$msg .= " Server sent: ".$result;
			throw new Exception($msg);
		*/
		zval message, *p_message;
		char str_message[800];

		//@todo:length is ok?
		sprintf(str_message, "Server error, returned HTTP code: %d Server sent:%s"
			, Z_LVAL_P(curl_code), Z_STRVAL_P(result));
		ZVAL_STRING(&message, str_message, 1);
		zend_class_entry *ce_excetpion;
		ce_excetpion = zend_fetch_class("Exception", strlen("Exception") - 1, 0);
		zend_throw_exception(ce_excetpion, Z_STRVAL(message), 0 TSRMLS_DC);
	}

	zval *stream;
	object_init_ex(stream, hessian_stream_entry);
	RETURN_ZVAL(stream, 0, NULL);
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
	PHP_ABSTRACT_ME(HessianCURLTransport, getStream, arginfo_ihessian_transport_get_stream)
	PHP_ABSTRACT_ME(HessianCURLTransport, testAvailable, arginfo_ihessian_transport_test_available)
	PHP_ABSTRACT_ME(HessianCURLTransport, getMetadata, arginfo_ihessian_transport_get_metadata)
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
