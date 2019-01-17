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
#include "php_hessian_init.h"


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
	
	//return $this->stream->read($count);
	stream = zend_read_property(NULL, self, ZEND_STRL("stream"), 1 TSRMLS_DC);
	ZVAL_STRING(function_name, "read", 1);
	ZVAL_LONG(z_count, count);
	params[0] = &z_count;

	call_user_function(NULL, stream, &function_name, &return_value, 1, params TSRMLS_DC);
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
	log = zend_read_property(NULL, self, ZEND_STRL("log"), 1 TSRMLS_DC);
	if (Z_TYPE_P(log) != IS_ARRAY){
		ALLOC_ZVAL(log);
		array_init_size(log, 4);
	}
	zend_hash_next_index_insert(Z_ARRVAL_P(log), &msg, sizeof(zval**), NULL);
	zend_update_property(NULL, self, ZEND_STRL("log"), log TSRMLS_DC);
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
	ZVAL_STRING(function_name, "read", 1);
	params[0] = count;

	call_user_function(NULL, stream, &function_name, &ret_val, 1, params TSRMLS_DC);
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
	uchar num;
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
			ZVAL_STRING(function_name, "resolveSymbol", 1);	
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
			ZVAL_LONG(z_num, num);
			params[0] = code;
			params[1] = z_num;
			ZVAL_STRING(function_name, Z_STRVAL_P(fun), 1);
			call_user_function(NULL, &self, &function_name, value, 2, params TSRMLS_DC);

			/*
			if ($value instanceof HessianIgnoreCode) {
                		$end  = false;
               		 $code = $this->read();
           		 } else $end = true;
            		*/
            if (instanceof_function(Z_OBJCE_P(value), hessian_ignore_code_entry TSRMLS_DC){
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
	ZVAL_STRING(function_name, "getCallback", 1);
	params[0] = rule_type;
	call_user_function(NULL, &filter_container, &function_name, filter, 1, params TSRMLS_DC);

	if (i_zend_is_true(filter)){
		zval  array;
		array_init_size(array, 2);
		zend_hash_next_index_insert(Z_ARRVAL_P(array), &value, sizeof(zval **), NULL);
		zend_hash_next_index_insert(Z_ARRVAL_P(array), &self, sizeof(zval **), NULL);
		ZVAL_STRING(function_name, "doCallback", 1);
		params[0] = filter;
		params[1] = &array;
		call_user_function(NULL, &filter_container, &function_name, value, 2, params TSRMLS_DC);
	}

	/*
		if (is_object($value)) {
            		$filter = $this->filterContainer->getCallback($value);
        */
     if (Z_TYPE_P(value) == IS_OBJECT){
		ZVAL_STRING(function_name, "getCallback", 1);
		params[0] = value;
		call_user_function(NULL, &filter_container, &function_name, filter, 1, params TSRMLS_DC);
     }


	/*
	 	if ($filter)
                $value = $this->filterContainer->doCallback($filter, array($value, $this));
        */
	 if (i_zend_is_true(filter)){
		zval  array;
		array_init_size(array, 2);
		zend_hash_next_index_insert(Z_ARRVAL_P(array), &value, sizeof(zval **), NULL);
		zend_hash_next_index_insert(Z_ARRVAL_P(array), &self, sizeof(zval **), NULL);
		ZVAL_STRING(function_name, "doCallback", 1);
		params[0] = value;
		params[1] = &array;
		call_user_function(NULL, &filter_container, &function_name, value, 2, params TSRMLS_DC);
	}

	 RETURN_ZVAL(value, 1);
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
    ZVAL_STRING(function_name, "unpack", 1);
    ZVAL_STRING(param1, "n", 1);
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
			call_user_function(NULL, self, &function_name, res, 0, params TSRMLS_DC);
			if (!data){
				data = Z_STRVAL_P(res);
			}else{
				data = strcat(data, Z_STRVAL_P(res));
			}
       }else{
       		ZVAL_STRING(&function_name, "parse", 1);
			params[0] = z_code;
			params[1] = &param1;
			call_user_function(NULL, self, &function_name, res, 0, params TSRMLS_DC);
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

	ZVAL_STRING(function_name, "unpack", 1);
	ZVAL_STRING(function_name, "N", 1);
	hessian2_parser_read(self, 4, read_str);
	params[0] = &param1;
	params[1] = read_str;
	call_user_function(EG(function_table), NULL, &function_name, data, 2, params TSRMLS_DC);
	if (Z_TYPE_P(data) != IS_ARRAY){
		RETURN_FALSE;
	}
	//@todo check is array
	zend_hash_find(Z_ARRVAL_P(data), "1", 1, &res);

	if (res){
		RETURN_ZVAL(res, 1);
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
	ZVAL_STRING(function_name, "HessianUtils::timestampFromBytes64", 1);
	params[0] = read_str;
	call_user_function(EG(function_table), NULL, &function_name, ts, 1, params TSRMLS_DC);

	RETURN_ZVAL(ts, 1);
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
	ZVAL_STRING(function_name, "unpack", 1);
	ZVAL_STRING(param1, "C4", 1);
	params[0] = param1;
	params[1] = read_str;
	call_user_function(EG(function_table), NULL, &function_name, data, 1, params TSRMLS_DC);


	zend_hash_find(Z_ARRVAL_P(data), "1", 1, &data1);
	zend_hash_find(Z_ARRVAL_P(data), "2", 1, &data2);
	zend_hash_find(Z_ARRVAL_P(data), "3", 1, &data3);
	zend_hash_find(Z_ARRVAL_P(data), "4", 1, &data4);

	ts = Z_STRVAL_P(data1)[0] << 24 + Z_STRVAL_P(data1)[1] << 16 + Z_STRVAL_P(data1)[2] << 8 + Z_STRVAL_P(data1)[3]; 
	ts *= 60;

	RETURN_ZVAL(ts, 1);
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
 	ZVAL_STRING(function_name, "strrev", 1); 
	params[0] = read_str; 
	call_user_function(EG(function_table), NULL, &function_name, read_str, 1, params TSRMLS_DC);

	ZVAL_STRING(function_name, "unpack", 1); 
	ZVAL_STRING(param1, "s", 1);
	params[0] = &param1;
	params[1] = read_str;
	call_user_function(EG(function_table), NULL, &function_name, b, 1, params TSRMLS_DC);


	zend_hash_find(Z_ARRVAL_P(b), "1", 1, &data);
	
	RETURN_DOUBLE(Z_STRVAL_P(data)[0]);
}




//Hessian2Parser functions
const zend_function_entry hessian2_iterator_writer_functions[] = {
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
	PHP_ME(Hessian2Parser, double2 arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
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
