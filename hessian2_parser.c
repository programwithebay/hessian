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
ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_parser_construct, 0, 0, 3)
	ZEND_ARG_INFO(0, resolver) /* string */
	ZEND_ARG_INFO(0, stream) /* string */
	ZEND_ARG_INFO(0, options) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_parser_set_stream, 0, 0, 1)
	ZEND_ARG_INFO(0, stream) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_parser_set_type_map, 0, 0, 1)
	ZEND_ARG_INFO(0, type_map) /* string */
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_parser_set_filters, 0, 0, 1)
	ZEND_ARG_INFO(0, container) /* string */
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_parser_log_msg, 0, 0, 1)
	ZEND_ARG_INFO(0, msg) /* string */
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_parser_read, 0, 0, 1)
	ZEND_ARG_INFO(0, count) /* string */
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_read_num, 0, 0, 1)
	ZEND_ARG_INFO(0, count) /* string */
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_parse_check, 0, 0, 1)
	ZEND_ARG_INFO(0, code) /* string */
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_parse, 0, 0, 2)
	ZEND_ARG_INFO(0, code) /* string */
	ZEND_ARG_INFO(0, expect) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_binary0, 0, 0, 2)
	ZEND_ARG_INFO(0, code) /* string */
	ZEND_ARG_INFO(0, num) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_binary1, 0, 0, 2)
	ZEND_ARG_INFO(0, code) /* string */
	ZEND_ARG_INFO(0, num) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_binary_long_data, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_binary_long, 0, 0, 2)
	ZEND_ARG_INFO(0, code) /* string */
	ZEND_ARG_INFO(0, num) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_compact_int1, 0, 0, 2)
	ZEND_ARG_INFO(0, code) /* string */
	ZEND_ARG_INFO(0, num) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_read_utf8_bytes, 0, 0, 1)
	ZEND_ARG_INFO(0, len) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_parse_type, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_fill_map, 0, 0, 1)
	ZEND_ARG_INFO(0, index) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_reference, 0, 0, 0)
ZEND_END_ARG_INFO()





//entry
zend_class_entry *hessian2_parser_entry;



/*
	Hessian2Parser::read
*/
void hessian2_parser_read(zval *self, ulong count, zval *return_value)
{
	zval *params[1];
	zval function_name;
	zval z_count;
	zval *stream;
	
	//return $this->stream->read($count);
	stream = zend_read_property(NULL, self, ZEND_STRL("stream"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "read", 1);
	ZVAL_LONG(&z_count, count);
	params[0] = &z_count;

	call_user_function(NULL, &stream, &function_name, return_value, 1, params TSRMLS_DC);
}



/*
	readUTF8Bytes
*/
void hessian2_parser_read_utf8_bytes(zval *self, ulong len, zval *return_value)
{
	zval *read_str;
	int pos, pass;
	char *string;
	zend_uchar char_code;

	/*
	 $pos    = 0;
        $pass   = 1;
        while ($pass <= $len) {
            $charCode = ord($string[$pos]);
            if ($charCode < 0x80) {
                $pos++;
            } elseif (($charCode & 0xe0) == 0xc0) {
                $pos    += 2;
                $string .= $this->read(1);
            } elseif (($charCode & 0xf0) == 0xe0) {
                $pos    += 3;
                $string .= $this->read(2);
            }
            $pass++;
        }
     */
    string = Z_STRVAL_P(read_str);
	pos = 0;
	pass = 1;

	while(pass <= len){
		char_code = string[pos];
		if (char_code < 0x80){
			++pos;
		}else if ((char_code & 0xe0) == 0xc0){
			pos += 2;
			// $string .= $this->read(1);
			hessian2_parser_read(self, 1, read_str);
			string = strcat(string, Z_STRVAL_P(read_str));
		}else if ((char_code & 0xf0) == 0xe0){
			pos += 3;
			// $string .= $this->read(2);
			hessian2_parser_read(self, 2, read_str); 
			string = strcat(string, Z_STRVAL_P(read_str));
		}
		++pass;
	}

	ZVAL_STRING(return_value, string, 1);
}

/*
	Hessian2Parser::stringLongData
*/
void hessian2_parser_string_long_data(zval *self, zval *return_value)
{
	ulong code, num, len;
	zval *read_str, *tmp_len;
	zval function_name, param1;
	zval *params[2];
	zval *z_len;
	zval string;

	/*
		$tempLen = unpack('n', $this->read(2));
        $len     = $tempLen[1];
        return $this->readUTF8Bytes($len);
	*/ 

	hessian2_parser_read(self, 2, read_str);
	ZVAL_STRING(&function_name, "unpack", 1);
	ZVAL_STRING(&param1, "n", 1);
	params[0] = &param1;
	params[1] = read_str;

	call_user_function(EG(function_table), NULL, &function_name, tmp_len, 2, params TSRMLS_DC);
	if (SUCCESS != zend_hash_find(Z_ARRVAL_P(tmp_len), "1", 1, (void **)&z_len)){
		RETURN_FALSE;
	}

	len = Z_LVAL_P(tmp_len);
	hessian2_parser_read_utf8_bytes(self, len, &string);

	//estrndup

	Z_TYPE_P(return_value) = IS_STRING;
	Z_STRVAL_P(return_value) = estrndup(Z_STRVAL(string), Z_STRLEN(string));
	Z_STRLEN_P(return_value) = Z_STRLEN(string);
}

//log msg
void hessian2_parser_log_msg(zval *self, zval *msg){
	zval *log;
	
	log = zend_read_property(NULL, self, ZEND_STRL("log"), 1 TSRMLS_DC);
	if (Z_TYPE_P(log) != IS_ARRAY){
		array_init_size(log, 4);
	}
	zend_hash_next_index_insert(Z_ARRVAL_P(log), &msg, sizeof(zval**), NULL);
	zend_update_property(NULL, self, ZEND_STRL("log"), log TSRMLS_DC);

}

/*
	Hessian2Parser::__construct
*/
static PHP_METHOD(Hessian2Parser, __construct)
{
	zval *resolver, *stream, *options;
	zval *self;
	zval *ref_map, *type_map;

	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &resolver, &stream, &options)) {
		return;
	}
	self = getThis();

	/*
		$this->resolver = $resolver;
        $this->refmap   = new HessianReferenceMap();
        $this->typemap  = new HessianTypeMap();
        $this->stream   = $stream;
        $this->options  = $options;
     */

	zend_update_property(NULL, self, ZEND_STRL("resolver"), resolver TSRMLS_DC);
	ALLOC_ZVAL(ref_map);
	ALLOC_ZVAL(type_map);

	object_init_ex(ref_map, hessian_reference_map_entry);
	object_init_ex(type_map, hessian_type_map_entry);

	zend_update_property(NULL, self, ZEND_STRL("stream"), stream TSRMLS_DC);
	zend_update_property(NULL, self, ZEND_STRL("options"), options TSRMLS_DC);
}


/*
	Hessian2Parser::setStream
*/
static PHP_METHOD(Hessian2Parser, setStream)
{
	zval *stream;
	zval *self;

	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &stream)) {
		return;
	}
	self = getThis();

	zend_update_property(NULL, self, ZEND_STRL("stream"), stream TSRMLS_DC);
}


/*
	Hessian2Parser::setTypeMap
*/
static PHP_METHOD(Hessian2Parser, setTypeMap)
{
	zval *type_map;
	zval *self;

	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &type_map)) {
		return;
	}
	self = getThis();

	zend_update_property(NULL, self, ZEND_STRL("typemap"), type_map TSRMLS_DC);
}


/*
	Hessian2Parser::setFilters
*/
static PHP_METHOD(Hessian2Parser, setFilters)
{
	zval *container;
	zval *self;

	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &container)) {
		return;
	}
	self = getThis();

	zend_update_property(NULL, self, ZEND_STRL("filterContainer"), container TSRMLS_DC);
}

/*
	Hessian2Parser::logMsg
*/
static PHP_METHOD(Hessian2Parser, logMsg)
{
	zval *msg;
	zval *self;
	zval *log;

	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &msg)) {
		return;
	}
	self = getThis();
	hessian2_parser_log_msg(self, msg);
}

/*
	Hessian2Parser::read
*/
static PHP_METHOD(Hessian2Parser, read)
{
	zval *count;
	zval *self;
	zval *stream;
	zval function_name;
	zval *params[1];
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &count)) {
		return;
	}
	self = getThis();
	if (Z_TYPE_P(count) != IS_LONG){
		php_error_docref(NULL, E_WARNING, "count muse be an long");
		return;
	}

	//return $this->stream->read($count);
	hessian2_parser_read(self, Z_LVAL_P(count), return_value);
}

/*
	Hessian2Parser::readNum
*/
static PHP_METHOD(Hessian2Parser, readNum)
{
	zval *count;
	zval *self;
	zval *stream;
	zval function_name;
	zval *params[1];
	zval *ret_val;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &count)) {
		return;
	}
	self = getThis();
	if (Z_TYPE_P(count) != IS_LONG){
		php_error_docref(NULL, E_WARNING, "count muse be an long");
		return;
	}

	//return $this->stream->read($count);
	stream = zend_read_property(NULL, self, ZEND_STRL("stream"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "read", 1);
	params[0] = count;

	call_user_function(NULL, &stream, &function_name, ret_val, 1, params TSRMLS_DC);
	if (Z_TYPE_P(ret_val) != IS_STRING){
		RETURN_FALSE;
	}
	if (!Z_STRVAL_P(ret_val)){
		RETURN_FALSE;
	}
	RETURN_LONG(Z_STRVAL_P(ret_val)[0]);
}



/*
	Hessian2Parser::parseCheck
*/
static PHP_METHOD(Hessian2Parser, parseCheck)
{
	zval *code;
	zval *self;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &code)) {
		return;
	}
	self = getThis();

	/*
	 $value = $this->parse($code);
        if (!HessianRef::isRef($value))
            return $value;
        return $this->refmap->objectlist[$value->index];
        */

	

}


/*
	Hessian2Parser::parse
*/
static PHP_METHOD(Hessian2Parser, parse)
{
	zval *code, *expect;
	zval *self;
	zend_bool end = 1;
	zval function_name;
	zval *params[2];
	zval *resolver, *rule, *fun;
	zend_uchar num;
	zval z_num;
	zval *value;
				

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &code, &expect)) {
		return;
	}
	if (Z_TYPE_P(code) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "code must be string");
		return;
	}
	self = getThis();

	if (!i_zend_is_true(code)){
		hessian2_parser_read(self, 1, code);
	}else{
		// $rule = $this->resolver->resolveSymbol($code, $expect);

		do{
			
			
			resolver = zend_read_property(NULL, self, ZEND_STRL("resolver"), 1 TSRMLS_DC);
			ZVAL_STRING(&function_name, "resolveSymbol", 1);	
			params[0] = code;
			params[1] = expect;

			call_user_function(NULL, &resolver,&function_name, rule, 2, params TSRMLS_DC);
			fun = zend_read_property(NULL, rule, ZEND_STRL("func"), 1 TSRMLS_DC);

			/*
			 $num  = ord($code);
	            	$this->logMsg("llamando $fun con code $code y num $num hex 0x" . dechex($num) . " offset " . $this->stream->pos);
	            	$value = $this->$fun($code, $num);
	            */

			num = Z_STRVAL_P(code)[0];
			ZVAL_LONG(&z_num, num);
			params[0] = code;
			params[1] = &z_num;
			ZVAL_STRING(&function_name, Z_STRVAL_P(fun), 1);
			call_user_function(NULL, &self, &function_name, value, 2, params TSRMLS_DC);

			/*
			if ($value instanceof HessianIgnoreCode) {
                		$end  = false;
               		 $code = $this->read();
           		 } else $end = true;
            		*/
            if (instanceof_function(Z_OBJCE_P(value), ihessian_ignore_code_entry TSRMLS_DC)){
				end = 0 ;
				hessian2_parser_read(self, 1, code);
            }else{
            	end = 1;
            }
		}while(end);
	}

	/*
	 	$filter = $this->filterContainer->getCallback($rule->type);
       	 if ($filter)
            		$value = $this->filterContainer->doCallback($filter, array($value, $this));
        	
            if ($filter)
                $value = $this->filterContainer->doCallback($filter, array($value, $this));
        	}
        	return $value;
        */

	zval *filter, *filter_container, *rule_type;
	filter_container = zend_read_property(NULL, self, ZEND_STRL("filterContainer"), 1 TSRMLS_DC);
	rule_type = zend_read_property(NULL, rule, ZEND_STRL("type"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "getCallback", 1);
	params[0] = rule_type;
	call_user_function(NULL, &filter_container, &function_name, filter, 1, params TSRMLS_DC);

	if (i_zend_is_true(filter)){
		zval  array;
		array_init_size(&array, 2);
		zend_hash_next_index_insert(Z_ARRVAL(array), &value, sizeof(zval **), NULL);
		zend_hash_next_index_insert(Z_ARRVAL(array), &self, sizeof(zval **), NULL);
		ZVAL_STRING(&function_name, "doCallback", 1);
		params[0] = filter;
		params[1] = &array;
		call_user_function(NULL, &filter_container, &function_name, value, 2, params TSRMLS_DC);
	}

	/*
		if (is_object($value)) {
            		$filter = $this->filterContainer->getCallback($value);
        */
     if (Z_TYPE_P(value) == IS_OBJECT){
		ZVAL_STRING(&function_name, "getCallback", 1);
		params[0] = value;
		call_user_function(NULL, &filter_container, &function_name, filter, 1, params TSRMLS_DC);
     }


	/*
	 	if ($filter)
                $value = $this->filterContainer->doCallback($filter, array($value, $this));
        */
	 if (i_zend_is_true(filter)){
		zval  array;
		array_init_size(&array, 2);
		zend_hash_next_index_insert(Z_ARRVAL(array), &value, sizeof(zval **), NULL);
		zend_hash_next_index_insert(Z_ARRVAL(array), &self, sizeof(zval **), NULL);
		ZVAL_STRING(&function_name, "doCallback", 1);
		params[0] = value;
		params[1] = &array;
		call_user_function(NULL, &filter_container, &function_name, value, 2, params TSRMLS_DC);
	}

	 RETURN_ZVAL(value, 1, NULL);
}



/*
	Hessian2Parser::binary0
*/
static PHP_METHOD(Hessian2Parser, binary0)
{
	ulong code, num, len;
	zval *self;
				

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();

	len = num = 32;
	hessian2_parser_read(self, len, return_value);
}



/*
	Hessian2Parser::binary1
*/
static PHP_METHOD(Hessian2Parser, binary1)
{
	ulong code, num, len;
	zval *self;
	zval *read_str;
				

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();


	/*
		 $len = (($num - 0x34) << 8) + ord($this->read());
        	return $this->read($len);
        */
    hessian2_parser_read(self, 1, read_str);
	len = (num -52)<< 8 + Z_STRVAL_P(read_str)[0];
	hessian2_parser_read(self, len, return_value);
}


/*
	Hessian2Parser::binaryLongData
*/
static PHP_METHOD(Hessian2Parser, binaryLongData)
{
	ulong code, num, len;
	zval *self;
	zval function_name;
	zval *params[2];
	zval param1;
	zval *read_str;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();


	/*
		 $tempLen = unpack('n', $this->read(2));
       	 $len     = $tempLen[1];
       	 return $this->read($len);
        */
    ZVAL_STRING(&function_name, "unpack", 1);
    ZVAL_STRING(&param1, "n", 1);
	hessian2_parser_read(self, 2, read_str);
	len = Z_STRVAL_P(read_str)[1];
	hessian2_parser_read(self, len, return_value);
}

/*
	Hessian2Parser::binaryLong
*/
static PHP_METHOD(Hessian2Parser, binaryLong)
{
	ulong num, len;
	zend_bool final=0;
	zval *self, *z_code;
	zval function_name;
	zval *params[2];
	zval *res;
	char *data;
	zval param1;
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zl", &z_code, &num)) {
		return;
	}
	
	self = getThis();
	ZVAL_STRING(&param1, "binary", 1);


	/*
		 $tempLen = unpack('n', $this->read(2));
       	 $len     = $tempLen[1];
       	 return $this->read($len);
        */

	do{
		final = num != 0x41;
		/*
		 if ($num == 0x41 || $num == 0x42)
                $data .= $this->binaryLongData();
            else
                $data .= $this->parse($code, 'binary');
                */
       if ((num == 0x41) || (num == 0x42)){
	   		ZVAL_STRING(&function_name, "binaryLongData", 1);
			call_user_function(NULL, &self, &function_name, res, 0, params TSRMLS_DC);
			if (!data){
				data = Z_STRVAL_P(res);
			}else{
				data = strcat(data, Z_STRVAL_P(res));
			}
       }else{
       		ZVAL_STRING(&function_name, "parse", 1);
			params[0] = z_code;
			params[1] = &param1;
			call_user_function(NULL, &self, &function_name, res, 0, params TSRMLS_DC);
			if (!data){
				data = Z_STRVAL_P(res);
			}else{
				data = strcat(data, Z_STRVAL_P(res));
			}
       }
	}while(!final);

	RETURN_STRING(data, 0);
}


/*
	Hessian2Parser::compactInt1
*/
static PHP_METHOD(Hessian2Parser, compactInt1)
{
	ulong code, num, len;
	zval *self;


	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();


		/*
		  if ($code == 0x90)
            		return 0;
        	return ord($code) - 0x90;
        */

		if (code == 0x90){
			RETURN_LONG(0);
		}
		RETURN_LONG(code - 0x90);
}



/*
	Hessian2Parser::compactInt2
*/
static PHP_METHOD(Hessian2Parser, compactInt2)
{
	ulong code, num, len;
	zval *self, *read_str;


	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();


		/*
		 $b0 = ord($this->read());
        	return ((ord($code) - 0xc8) << 8) + $b0;
        */

	hessian2_parser_read(self, 1, read_str);
	RETURN_LONG((code-0xc8)<<8 + Z_STRVAL_P(read_str)[0]);
}


/*
	Hessian2Parser::compactInt3
*/
static PHP_METHOD(Hessian2Parser, compactInt3)
{
	ulong code, num, res;
	zval *self, *b1, *b0;


	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();


	/*
		 $b1 = ord($this->read());
        	$b0 = ord($this->read());
        	return ((ord($code) - 0xd4) << 16) + ($b1 << 8) + $b0;
        */

	hessian2_parser_read(self, 1, b1);
	hessian2_parser_read(self, 1, b0);
	
	res = (code - 0xd4) << 16 + (Z_STRVAL_P(b1)[0] << 8) + Z_STRVAL_P(b0)[0];

	RETURN_LONG(res);
}


/*
	Hessian2Parser::parseInt
*/
static PHP_METHOD(Hessian2Parser, parseInt)
{
	ulong code, num;
	zval *self, *read_str, *data, *res;
	zval function_name, param1;
	zval *params[2];


	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();


	/*
		 $data = unpack('N', $this->read(4));
        	return $data[1];
        */

	ZVAL_STRING(&function_name, "unpack", 1);
	ZVAL_STRING(&param1, "N", 1);
	hessian2_parser_read(self, 4, read_str);
	params[0] = &param1;
	params[1] = read_str;
	call_user_function(EG(function_table), NULL, &function_name, data, 2, params TSRMLS_DC);
	if (Z_TYPE_P(data) != IS_ARRAY){
		RETURN_FALSE;
	}
	//@todo check is array
	zend_hash_find(Z_ARRVAL_P(data), "1", 1, (void **)&res);

	if (res){
		RETURN_ZVAL(res, 1, NULL);
	}
	RETURN_FALSE;
}

/*
	Hessian2Parser::bool
*/
static PHP_METHOD(Hessian2Parser, bool)
{
	ulong code, num;
	zval *self;


	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();


	RETURN_BOOL(code == 'T');
}


/*
	Hessian2Parser::date
*/
static PHP_METHOD(Hessian2Parser, date)
{
	ulong code, num;
	zval *self, *read_str, *ts;
	zval function_name;
	zval *params[1];

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	/*
		 $ts = HessianUtils::timestampFromBytes64($this->read(8));
        	return $ts;
	*/
	hessian2_parser_read(self, 8, read_str);
	ZVAL_STRING(&function_name, "HessianUtils::timestampFromBytes64", 1);
	params[0] = read_str;
	call_user_function(EG(function_table), NULL, &function_name, ts, 1, params TSRMLS_DC);

	RETURN_ZVAL(ts, 1, NULL);
}


/*
	Hessian2Parser::compactDate
*/
static PHP_METHOD(Hessian2Parser, compactDate)
{
	ulong code, num, ts;
	zval *self, *read_str, *data;
	zval *data1, *data2, *data3, *data4;
	zval function_name, param1;
	zval *params[2];

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	/*
		 $data = unpack('C4', $this->read(4));
        	$num  = ($data[1] << 24) +
            		($data[2] << 16) +
            		($data[3] << 8) +
            		$data[4];
        	$ts   = $num * 60;
        	return $ts;
	*/
	hessian2_parser_read(self, 4, read_str);
	ZVAL_STRING(&function_name, "unpack", 1);
	ZVAL_STRING(&param1, "C4", 1);
	params[0] = &param1;
	params[1] = read_str;
	call_user_function(EG(function_table), NULL, &function_name, data, 1, params TSRMLS_DC);


	zend_hash_find(Z_ARRVAL_P(data), "1", 1, (void **)&data1);
	zend_hash_find(Z_ARRVAL_P(data), "2", 1, (void **)&data2);
	zend_hash_find(Z_ARRVAL_P(data), "3", 1, (void **)&data3);
	zend_hash_find(Z_ARRVAL_P(data), "4", 1, (void **)&data4);

	ts = Z_STRVAL_P(data1)[0] << 24 + Z_STRVAL_P(data1)[1] << 16 + Z_STRVAL_P(data1)[2] << 8 + Z_STRVAL_P(data1)[3]; 
	ts *= 60;

	RETURN_LONG(ts);
}


/*
	Hessian2Parser::double1
*/
static PHP_METHOD(Hessian2Parser, double1)
{
	ulong code, num;
	zval *self, *read_str ; 

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	/*
		if ($num == 0x5b)
            return (float)0;
        if ($num == 0x5c)
            return (float)1.0;
        $bytes = $this->read(1);
        return (float)ord($bytes);
	*/
	if (num == 0x5b){
		RETURN_DOUBLE(0);
	}
	if (num == 0x5C){
		RETURN_DOUBLE(1.0);
	}
	hessian2_parser_read(self, 1, read_str);
 

	RETURN_DOUBLE(Z_STRVAL_P(read_str)[0]);
}



/*
	Hessian2Parser::double2
*/
static PHP_METHOD(Hessian2Parser, double2)
{
	ulong code, num;
	zval *self, *read_str, *b, *data;
	zval function_name, param1;
	zval *params[2];

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	/*
		 $bytes = $this->read(2);
        $b     = unpack('s', strrev($bytes));
        return (float)$b[1];
	*/ 
	hessian2_parser_read(self, 2, read_str);
 	ZVAL_STRING(&function_name, "strrev", 1); 
	params[0] = read_str; 
	call_user_function(EG(function_table), NULL, &function_name, read_str, 1, params TSRMLS_DC);

	ZVAL_STRING(&function_name, "unpack", 1); 
	ZVAL_STRING(&param1, "s", 1);
	params[0] = &param1;
	params[1] = read_str;
	call_user_function(EG(function_table), NULL, &function_name, b, 1, params TSRMLS_DC);


	zend_hash_find(Z_ARRVAL_P(b), "1", 1, (void **)&data);
	
	RETURN_DOUBLE(Z_STRVAL_P(data)[0]);
}


/*
	Hessian2Parser::double4
*/
static PHP_METHOD(Hessian2Parser, double4)
{
	ulong code, num;
	zval *self, *read_str, *b;


	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	/*
		 $b   = $this->read(4);
        $num = (ord($b[0]) << 24) +
            (ord($b[1]) << 16) +
            (ord($b[2]) << 8) +
            ord($b[3]);
        return 0.001 * $num;
	*/ 
	hessian2_parser_read(self, 4, read_str);
	num = Z_STRVAL_P(read_str)[0] << 24 + Z_STRVAL_P(read_str)[1] << 16 + Z_STRVAL_P(read_str)[2] << 8 + Z_STRVAL_P(read_str)[3];

	RETURN_DOUBLE(0.001 * num);
}

/*
	Hessian2Parser::double64
*/
static PHP_METHOD(Hessian2Parser, double64)
{
	ulong code, num;
	zval *self, *read_str;
	zval *little_endian, *z_double, *res;;
	zval function_name, param1;
	zval *params[2];	

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	/*
		 $bytes = $this->read(8);
        if (HessianUtils::$littleEndian)
            $bytes = strrev($bytes);
        //$double = unpack("dflt", strrev($bytes));
        $double = unpack("dflt", $bytes);
        return $double['flt'];
	*/ 
	hessian2_parser_read(self, 8, read_str);
	little_endian = zend_read_property(hessian_utils_entry, NULL, ZEND_STRL("littleEndian"), 1 TSRMLS_DC);
	if (i_zend_is_true(little_endian)){
		

		ZVAL_STRING(&function_name, "strrev", 1);
		params[0] = read_str;

		call_user_function(EG(function_table), NULL, &function_name, read_str, 1, params TSRMLS_DC);
	}

	ZVAL_STRING(&function_name, "unpack", 1);
	ZVAL_STRING(&param1, "dflt", 1);
	params[0] = &param1;
	params[1] = read_str;
	call_user_function(EG(function_table), NULL, &function_name, z_double, 2, params TSRMLS_DC);

	zend_hash_find(Z_ARRVAL_P(read_str), "flt", 3, (void **)&res);
	RETURN_ZVAL(res, 0, NULL);
}


/*
	Hessian2Parser::long1
*/
static PHP_METHOD(Hessian2Parser, long1)
{
	ulong code, num;
	zval *self;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	/*
		  if ($code == 0xe0)
            return 0;
        return $num - 0xe0;
	*/ 

	if (0xe0 == code){
		RETURN_LONG(0);
	}
	RETURN_LONG(num - 0xe0);
}


/*
	Hessian2Parser::long2
*/
static PHP_METHOD(Hessian2Parser, long2)
{
	ulong code, num, res;
	zval *self, *read_str;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	/*
		 return (($num - 0xf8) << 8) + $this->readNum();
	*/ 

	
	hessian2_parser_read(self, 1, read_str);
	res = ((num - 0xf8) << 8) + Z_STRVAL_P(read_str)[0];

	RETURN_LONG(res);
}

/*
	Hessian2Parser::long3
*/
static PHP_METHOD(Hessian2Parser, long3)
{
	ulong code, num, res;
	zval *self, *read1, *read2;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	/*
		 return ((ord($code) - 0x3c) << 16)
            + ($this->readNum() << 8)
            + $this->readNum();
	*/ 

	
	hessian2_parser_read(self, 1, read1);
	hessian2_parser_read(self, 1, read2);
	res = ((code - 0x3c) << 16) + Z_STRVAL_P(read1)[0] << 8 + Z_STRVAL_P(read1)[2];

	RETURN_LONG(res);
}


/*
	Hessian2Parser::long32
*/
static PHP_METHOD(Hessian2Parser, long32)
{
	ulong code, num, res;
	zval *self, *read1, *read2, *read3, *read4;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	/*
		  return ($this->readNum() << 24) +
            ($this->readNum() << 16) +
            ($this->readNum() << 8) +
            $this->readNum();
	*/ 

	
	hessian2_parser_read(self, 1, read1);
	hessian2_parser_read(self, 1, read2);
	hessian2_parser_read(self, 1, read3);
	hessian2_parser_read(self, 1, read4);
	res = Z_STRVAL_P(read1)[0] << 24 + Z_STRVAL_P(read2)[0] << 16 + Z_STRVAL_P(read3)[0] << 8 + Z_STRVAL_P(read4)[0];

	RETURN_LONG(res);
}


/*
	Hessian2Parser::long64
*/
static PHP_METHOD(Hessian2Parser, long64)
{
	ulong code, num, res;
	zval *self, *read1, *read2, *read3, *read4, *read5, *read6, *read7, *read8;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	/*
		  return ($this->readNum() << 24) +
            ($this->readNum() << 16) +
            ($this->readNum() << 8) +
            $this->readNum();
	*/ 

	
	hessian2_parser_read(self, 1, read1);
	hessian2_parser_read(self, 1, read2);
	hessian2_parser_read(self, 1, read3);
	hessian2_parser_read(self, 1, read4);
	hessian2_parser_read(self, 1, read5);
	hessian2_parser_read(self, 1, read6);
	hessian2_parser_read(self, 1, read7);
	hessian2_parser_read(self, 1, read8);
	
	res = Z_STRVAL_P(read1)[0] << 56 + Z_STRVAL_P(read2)[0] << 48 + Z_STRVAL_P(read3)[0] << 40 + Z_STRVAL_P(read4)[0] << 32
		+ Z_STRVAL_P(read5)[0] << 24 + Z_STRVAL_P(read6)[0] << 16 + Z_STRVAL_P(read7)[0] << 8 + Z_STRVAL_P(read8)[0];

	RETURN_LONG(res);
}

/*
	Hessian2Parser::parseNull
*/
static PHP_METHOD(Hessian2Parser, parseNull)
{
	ulong code, num;


	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	
	RETURN_NULL();
}


/*
	Hessian2Parser::reserved
*/
static PHP_METHOD(Hessian2Parser, reserved)
{
	ulong code, num;
	char buf[100];

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	sprintf(buf, "Code %l reserved", code);
	zend_throw_exception(hessian_parsing_exception_entry, buf, 0 TSRMLS_DC);
}

/*
	Hessian2Parser::readUTF8Bytes
*/
static PHP_METHOD(Hessian2Parser, readUTF8Bytes)
{
	ulong len;
	zval *self;
	zval string;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &len)) {
		return;
	}

	self = getThis();
	hessian2_parser_read_utf8_bytes(self, len, &string);
	
	RETURN_STRING(Z_STRVAL(string), 1);
}

/*
	Hessian2Parser::string0
*/
static PHP_METHOD(Hessian2Parser, string0)
{
	ulong code, num;
	zval *self, *read_str;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	/*
		return $this->readUTF8Bytes($num);
	*/ 

	hessian2_parser_read_utf8_bytes(self, num, read_str);
	RETURN_ZVAL(read_str, 1, NULL);
}


/*
	Hessian2Parser::string1
*/
static PHP_METHOD(Hessian2Parser, string1)
{
	ulong code, num, len;
	zval *self, *read_str;
	zval read_bytes;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	/*
		$len = (($num - 0x30) << 8) + ord($this->read());
        	return $this->readUTF8Bytes($len);
	*/ 

	hessian2_parser_read(self, 1, read_str);
	len = (num - 0x30) << 8 + Z_STRVAL_P(read_str)[0];
	hessian2_parser_read_utf8_bytes(self, len, &read_bytes);
	RETURN_ZVAL(&read_bytes, 1, NULL);
}

/*
	Hessian2Parser::stringLongData
*/
static PHP_METHOD(Hessian2Parser, stringLongData)
{
	ulong code, num, len;
	zval *self, *read_str, *tmp_len;
	zval function_name, param1;
	zval *params[2];
	zval *z_len;
	zval string;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	/*
		$tempLen = unpack('n', $this->read(2));
        $len     = $tempLen[1];
        return $this->readUTF8Bytes($len);
	*/ 

	hessian2_parser_read(self, 2, read_str);
	ZVAL_STRING(&function_name, "unpack", 1);
	ZVAL_STRING(&param1, "n", 1);
	params[0] = &param1;
	params[1] = read_str;

	call_user_function(EG(function_table), NULL, &function_name, tmp_len, 2, params TSRMLS_DC);
	if (SUCCESS != zend_hash_find(Z_ARRVAL_P(tmp_len), "1", 1, (void **)&z_len)){
		RETURN_FALSE;
	}

	len = Z_LVAL_P(tmp_len);
	hessian2_parser_read_utf8_bytes(self, len, &string);
	
	RETURN_ZVAL(&string, 1, NULL);
}

/*
	Hessian2Parser::stringLong
*/
static PHP_METHOD(Hessian2Parser, stringLong)
{
	ulong code, num, len;
	zval *z_code;
	zval *self, *read_str;
	zend_bool final;
	char *data;
	zval z_string;
	zval function_name, param_string;
	zval *params[2];
	char *buf;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zl", &z_code, &num)) {
		return;
	}
	self = getThis();
	/*
		 do {
            $final = $num != 0x52;
            if ($num == 0x52 || $num == 0x53)
                $data .= $this->stringLongData();
            else
                $data .= $this->parse($code, 'string');
            if (!$final) {
                $code = $this->read();
                $num  = ord($code);
            }
        } while (!$final);
        return $data;
	*/ 

	final = 1;
	ZVAL_STRING(&function_name, "parse", 1);
	ZVAL_STRING(&param_string, "string", 1);
	do{
		final = num != 0x52;
		if ( (num == 0x52) || (num == 0x53) ){
			hessian2_parser_string_long_data(self, &z_string);
			if (!data){
				data = Z_STRVAL(z_string);
			}else{
				buf = Z_STRVAL(z_string);
				data = strcat(data, buf);
			}
		}else{
			params[0] = z_code;
			params[1] = &param_string;
			call_user_function(NULL, &self, &function_name, &z_string, 2, params TSRMLS_DC);
			if (!data){
				data = Z_STRVAL(z_string);
			}else{
				buf = Z_STRVAL(z_string);
				data = strcat(data, buf);
			}
		}

		if (!final){
			hessian2_parser_read(self, 1, z_code);
			num = Z_STRVAL_P(z_code)[0];
		}
	}while(!final);
}


/*
	Hessian2Parser::vlenList
*/
static PHP_METHOD(Hessian2Parser, vlenList)
{
	ulong code, num;
	zval *self;
	zval function_name;
	zval *params[2];
	zval *type, *refmap, array, *obj_list;
	zval *z_code;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	
	/*
	   $type  = $this->parseType();
        $array = array();
        $this->refmap->incReference();
	*/

	ZVAL_STRING(&function_name, "parseType", 1);
	call_user_function(NULL, &self, &function_name, type, 0,params TSRMLS_DC);
	
	refmap = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "incReference", 1);
	call_user_function(NULL, &refmap, &function_name, NULL, 0, params TSRMLS_DC);


	// $this->refmap->objectlist[] = &$array;
	array_init_size(&array, 0);
	obj_list = zend_read_property(NULL, self, ZEND_STRL("objectlist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(obj_list) != IS_ARRAY){
		array_init_size(obj_list, 1);
	}
	//todo:reference 
	zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &array, sizeof(zval**), NULL);

	/*
	 while ($code != 'Z') {
            $code = $this->read();
            if ($code != 'Z') {
                $item = $this->parse($code);
                if (HessianRef::isRef($item))
                    $array[] = &$this->refmap->objectlist[$item->index];
                else
                    $array[] = $item;
                //$array[] = $this->parse($code);
            }
        }
        return $array;
        */

	zval *item, *is_ref;
	zval function_name1;
	
	ZVAL_STRING(&function_name, "parse", 1);
	ZVAL_STRING(&function_name1, "HessianRef::isRef", 1);
	while(code != 'Z'){
		hessian2_parser_read(self, 1, z_code);
		if (Z_STRVAL_P(z_code)[0] != 'Z'){
			params[0] = z_code;
			call_user_function(NULL, &self, &function_name, item, 1, params TSRMLS_DC);


			params[0] = item;
			call_user_function(NULL, &self, &function_name, is_ref, 1, params TSRMLS_DC);

			if (i_zend_is_true(item)){
				zval *index, *node;
				index = zend_read_property(NULL, item, "index", 5, 1 TSRMLS_DC);
				if (SUCCESS == zend_hash_find(Z_ARRVAL_P(obj_list), Z_STRVAL_P(index), Z_STRLEN_P(index), (void **)&node)){
					zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &node, sizeof(zval**), NULL);
				}
			}else{
				zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &item, sizeof(zval**), NULL);
			}
		}
	}

	RETURN_ZVAL(&array, 1, NULL);
}


/*
	Hessian2Parser::flenList
*/
static PHP_METHOD(Hessian2Parser, flenList)
{
	ulong code, num;
	zval *self;
	zval function_name;
	zval *params[2];
	zval *len;
	zval *type, *refmap, array, *obj_list;
	zval *z_code;
	zval z_null, z_str_integer;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	
	/*
	  $len   = $this->parse(null, 'integer');
        $array = array();
        $this->refmap->incReference();
        $this->refmap->objectlist[] = &$array;
       
        return $array;
	*/

	ZVAL_STRING(&function_name, "parse", 1);
	Z_TYPE(z_null) = IS_NULL;
	Z_STRVAL(z_str_integer) = "integer";
	Z_STRLEN(z_str_integer) = strlen("integer")-1;
	params[0] = &z_null;
	params[1] = &z_str_integer;
	
	call_user_function(NULL, &self, &function_name, len, 2,params TSRMLS_DC);
	
	refmap = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "incReference", 1);
	call_user_function(NULL, &refmap, &function_name, NULL, 0, params TSRMLS_DC);


	// $this->refmap->objectlist[] = &$array;
	array_init_size(&array, 0);
	obj_list = zend_read_property(NULL, self, ZEND_STRL("objectlist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(obj_list) != IS_ARRAY){
		array_init_size(obj_list, 1);
	}
	//todo:reference 
	zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &array, sizeof(zval**), NULL);

	/*
	 for ($i = 0; $i < $len; $i++) {
            $item = $this->parse();
            if (HessianRef::isRef($item))
                $array[] = &$this->refmap->objectlist[$item->index];
            else
                $array[] = $item;
            //$array[] = $this->parse();
        }
        return $array;
        */

	zval *item, *is_ref;
	zval function_name1;
	int i;
	
	ZVAL_STRING(&function_name, "parse", 1);
	ZVAL_STRING(&function_name1, "HessianRef::isRef", 1);
	for(i=0; i<Z_LVAL_P(len); i++){
		call_user_function(NULL, &self, &function_name, item, 0, params TSRMLS_DC);


		params[0] = item;
		call_user_function(NULL, &self, &function_name, is_ref, 1, params TSRMLS_DC);

		if (i_zend_is_true(item)){
			zval *index, *node;
			index = zend_read_property(NULL, item, "index", 5, 1 TSRMLS_DC);
			if (SUCCESS == zend_hash_find(Z_ARRVAL_P(obj_list), Z_STRVAL_P(index), Z_STRLEN_P(index), (void **)&node)){
				zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &node, sizeof(zval**), NULL);
			}
		}else{
			zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &item, sizeof(zval**), NULL);
		}
	}

	RETURN_ZVAL(&array, 1, NULL);
}

/*
	Hessian2Parser::vlenUntypedList
*/
static PHP_METHOD(Hessian2Parser, vlenUntypedList)
{
	ulong code, num;
	zval *self;
	zval function_name;
	zval *params[2];
	zval *type, *refmap, array, *obj_list;
	zval *z_code;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	
	/*
	   $type  = $this->parseType();
        $array = array();
        $this->refmap->incReference();
	*/

	refmap = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "incReference", 1);
	call_user_function(NULL, &refmap, &function_name, NULL, 0, params TSRMLS_DC);


	// $this->refmap->objectlist[] = &$array;
	array_init_size(&array, 0);
	obj_list = zend_read_property(NULL, self, ZEND_STRL("objectlist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(obj_list) != IS_ARRAY){
		array_init_size(obj_list, 1);
	}
	//todo:reference 
	zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &array, sizeof(zval**), NULL);

	/*
	 while ($code != 'Z') {
            $code = $this->read();
            if ($code != 'Z') {
                $item = $this->parse($code);
                if (HessianRef::isRef($item))
                    $array[] = &$this->refmap->objectlist[$item->index];
                else
                    $array[] = $item;
                //$array[] = $this->parse($code);
            }
        }
        return $array;
        */

	zval *item, *is_ref;
	zval function_name1;
	
	ZVAL_STRING(&function_name, "parse", 1);
	ZVAL_STRING(&function_name1, "HessianRef::isRef", 1);
	while(code != 'Z'){
		hessian2_parser_read(self, 1, z_code);
		if (Z_STRVAL_P(z_code)[0] != 'Z'){
			params[0] = z_code;
			call_user_function(NULL, &self, &function_name, item, 1, params TSRMLS_DC);


			params[0] = item;
			call_user_function(NULL, &self, &function_name, is_ref, 1, params TSRMLS_DC);

			if (i_zend_is_true(item)){
				zval *index, *node;
				index = zend_read_property(NULL, item, "index", 5, 1 TSRMLS_DC);
				if (SUCCESS == zend_hash_find(Z_ARRVAL_P(obj_list), Z_STRVAL_P(index), Z_STRLEN_P(index), (void **)&node)){
					zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &node, sizeof(zval**), NULL);
				}
			}else{
				zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &item, sizeof(zval**), NULL);
			}
		}
	}

	RETURN_ZVAL(&array, 1, NULL);
}


/*
	Hessian2Parser::flenUntypedList
*/
static PHP_METHOD(Hessian2Parser, flenUntypedList)
{
	ulong code, num;
	zval *self;
	zval function_name;
	zval *params[2];
	zval *len;
	zval *type, *refmap, array, *obj_list;
	zval *z_code;
	zval z_null, z_str_integer;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	
	/*
	  $len   = $this->parse(null, 'integer');
        $array = array();
        $this->refmap->incReference();
        $this->refmap->objectlist[] = &$array;
       
        return $array;
	*/
	refmap = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "incReference", 1);
	call_user_function(NULL, &refmap, &function_name, NULL, 0, params TSRMLS_DC);


	// $this->refmap->objectlist[] = &$array;
	array_init_size(&array, 0);
	obj_list = zend_read_property(NULL, self, ZEND_STRL("objectlist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(obj_list) != IS_ARRAY){
		array_init_size(obj_list, 1);
	}
	//todo:reference 
	zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &array, sizeof(zval**), NULL);

	/*
	 for ($i = 0; $i < $len; $i++) {
            $item = $this->parse();
            if (HessianRef::isRef($item))
                $array[] = &$this->refmap->objectlist[$item->index];
            else
                $array[] = $item;
            //$array[] = $this->parse();
        }
        return $array;
        */

	zval *item, *is_ref;
	zval function_name1;
	int i;
	
	ZVAL_STRING(&function_name, "parse", 1);
	ZVAL_STRING(&function_name1, "HessianRef::isRef", 1);
	for(i=0; i<Z_LVAL_P(len); i++){
		call_user_function(NULL, &self, &function_name, item, 0, params TSRMLS_DC);


		params[0] = item;
		call_user_function(NULL, &self, &function_name, is_ref, 1, params TSRMLS_DC);

		if (i_zend_is_true(item)){
			zval *index, *node;
			index = zend_read_property(NULL, item, "index", 5, 1 TSRMLS_DC);
			if (SUCCESS == zend_hash_find(Z_ARRVAL_P(obj_list), Z_STRVAL_P(index), Z_STRLEN_P(index), (void **)&node)){
				zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &node, sizeof(zval**), NULL);
			}
		}else{
			zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &item, sizeof(zval**), NULL);
		}
	}

	RETURN_ZVAL(&array, 1, NULL);
}


/*
	Hessian2Parser::directListUntyped
*/
static PHP_METHOD(Hessian2Parser, directListUntyped)
{
	ulong code, num;
	zval *self;
	zval function_name;
	zval *params[2];
	ulong len;
	zval *type, *refmap, array, *obj_list;
	zval *z_code;
	zval z_null, z_str_integer;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	len = code - 0x78;
	/*
	  $len   = $this->parse(null, 'integer');
        $array = array();
        $this->refmap->incReference();
        $this->refmap->objectlist[] = &$array;
       
        return $array;
	*/
	refmap = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "incReference", 1);
	call_user_function(NULL, &refmap, &function_name, NULL, 0, params TSRMLS_DC);


	// $this->refmap->objectlist[] = &$array;
	array_init_size(&array, 0);
	obj_list = zend_read_property(NULL,self, ZEND_STRL("objectlist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(obj_list) != IS_ARRAY){
		array_init_size(obj_list, 1);
	}
	//todo:reference 
	zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &array, sizeof(zval**), NULL);

	/*
	 for ($i = 0; $i < $len; $i++) {
            $item = $this->parse();
            if (HessianRef::isRef($item))
                $array[] = &$this->refmap->objectlist[$item->index];
            else
                $array[] = $item;
            //$array[] = $this->parse();
        }
        return $array;
        */

	zval *item, *is_ref;
	zval function_name1;
	int i;
	
	ZVAL_STRING(&function_name, "parse", 1);
	ZVAL_STRING(&function_name1, "HessianRef::isRef", 1);
	for(i=0; i<len; i++){
		call_user_function(NULL, &self, &function_name, item, 0, params TSRMLS_DC);


		params[0] = item;
		call_user_function(NULL, &self, &function_name, is_ref, 1, params TSRMLS_DC);

		if (i_zend_is_true(item)){
			zval *index, *node;
			index = zend_read_property(NULL, item, "index", 5, 1 TSRMLS_DC);
			if (SUCCESS == zend_hash_find(Z_ARRVAL_P(obj_list), Z_STRVAL_P(index), Z_STRLEN_P(index), (void **)&node)){
				zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &node, sizeof(zval**), NULL);
			}
		}else{
			zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &item, sizeof(zval**), NULL);
		}
	}

	RETURN_ZVAL(&array, 1, NULL);
}



/*
	Hessian2Parser::directListTyped
*/
static PHP_METHOD(Hessian2Parser, directListTyped)
{
	ulong code, num;
	zval *self;
	zval function_name;
	zval *params[2];
	ulong len;
	zval *type, *refmap, array, *obj_list;
	zval *z_code;
	zval z_null, z_str_integer;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	len = code - 0x70;
	/*
	  $len   = ord($code) - 0x70;
        $type  = $this->parseType();
        $array = array();
        $this->refmap->incReference();
        $this->refmap->objectlist[] = &$array;
        for ($i = 0; $i < $len; $i++) {
            $item = $this->parse();
            if (HessianRef::isRef($item))
                $array[] = &$this->refmap->objectlist[$item->index];
            else
                $array[] = $item;
            //$array[] = $this->parse();
        }
        return $array;
	*/
	refmap = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "incReference", 1);
	call_user_function(NULL, &refmap, &function_name, NULL, 0, params TSRMLS_DC);


	// $this->refmap->objectlist[] = &$array;
	array_init_size(&array, 0);
	obj_list = zend_read_property(NULL,self, ZEND_STRL("objectlist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(obj_list) != IS_ARRAY){
		array_init_size(obj_list, 1);
	}
	//todo:reference 
	zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &array, sizeof(zval**), NULL);

	/*
	 for ($i = 0; $i < $len; $i++) {
            $item = $this->parse();
            if (HessianRef::isRef($item))
                $array[] = &$this->refmap->objectlist[$item->index];
            else
                $array[] = $item;
            //$array[] = $this->parse();
        }
        return $array;
        */

	zval *item, *is_ref;
	zval function_name1;
	int i;
	
	ZVAL_STRING(&function_name, "parse", 1);
	ZVAL_STRING(&function_name1, "HessianRef::isRef", 1);
	for(i=0; i<len; i++){
		call_user_function(NULL, &self, &function_name, item, 0, params TSRMLS_DC);


		params[0] = item;
		call_user_function(NULL, &self, &function_name, is_ref, 1, params TSRMLS_DC);

		if (i_zend_is_true(item)){
			zval *index, *node;
			index = zend_read_property(NULL, item, "index", 5, 1 TSRMLS_DC);
			if (SUCCESS == zend_hash_find(Z_ARRVAL_P(obj_list), Z_STRVAL_P(index), Z_STRLEN_P(index), (void **)&node)){
				zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &node, sizeof(zval**), NULL);
			}
		}else{
			zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &item, sizeof(zval**), NULL);
		}
	}

	RETURN_ZVAL(&array, 1, NULL);
}


/*
	Hessian2Parser::parseType
*/
static PHP_METHOD(Hessian2Parser, parseType)
{
	zval *self, *type;
	zval msg;
	zval function_name, z_null, param1;
	zval *params[2];

	
	self = getThis();
	/*
		 $this->logMsg('Parsing type');
        $type = $this->parse(null, 'string,integer');
      
	*/ 

	ZVAL_STRING(&msg, "Parsing type", 1);
	ZVAL_STRING(&function_name, "parse", 1);
	Z_TYPE(z_null) = IS_NULL;
	ZVAL_STRING(&function_name, "string,integer", 1);
	params[0] = &z_null;
	params[1] = &param1;

	call_user_function(NULL, &self, &function_name, type, 2, params TSRMLS_DC);

	/*
	  if (is_integer($type)) {
            $index = $type;
            if (!isset($this->refmap->reflist[$index]))
                throw new HessianParsingException("Reference index $index not found");
            return $this->refmap->typelist[$index];
        }
        $this->refmap->typelist[] = $type;
        return $type;
        */

	if (Z_TYPE_P(type) == IS_LONG){
		zval *node, *ref_map, *ref_list, *type_list;

		ref_map = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_DC);
		ref_list = zend_read_property(NULL, ref_map, ZEND_STRL("reflist"), 1 TSRMLS_DC);

		if (zend_hash_find(Z_ARRVAL_P(ref_list), Z_STRVAL_P(type), Z_STRLEN_P(type), (void **)&node) != SUCCESS){
			zend_throw_exception(hessian_parsing_exception_entry, sprintf("Reference index %s not found", Z_STRVAL_P(type))
				, 0 TSRMLS_DC);
		}
		type_list = zend_read_property(NULL, ref_map, ZEND_STRL("typelist"), 1 TSRMLS_DC);
		if (Z_TYPE_P(type_list) != IS_ARRAY){
			array_init_size(type_list, 1);
		}
		zend_hash_next_index_insert(Z_ARRVAL_P(ref_list), &type, sizeof(zval **), NULL);
	}

	RETURN_ZVAL(type, 1, NULL);
}


/*
	Hessian2Parser::untypedMap
*/
static PHP_METHOD(Hessian2Parser, untypedMap)
{
	ulong code, num;
	zval *self;
	zval function_name;
	zval *params[2];
	ulong len;
	zval *type, *refmap, array, *obj_list, *ref_list;
	zval *z_code;
	zval z_null, z_str_integer;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	len = code - 0x78;
	/*
	  $len   = $this->parse(null, 'integer');
        $array = array();
        $this->refmap->incReference();
        $this->refmap->objectlist[] = &$array;
       
        return $array;
	*/
	refmap = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "incReference", 1);
	call_user_function(NULL, &refmap, &function_name, NULL, 0, params TSRMLS_DC);


	// $this->refmap->objectlist[] = &$array;
	array_init_size(&array, 0);
	obj_list = zend_read_property(NULL, self, ZEND_STRL("objectlist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(obj_list) != IS_ARRAY){
		array_init_size(obj_list, 1);
	}
	ref_list = zend_read_property(NULL, obj_list, ZEND_STRL("reflist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(ref_list) != IS_ARRAY){
		array_init_size(ref_list, 1);
	}
	//todo:reference 
	zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &array, sizeof(zval**), NULL);

	/*
	while ($code != 'Z') {
            $key   = $this->parse($code);
            $value = $this->parse();
            if (HessianRef::isRef($key)) $key = &$this->objectlist->reflist[$key->index];
            if (HessianRef::isRef($value)) $value = &$this->objectlist->reflist[$value->index];

            $map[$key] = $value;
            if ($code != 'Z')
                $code = $this->read();
        }
        */
	zval *item, *key, *value;
	zval function_name1;
	
	ZVAL_STRING(&function_name, "parse", 1);
	ZVAL_STRING(&function_name1, "HessianRef::isRef", 1);
	while(code != 'Z'){
		zval *index, *node;
		
		params[0] = z_code;
		call_user_function(NULL, &self, &function_name, key, 1, params TSRMLS_DC);
		call_user_function(NULL, &self, &function_name, value, 0, params TSRMLS_DC);

		params[0] = key;
		call_user_function(NULL, &key, &function_name1, item, 1, params TSRMLS_DC);
		
		if (i_zend_is_true(item)){
			index = zend_read_property(NULL, key, "index", 5, 1 TSRMLS_DC);
			if (SUCCESS == zend_hash_find(Z_ARRVAL_P(ref_list), Z_STRVAL_P(index), Z_STRLEN_P(index), (void **)&node)){
				zend_hash_next_index_insert(Z_ARRVAL_P(ref_list), &node, sizeof(zval**), NULL);
			}
		}

		call_user_function(NULL, &value, &function_name1, item, 1, params TSRMLS_DC);
		
		if (i_zend_is_true(item)){
			index = zend_read_property(NULL, value, "index", 5, 1 TSRMLS_DC);
			if (SUCCESS == zend_hash_find(Z_ARRVAL_P(ref_list), Z_STRVAL_P(index), Z_STRLEN_P(index), (void **)&node)){
				zend_hash_next_index_insert(Z_ARRVAL_P(ref_list), &node, sizeof(zval**), NULL);
			}
		}

		/*
		$map[$key] = $value;
            	if ($code != 'Z')
                $code = $this->read();
                */

		zend_hash_add(Z_ARRVAL(array), Z_STRVAL_P(key), Z_STRLEN_P(key), &value, sizeof(zval **), NULL);
		if (code != 'Z'){
			hessian2_parser_read(self, 1, z_code);
			code = Z_LVAL_P(z_code);
		}
	}

	RETURN_ZVAL(&array, 1, NULL);
}


/*
	Hessian2Parser::typedMap
*/
static PHP_METHOD(Hessian2Parser, typedMap)
{
	ulong code, num;
	zval *self;
	zval function_name;
	zval *params[2];
	ulong len;
	zval *type, *refmap, array, *obj_list, *ref_list;
	zval *z_code;
	zval z_null, z_str_integer;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	len = code - 0x78;
	/*
	  $len   = $this->parse(null, 'integer');
        $array = array();
        $this->refmap->incReference();
        $this->refmap->objectlist[] = &$array;
       
        return $array;
	*/
	refmap = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "incReference", 1);
	call_user_function(NULL, &refmap, &function_name, NULL, 0, params TSRMLS_DC);


	// $this->refmap->objectlist[] = &$array;
	array_init_size(&array, 0);
	obj_list = zend_read_property(NULL, self, ZEND_STRL("objectlist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(obj_list) != IS_ARRAY){
		array_init_size(obj_list, 1);
	}
	ref_list = zend_read_property(NULL, obj_list, ZEND_STRL("reflist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(ref_list) != IS_ARRAY){
		array_init_size(ref_list, 1);
	}
	//todo:reference 
	zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &array, sizeof(zval**), NULL);

	/*
	while ($code != 'Z') {
            $key   = $this->parse($code);
            $value = $this->parse();
            if (HessianRef::isRef($key)) $key = &$this->objectlist->reflist[$key->index];
            if (HessianRef::isRef($value)) $value = &$this->objectlist->reflist[$value->index];

            $map[$key] = $value;
            if ($code != 'Z')
                $code = $this->read();
        }
        */
	zval *item, *key, *value;
	zval function_name1;
	
	ZVAL_STRING(&function_name, "parse", 1);
	ZVAL_STRING(&function_name1, "HessianRef::isRef", 1);
	while(code != 'Z'){
		zval *index, *node;
		
		params[0] = z_code;
		call_user_function(NULL, &self, &function_name, key, 1, params TSRMLS_DC);
		call_user_function(NULL, &self, &function_name, value, 0, params TSRMLS_DC);

		params[0] = key;
		call_user_function(NULL, &key, &function_name1, item, 1, params TSRMLS_DC);
		
		if (i_zend_is_true(item)){
			index = zend_read_property(NULL, key, "index", 5, 1 TSRMLS_DC);
			if (SUCCESS == zend_hash_find(Z_ARRVAL_P(ref_list), Z_STRVAL_P(index), Z_STRLEN_P(index), (void **)&node)){
				zend_hash_next_index_insert(Z_ARRVAL_P(ref_list), &node, sizeof(zval**), NULL);
			}
		}

		call_user_function(NULL, &value, &function_name1, item, 1, params TSRMLS_DC);
		
		if (i_zend_is_true(item)){
			index = zend_read_property(NULL, value, "index", 5, 1 TSRMLS_DC);
			if (SUCCESS == zend_hash_find(Z_ARRVAL_P(ref_list), Z_STRVAL_P(index), Z_STRLEN_P(index), (void **)&node)){
				zend_hash_next_index_insert(Z_ARRVAL_P(ref_list), &node, sizeof(zval**), NULL);
			}
		}

		zend_hash_add(Z_ARRVAL(array), Z_STRVAL_P(key), Z_STRLEN_P(key), &value, sizeof(zval **), NULL);
		if (code != 'Z'){
			hessian2_parser_read(self, 1, z_code);
			code = Z_LVAL_P(z_code);
		}
	}

	RETURN_ZVAL(&array, 1, NULL);
}


/*
	Hessian2Parser::typeDefinition
*/
static PHP_METHOD(Hessian2Parser, typeDefinition)
{
	ulong code, num;
	zval *self;
	zval function_name;
	zval *params[2];
	ulong len;
	zval *type;
	zval z_null, z_str_integer;
	zval *num_fields, *class_def;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();

	/*
	 $type           = $this->parseType();
        $numfields      = $this->parse(null, 'integer');
	*/
	ZVAL_STRING(&function_name, "parseType", 1);
	call_user_function(NULL, &self, &function_name, type, 0, params TSRMLS_DC);

	ZVAL_STRING(&function_name, "parse", 1);
	Z_TYPE(z_null) = IS_NULL;
	ZVAL_STRING(&z_str_integer, "integer", 1);
	params[0] = &z_null;
	params[1] = &z_str_integer;
	call_user_function(NULL, &self, &function_name, num_fields, 2, params TSRMLS_DC);

	/*
	     $classdef       = new HessianClassDef();
        $classdef->type = $type;
        */

	object_init_ex(class_def, hessian_class_def_entry);
	zend_update_property(NULL, self, ZEND_STRL("type"), type TSRMLS_DC);

	
	/*
	 for ($i = 0; $i < $numfields; $i++) {
            $classdef->props[] = $this->parse(null, 'string');
        }
        $this->refmap->addClassDef($classdef);
        return $classdef;
        */

	int i;
	zval *props;

	ZVAL_STRING(&z_str_integer, "string", 1);
	ZVAL_STRING(&function_name, "parse", 1);
	props = zend_read_property(NULL, class_def, ZEND_STRL("props"), 1 TSRMLS_DC);
	if (Z_TYPE_P(props) != IS_ARRAY){
		array_init_size(props, 1);
	}

	params[0] = &z_null;
	params[1] = &z_str_integer;
	
	for(i=0; i<Z_LVAL_P(num_fields); i++){
		zval *node;
		call_user_function(NULL, &self, &function_name, node, 2, params TSRMLS_DC);
		zend_hash_next_index_insert(Z_ARRVAL_P(props), &node, sizeof(zval **), NULL);
	}

	zval *ref_map;
	ref_map = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "addClassDef", 1);
	params[0] = class_def;
	call_user_function(NULL, &self, &function_name, NULL, 1, params TSRMLS_DC);

	RETURN_ZVAL(class_def, 1, NULL);
}

/*
	Hessian2Parser::objectInstance
*/
static PHP_METHOD(Hessian2Parser, objectInstance)
{
	ulong code, num;
	zval *self;
	zval function_name;
	zval *params[2];
	zval *res;
	zval z_null, z_str_integer;
	zval *index;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();

	/*
	 $index = $this->parse(null, 'integer');
        return $this->fillMap($index);
	*/

	ZVAL_STRING(&function_name, "parse", 1);
	Z_TYPE(z_null) = IS_NULL;
	ZVAL_STRING(&z_str_integer, "integer", 1);
	params[0] = &z_null;
	params[1] = &z_str_integer;
	call_user_function(NULL, &self, &function_name, index, 2, params TSRMLS_DC);

	ZVAL_STRING(&function_name, "fillMap", 1);
	params[0] = index;
	call_user_function(NULL, &self, &function_name, res, 1, params TSRMLS_DC);

	RETURN_ZVAL(res, 1, NULL);
}



/*
	Hessian2Parser::objectDirect
*/
static PHP_METHOD(Hessian2Parser, objectDirect)
{
	ulong code, num;
	zval *self;
	zval function_name;
	zval *params[2];
	zval index, *res;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();

	/*
	$index = $num - 0x60;
        return $this->fillMap($index);
	*/
	Z_TYPE(index) = IS_LONG;
	Z_LVAL(index) = num - 0x60;

	ZVAL_STRING(&function_name, "fillMap", 1);
	params[0] = &index;
	call_user_function(NULL, &self, &function_name, res, 1, params TSRMLS_DC);

	RETURN_ZVAL(res, 1, NULL);
}


/*
	Hessian2Parser::fillMap
*/
static PHP_METHOD(Hessian2Parser, fillMap)
{
	zval *self;
	zval *index;
	zval *ref_map, *class_list, *object_factory, *object_list;
	zval *class_def, *local_type, *class_def_type, *obj, *type_map;
	zval function_name;
	zval *params[2];
	
	/*
	 if (!isset($this->refmap->classlist[$index]))
            throw new HessianParsingException("Class def index $index not found");
       */

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &index)) {
		return;
	}
	if (Z_TYPE_P(index) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "index must be an string");
		return;
	}
	self = getThis();
	ref_map = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_DC);
	if (Z_TYPE_P(ref_map) != IS_OBJECT){
		zend_throw_exception(hessian_parsing_exception_entry, "refmap is not an oject", 0 TSRMLS_DC);
	}
	class_list = zend_read_property(NULL, self, ZEND_STRL("classlist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(ref_map) != IS_ARRAY){
		zend_throw_exception(hessian_parsing_exception_entry, "classlist is not an array", 0 TSRMLS_DC);
	}


	if (SUCCESS != zend_hash_find(Z_ARRVAL_P(class_list), Z_STRVAL_P(index)
		, Z_STRLEN_P(index),  (void **)&class_def TSRMLS_DC)){
		char buf[100];
		sprintf(buf, "Class def index %s not found", Z_STRVAL_P(index));
		zend_throw_exception(hessian_parsing_exception_entry, buf, 0 TSRMLS_DC);
	}


	/*
	 	$localType = $this->typemap->getLocalType($classdef->type);
        	$obj       = $this->objectFactory->getObject($localType);
        */
    type_map = zend_read_property(NULL, self, ZEND_STRL("typemap"), 1 TSRMLS_DC);
	class_def_type = zend_read_property(NULL, class_def, ZEND_STRL("type"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "getLocalType", 1);
	params[0] = class_def_type;

	
	call_user_function(NULL, &type_map, &function_name, local_type, 1, params TSRMLS_DC);

	object_factory = zend_read_property(NULL, self, ZEND_STRL("objectFactory"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "getObject", 1);
	params[0] = local_type;
	call_user_function(NULL, &object_factory, &function_name, obj, 1, params TSRMLS_DC);


	/*
		$this->refmap->incReference();
        	$this->refmap->objectlist[] = $obj;
        */
    ZVAL_STRING(&function_name, "incReference", 1);
	call_user_function(NULL, &ref_map, &function_name, obj, 0, params TSRMLS_DC);

	object_list = zend_read_property(NULL, ref_map, ZEND_STRL("objectlist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(object_list) != IS_ARRAY){
		array_init_size(object_list, 2);
	}
	zend_hash_next_index_insert(Z_ARRVAL_P(object_list), &obj, sizeof(zval**), NULL);

	/*
		foreach ($classdef->props as $prop) {
	            $item = $this->parse();
	            if (HessianRef::isRef($item))
	                $item = &$this->refmap->objectlist[$item->index];
	            $obj->$prop = $item;
	        }

	        return $obj;
        */
    zval *props;
	HashPosition pos;
	zval function_name1;
	zval *src_entry, *item;
	zval *is_ref;
	
	
	props = zend_read_property(NULL, class_def, ZEND_STRL("props"), 1 TSRMLS_DC);

	ZVAL_STRING(&function_name, "parse", 1);
	ZVAL_STRING(&function_name1, "HessianRef::isRef", 1);
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(props), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(props), (void **)&src_entry, &pos) == SUCCESS) {
		call_user_function(NULL, &self, &function_name, item, 0, params TSRMLS_DC);
		params[0] = item;
		call_user_function(NULL, &self, &function_name1, is_ref, 1, params TSRMLS_DC);
		if (i_zend_is_true(is_ref)){
			zval *index, *tmp;
			index = zend_read_property(NULL,  item, ZEND_STRL("index"), 1 TSRMLS_DC);
			if (SUCCESS == zend_hash_find(Z_ARRVAL_P(object_list), Z_STRVAL_P(index), Z_STRLEN_P(index), (void **)&tmp)){
				item = tmp;
			}
		}
		zend_update_property(NULL, obj, Z_STRVAL_P(src_entry), Z_STRLEN_P(src_entry), item TSRMLS_DC);
		zend_hash_move_forward_ex(Z_ARRVAL_P(props), &pos);
	}

	RETURN_ZVAL(obj, 1, NULL);
}


/*
	Hessian2Parser::reference
*/
static PHP_METHOD(Hessian2Parser, reference)
{
	zval *self;
	zval *index, *ref_map, *ref_list, *ref_list_index;
	zval function_name, param_null, param_integer;
	zval *params[2];
	zval *res;

	self = getThis();

	/*
	 $index = $this->parse(null, 'integer');
        if (!isset($this->refmap->reflist[$index]))
            throw new HessianParsingException("Reference index $index not found");
        return $this->refmap->reflist[$index];
	*/

	Z_TYPE(param_null) = IS_NULL;
	ZVAL_STRING(&param_integer, "integer", 1);

	ZVAL_STRING(&function_name, "parse", 1);
	params[0] = &param_null;
	params[1] = &param_integer;

	call_user_function(NULL, &self, &function_name, index, 2,  params TSRMLS_DC);
	ref_map = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_DC);
	ref_list = zend_read_property(NULL, ref_map, ZEND_STRL("reflist"), 1 TSRMLS_DC);

	if (zend_hash_find(Z_ARRVAL_P(ref_list), Z_STRVAL_P(index), Z_STRLEN_P(index), (void **)&ref_list_index) != SUCCESS){
		char buf[1000];

		sprintf(buf, "Reference index %s not found", Z_STRVAL_P(index));
		zend_throw_exception(hessian_parsing_exception_entry, buf, 0 TSRMLS_DC);
	}

	RETURN_ZVAL(ref_list_index, 1, NULL);
}




//Hessian2Parser functions
const zend_function_entry hessian2_writer_functions[] = {
	PHP_ME(Hessian2Parser, __construct, arginfo_hessian2_parser_construct, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, setStream, arginfo_hessian2_parser_set_stream, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, setTypeMap, arginfo_hessian2_parser_set_type_map, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, setFilters, arginfo_hessian2_parser_set_filters, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, logMsg, arginfo_hessian2_parser_log_msg, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, read, arginfo_hessian2_parser_read, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, readNum, arginfo_hessian2_read_num, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, parseCheck, arginfo_hessian2_parse_check, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, parse, arginfo_hessian2_parse, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, binary0, arginfo_hessian2_binary0, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, binary1, arginfo_hessian2_binary1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, binaryLongData, arginfo_hessian2_binary_long_data, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, binaryLong, arginfo_hessian2_binary_long, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, compactInt1, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, compactInt2, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, compactInt3, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, parseInt, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, bool, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, date, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, compactDate, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, double1, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, double2, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, double4, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, double64, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, long1, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, long2, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, long3, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, long32, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, parseNull, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, reserved, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, string0, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, string1, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, stringLongData, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, stringLong, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, readUTF8Bytes, arginfo_hessian2_read_utf8_bytes, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, vlenList, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, vlenUntypedList, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, flenUntypedList, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, directListTyped, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, directListUntyped, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, parseType, arginfo_hessian2_parse_type, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, untypedMap, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, typedMap, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, typeDefinition, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, objectInstance, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, objectDirect, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, fillMap, arginfo_hessian2_fill_map, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Parser, reference, arginfo_hessian2_reference, ZEND_ACC_PUBLIC)
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
