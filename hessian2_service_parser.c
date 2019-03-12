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


//params
ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_parser_detect_version, 0, 0, 1)
	ZEND_ARG_INFO(0, stream) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_parser_parse_top, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_parser_parse_reply, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_parser_parse_call, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_parser_parse_fault, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_parser_parse_envelope, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_parser_parse_packet, 0, 0, 0)
ZEND_END_ARG_INFO()



//entry
zend_class_entry *hessian2_service_parser_entry;



/*
	Hessian2ServiceParser::detectVersion
*/
static PHP_METHOD(Hessian2ServiceParser, detectVersion)
{
	zval  *stream;
	zval *version;
	zval function_name, param1, param2;
	zval* params[2];
	char *buf;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &stream)) {
		return;
	}
	if (Z_TYPE_P(stream) != IS_OBJECT){
		php_error_docref(NULL, E_WARNING, "stream must be a stream");
		return;
	}

	/**
		$version = $stream->peek(3, 0);
		return $version == "H\x02\x00";
	*/
	Z_LVAL(param1) = 3;
	Z_TYPE(param1) = IS_LONG;
	Z_LVAL(param1) = 0;
	Z_TYPE(param1) = IS_LONG;
	ZVAL_STRING(&function_name, "peek", 1);
	params[0]= &param1;
	params[1] = &param2;

	call_user_function(NULL, &stream, &function_name, version, 2, params TSRMLS_DC);
	if (Z_TYPE_P(version) != IS_STRING){
		RETURN_FALSE;
	}
	if (Z_STRLEN_P(version) < 3){
		RETURN_FALSE;
	}
	buf = Z_STRVAL_P(version);

	if ((buf[0] == 'H') && (buf[1] == 2) && (buf[2] == 0)){
		RETURN_TRUE;
	}else{
		RETURN_FALSE;
	}
}


/*
	Hessian2ServiceParser::parseTop
*/
static PHP_METHOD(Hessian2ServiceParser, parseTop)
{
	zval  *self;
	zval code, *value;
	zval function_name, param_msg; 
	zval* params[2];
	
	self = getThis();

	/*
		$this->logMsg('Parsing top element');
		$this->parseVersion();
		$code = $this->read();
		$value = null;
	*/

	ZVAL_STRING(&param_msg, "'Parsing top element'", 1);
	hessian2_parser_log_msg(self, &param_msg);

	//parseVersion
	hessian2_parser_read(self, 3, NULL);
	hessian2_parser_read(self, 1, &code);
	Z_TYPE_P(value) = IS_NULL;
	
	/*
		switch($code){
			case 'R':
				$value = $this->parseReply();
				break;
			case 'C':
				$value = $this->parseCall();
				break;
			case 'F':
				$value = $this->parseFault();
				break;
			case 'E':
				$value = $this->parseEnvelope();
				break;
			default:
				throw new HessianFault("Code $code not recognized as a top element");
		}
		return $value;
	*/

	if(Z_TYPE(code) !=  IS_STRING){
		php_error_docref(NULL, E_WARNING, "code not a string");
		return;
	}

	if (Z_STRLEN(code) < 1){
		zend_throw_exception(hessian_fault_entry, "code is empty", 0 TSRMLS_DC);
		return;
	}
	
	char msg_buf[100];
	switch(Z_STRVAL(code)[0]){
		case 'R':
			ZVAL_STRING(&function_name, "parseReply",  1);
			call_user_function(NULL, &self, &function_name, value, 0, params TSRMLS_DC);
			break;
		case 'C':
			ZVAL_STRING(&function_name, "parseCall",  1);
			call_user_function(NULL, &self, &function_name, value, 0, params TSRMLS_DC);
			break;
		case 'F':
			ZVAL_STRING(&function_name, "parseFault",  1);
			call_user_function(NULL, &self, &function_name, value, 0, params TSRMLS_DC);
			break;
		case 'E':
			ZVAL_STRING(&function_name, "parseEnvelope",  1);
			call_user_function(NULL, &self, &function_name, value, 0, params TSRMLS_DC);
			break;
		default:
			sprintf(msg_buf, "Code %s not recognized as a top element", Z_STRVAL(code)[0]);
			zend_throw_exception(hessian_fault_entry, msg_buf, 0 TSRMLS_DC);
	}
}

/*
	Hessian2ServiceParser::parseReply
*/
static PHP_METHOD(Hessian2ServiceParser, parseReply)
{
	zval *self, *res;
	zval param_msg, function_name;
	zval *params[1];

	self = getThis();

	/*
		$this->logMsg('Parsing reply');
		return $this->parse(); 
	*/

	ZVAL_STRING(&param_msg, "Parsing reply", 1);
	hessian2_parser_log_msg(self, &param_msg);

	ZVAL_STRING(&function_name, "parse", 1);
	call_user_function(NULL, &self, &function_name, res, 0, params TSRMLS_DC);

	RETURN_ZVAL(res, 1,  NULL);
}

/*
	Hessian2ServiceParser::parseCall
*/
static PHP_METHOD(Hessian2ServiceParser, parseCall)
{
	zval *self, call, *call_method, *num;
	zval *arguments;
	zval param_msg, function_name;
	zval param1, param2;
	zval *params[1];
	int i;

	self = getThis();

	/*
		$this->logMsg('Parsing call');
		$call = new HessianCall();
		$call->method = $this->parse(null, 'string');
	*/

	ZVAL_STRING(&param_msg, "Parsing call", 1);
	hessian2_parser_log_msg(self, &param_msg);

	object_init_ex(&call, hessian_call_entry);
	

	//$call->method = $this->parse(null, 'string');
	Z_TYPE(param1) = IS_NULL;
	ZVAL_STRING(&param2, "string", 1);

	ZVAL_STRING(&function_name, "parse", 1);
	params[0] = &param1;
	params[1] = &param2;
	call_user_function(NULL, &self, &function_name, call_method, 2, params TSRMLS_DC);
	zend_update_property(NULL, &call, ZEND_STRL("method"), call_method TSRMLS_DC);
	
	//$num = $this->parse(null, 'integer');
	ZVAL_STRING(&param2, "integer", 1);
	call_user_function(NULL, &self, &function_name, num, 2, params TSRMLS_DC);


	/*
		for($i=0;$i<$num;$i++){
			$call->arguments[] = $this->parseCheck();
		}
		return $call;
	*/

	if (Z_TYPE_P(num) != IS_LONG){
		php_error_docref(NULL, E_WARNING, "parse get num must be a number");
		return;
	}

	arguments = zend_read_property(NULL, &call, ZEND_STRL("arguments"), 1 TSRMLS_DC);
	if (Z_TYPE_P(arguments) != IS_ARRAY){
		array_init_size(arguments, 4);
	}

	zval *arg;
	ZVAL_STRING(&function_name, "parseCheck", 1);
	for(i=0; i<Z_LVAL_P(num); i++){
		call_user_function(NULL, &self, &function_name, arg, 0, params TSRMLS_DC);
		zend_hash_next_index_insert(Z_ARRVAL_P(arguments), &arg, sizeof(zval **), NULL);
	}

	zend_update_property(NULL, &call, ZEND_STRL("arguments"), arguments TSRMLS_DC);

	RETURN_ZVAL(&call, 1, NULL);
}


/*
	Hessian2ServiceParser::parseFault
*/
static PHP_METHOD(Hessian2ServiceParser, parseFault)
{
	zval *self, *map, fault;
	zval *p_fault;
	zval *message, *code;
	zval param1, param2, function_name;
	zval *params[3];

	self = getThis();

	/*
			$this->logMsg('Parsing fault');
		$map = $this->parse(null, 'map');
		throw $fault;
	*/

	ZVAL_STRING(&param1, "Parsing fault", 1);
	hessian2_parser_log_msg(self, &param1);

	ZVAL_STRING(&function_name, "parse", 1);
	ZVAL_STRING(&param1, "map", 1);
	Z_TYPE(param2) = IS_NULL;
	call_user_function(NULL, &self, &function_name, map, 2, params TSRMLS_DC);

	//$fault = new HessianFault($map['message'], $map['code'], $map);
	zend_hash_find(Z_ARRVAL_P(map), "message", 7, (void **)&message);
	zend_hash_find(Z_ARRVAL_P(map), "code", 7, (void **)&code);

	object_init_ex(&fault, hessian_fault_entry);
	ZVAL_STRING(&function_name, "__construct", 1);
	params[0] = message;
	params[1] = code;
	params[2] = map;

	p_fault = &fault;
	call_user_function(NULL, &p_fault, &function_name, map, 3, params TSRMLS_DC);
	
	RETURN_ZVAL(&fault, 1, NULL);
}


/*
	Hessian2ServiceParser::parseEnvelope
*/
static PHP_METHOD(Hessian2ServiceParser, parseEnvelope)
{
	zend_class_entry *ce_exception;
	
	ce_exception = zend_fetch_class("Exception", strlen("Exception") - 1, 0 TSRMLS_DC);
	zend_throw_exception(ce_exception, "Envelopes currently not supported", 0);
}

/*
	Hessian2ServiceParser::parseEnvelope
*/
static PHP_METHOD(Hessian2ServiceParser, parsePacket)
{
	zend_class_entry *ce_exception;
	
	ce_exception = zend_fetch_class("Exception", strlen("Exception") - 1, 0 TSRMLS_DC);
	zend_throw_exception(ce_exception, "Packetc currently not supported", 0);
}



//Hessian2ServiceParser functions
const zend_function_entry hessian2_service_parser_functions[] = {
	PHP_ME(Hessian2ServiceParser, detectVersion, arginfo_hessian2_parser_detect_version, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2ServiceParser, parseTop, arginfo_hessian2_parser_parse_top, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2ServiceParser, parseReply, arginfo_hessian2_parser_parse_top, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2ServiceParser, parseCall, arginfo_hessian2_parser_parse_call, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2ServiceParser, parseFault, arginfo_hessian2_parser_parse_fault, ZEND_ACC_PUBLIC)
	//PHP_ME(Hessian2ServiceParser, parseFault, arginfo_hessian2_parser_parse_fault, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2ServiceParser, parseEnvelope, arginfo_hessian2_parser_parse_envelope, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2ServiceParser, parsePacket, arginfo_hessian2_parser_parse_packet, ZEND_ACC_PUBLIC)
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
