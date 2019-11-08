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
#include "php_hessian.h"
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

ZEND_DECLARE_MODULE_GLOBALS(hessian)


//entry
zend_class_entry *hessian2_parser_entry;



/*
	Hessian2Parser::read
*/
int hessian2_parser_read(zval *self, ulong count, zval *return_value)
{
	zval *stream;
	
	//return $this->stream->read($count);
	stream = zend_read_property(NULL, self, ZEND_STRL("stream"), 1 TSRMLS_DC);
	return hessian_stream_read(stream, count, return_value);
}



/*
	readUTF8Bytes
*/
void hessian2_parser_read_utf8_bytes(zval *self, ulong len, zval *return_value)
{
	zval read_str, read_str2;
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
    hessian2_parser_read(self, len, &read_str);
    string = Z_STRVAL(read_str);
	pos = 0;
	pass = 1;

	while(pass <= len){
		char_code = string[pos];
		if (char_code < 0x80){
			++pos;
		}else if ((char_code & 0xe0) == 0xc0){
			pos += 2;
			// $string .= $this->read(1);
			hessian2_parser_read(self, 1, &read_str2);
			string = strcat(string, Z_STRVAL(read_str2));
			zval_dtor(&read_str2);
		}else if ((char_code & 0xf0) == 0xe0){
			pos += 3;
			// $string .= $this->read(2);
			hessian2_parser_read(self, 2, &read_str2); 
			string = strcat(string, Z_STRVAL(read_str2));
			zval_dtor(&read_str2);
		}
		++pass;
	}

	RETVAL_ZVAL(return_value, string, 1);
	zval_dtor(&read_str);
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
	zval_dtor(read_str);

	len = Z_LVAL_P(tmp_len);
	hessian2_parser_read_utf8_bytes(self, len, &string);

	//estrndup

	Z_TYPE_P(return_value) = IS_STRING;
	Z_STRVAL_P(return_value) = estrndup(Z_STRVAL(string), Z_STRLEN(string));
	zval_dtor(&string);
	Z_STRLEN_P(return_value) = Z_STRLEN(string);
}



/*
	HessianRuleResolver::resolveSymbol
*/
 int hessian_rule_resolver_resolve_symbol(zend_uchar symbol, char *type_expected, hessian_parsing_rule *res)
{
	char index;
	hessian_parsing_rule rule;
	
	
	index = HESSIAN_G(hessian2_symbols)[symbol];
	rule = HESSIAN_G(hessian2_rules)[index];
	if (type_expected && (strlen(type_expected) > 0)){
		char* split[10];	//max is 10
		char *p_head, *p_end;
		int i, len;
		char *str;

		len = strlen(type_expected);
		str = pemalloc(len+1, 0);
		memcpy(str, type_expected, len);
		str[len] = 0;
		i = 0;
		p_head = str;
		while(*p_head == ' ') ++p_head;
		if (!p_head){
			return FAILURE;
		}
		p_end = str + len;
		while(p_head < p_end){
			split[i++] = p_head;
			while(*p_head && *p_head != ',') p_head++;
			if (*p_head && *p_head == ',') *p_head = 0;
			++p_head;
		}

		len = i;
		
		for(i=0; i<len; i++){
			if (strncmp(rule.type, split[i], strlen(rule.type)) == 0){
				*res = rule;
				pefree(str, 0);
				return SUCCESS;
			}
		}

		pefree(str, 0);
			char err_msg[100];
		//throw new HessianParsingException("Type $typeExpected expected");
		//@todo:error code
		sprintf(err_msg, "Type %s expected", type_expected);
		zend_error(E_WARNING, err_msg);

		return FAILURE;
	}

	*res = rule;
	return SUCCESS;	
}





/*
	Hessian2Parser::parse
*/
void hessian2_parser_parse(zval *self, zval *code, zval *expect, zval *return_value)
{
	zend_bool end = 1;
	zval function_name;
	zval *params[2];
	zval *resolver, *rule, *fun;
	zend_uchar num;
	zval z_num;
	zval value;
	zval expect_false;
	hessian_parsing_rule hessian_rule_item;
	zend_uchar *char_expect, *char_code;
	

	if (!code){
		zval code_null;
		Z_TYPE(code_null) = IS_NULL;
		code = &code_null;
	}
	if (!expect){
		char_expect = "";
	}else{
		char_expect = Z_STRVAL_P(expect);
	}
	if (!i_zend_is_true(code)){
		hessian2_parser_read(self, 1, code);
		if ((Z_TYPE_P(code) != IS_STRING) ){
			return;
		}
	}
	char_code = Z_STRVAL_P(code);
	// $rule = $this->resolver->resolveSymbol($code, $expect);
	INIT_ZVAL(z_num);
	INIT_ZVAL(value);
	Z_TYPE(value) = IS_NULL;
	
	do{
		if (SUCCESS != hessian_rule_resolver_resolve_symbol(char_code[0], char_expect, &hessian_rule_item)){
			zend_error(E_WARNING, "hessian2_parser_parse error");
			return;
		}
		/*
		 $num  = ord($code);
            	$this->logMsg("llamando $fun con code $code y num $num hex 0x" . dechex($num) . " offset " . $this->stream->pos);
            	$value = $this->$fun($code, $num);
            */


		num = char_code[0];
		ZVAL_LONG(&z_num, num);
		ZVAL_STRING(&function_name, hessian_rule_item.func, 1);
		params[0]  = code;
		params[1] = &z_num;
		hessian_call_class_function_helper(self, &function_name, 2, params, &value);
		zval_dtor(&function_name);
		

		/*
		if ($value instanceof HessianIgnoreCode) {
            		$end  = false;
           		 $code = $this->read();
       		 } else $end = true;
        		*/
        zval_dtor(code);
        if ( (Z_TYPE(value) == IS_OBJECT)
			&& instanceof_function(Z_OBJCE(value), ihessian_ignore_code_entry TSRMLS_CC)){
			end = 0 ;
			if (SUCCESS != hessian2_parser_read(self, 1, code)){
				end = 1;
				break;
			}
			char_code = Z_STRVAL_P(code);
			if (char_code == NULL){
				end = 1;
				break;
			}
        }else{
        	end = 1;
        }
	}while(!end);
	

	/*
	 	$filter = $this->filterContainer->getCallback($rule->type);
       	 if ($filter)
            		$value = $this->filterContainer->doCallback($filter, array($value, $this));
        	
            if ($filter)
                $value = $this->filterContainer->doCallback($filter, array($value, $this));
        	}
        	return $value;
        */

	zval filter, *filter_container, z_rule_type;
	zend_uchar *rule_type;
	filter_container = zend_read_property(NULL, self, ZEND_STRL("filterContainer"), 1 TSRMLS_DC);
	rule_type = hessian_rule_item.type;

	INIT_ZVAL(z_rule_type);
	ZVAL_STRING(&function_name, "getCallback", 1);
	ZVAL_STRING(&z_rule_type, rule_type, 1);
	params[0] = &z_rule_type;
	hessian_call_class_function_helper(filter_container, &function_name, 1, params, &filter);
	zval_dtor(&function_name);
	zval_dtor(&z_rule_type);


	if (i_zend_is_true(&filter)){
		zval  array;
		array_init_size(&array, 2);
		zend_hash_next_index_insert(Z_ARRVAL(array), &value, sizeof(zval **), NULL);
		zend_hash_next_index_insert(Z_ARRVAL(array), &self, sizeof(zval **), NULL);
		ZVAL_STRING(&function_name, "doCallback", 1);
		params[0] = &filter;
		params[1] = &array;
		hessian_call_class_function_helper(filter_container, &function_name, 2, params, &value);
		zval_dtor(&function_name);
	}

	/*
		if (is_object($value)) {
            		$filter = $this->filterContainer->getCallback($value);
        */
     if (Z_TYPE(value) == IS_OBJECT){
		ZVAL_STRING(&function_name, "getCallback", 1);
		params[0] = &value;
		hessian_call_class_function_helper(filter_container, &function_name, 1, params, &filter);
		zval_dtor(&function_name);
     }


	/*
	 	if ($filter)
                $value = $this->filterContainer->doCallback($filter, array($value, $this));
        */
	 if (i_zend_is_true(&filter)){
		zval  array;
		array_init_size(&array, 2);
		zend_hash_next_index_insert(Z_ARRVAL(array), &value, sizeof(zval **), NULL);
		zend_hash_next_index_insert(Z_ARRVAL(array), &self, sizeof(zval **), NULL);
		ZVAL_STRING(&function_name, "doCallback", 1);
		params[0] = &value;
		params[1] = &array;
		hessian_call_class_function_helper(filter_container, &function_name, 2, params, &value);
		zval_dtor(&function_name);
	}

	 RETURN_ZVAL(&value, 1, NULL);
}




//log msg
void hessian2_parser_log_msg(zval *self, zval *msg){
	zval *log;
	zend_uchar need_update = 0;
	
	log = zend_read_property(NULL, self, ZEND_STRL("log"), 1 TSRMLS_DC);
	if (Z_TYPE_P(log) != IS_ARRAY){
		ALLOC_ZVAL(log);
		array_init_size(log, 4);
		need_update = 1;
	}
	zend_hash_next_index_insert(Z_ARRVAL_P(log), &msg, sizeof(zval*), NULL);
	zval_addref_p(msg);
	if (need_update){
		zend_update_property(NULL, self, ZEND_STRL("log"), log TSRMLS_DC);
	}

}

/*
	Hessian2Parser::__construct
*/
static PHP_METHOD(Hessian2Parser, __construct)
{
	zval *stream, *options;
	zval *self;
	zval *ref_map, *type_map;

	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &stream, &options)) {
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

	//zend_update_property(NULL, self, ZEND_STRL("resolver"), resolver TSRMLS_DC);
	ALLOC_ZVAL(ref_map);
	ALLOC_ZVAL(type_map);

	object_init_ex(ref_map, hessian_reference_map_entry);
	object_init_ex(type_map, hessian_type_map_entry);

	zend_update_property(NULL, self, ZEND_STRL("refmap"), ref_map TSRMLS_CC);
	zend_update_property(NULL, self, ZEND_STRL("typemap"), type_map TSRMLS_CC);
	zend_update_property(NULL, self, ZEND_STRL("stream"), stream TSRMLS_CC);
	zend_update_property(NULL, self, ZEND_STRL("options"), options TSRMLS_CC);
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
void hessian2_parser_set_type_map(zval *self, zval *type_map)
{
	zend_update_property(NULL, self, ZEND_STRL("typemap"), type_map TSRMLS_DC);
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
	zval ret_val;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &count)) {
		return;
	}
	self = getThis();
	if (Z_TYPE_P(count) != IS_LONG){
		php_error_docref(NULL, E_WARNING, "count muse be an long");
		return;
	}

	//return $this->stream->read($count);
	hessian2_parser_read(self, Z_LVAL_P(count), &ret_val);

	if (Z_TYPE(ret_val) != IS_STRING){
		RETURN_FALSE;
	}
	if (!Z_STRVAL(ret_val)){
		RETURN_FALSE;
	}
	RETURN_LONG(Z_STRVAL(ret_val)[0]);
}




/*
	Hessian2Parser::parseCheck
*/
void hessian2_parser_parse_check(zval *self, zval *code, zval *return_value)
{
	zval value, is_ref;
	zval function_name;
	zval *params[2];


	hessian2_parser_parse(self, code, NULL, &value);
	ZVAL_STRING(&function_name, "value", 1);
	params[0] = &value;
	call_user_function(EG(function_table), NULL, &function_name, &is_ref, 1, params TSRMLS_DC);
	if (!i_zend_is_true(&is_ref)){
		//RETURN_ZVAL(value, 1, zval_dtor);
		RETURN_ZVAL(&value, 1, NULL);
	}
	
	/*
	 $value = $this->parse($code);
        if (!HessianRef::isRef($value))
            return $value;
        return $this->refmap->objectlist[$value->index];
        */

	zval  *ref_map, *obj_list;
	long index;
	zval **find_res;

	ref_map = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_DC);
	obj_list = zend_read_property(NULL, ref_map, ZEND_STRL("objectlist"), 1 TSRMLS_DC);
	zend_hash_index_find(Z_ARRVAL_P(obj_list), Z_LVAL(value), (void * *)&find_res);
	
	RETURN_ZVAL(*find_res, 1, NULL);
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

	hessian2_parser_parse_check(self, code, return_value);
}




/*
	Hessian2Parser::parse
*/
static PHP_METHOD(Hessian2Parser, parse)
{
	zval *code, *expect;
	zval *self;
	zval z_null, z_false;


	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|zz", &code, &expect)) {
		return;
	}
	self = getThis();


	hessian2_parser_parse(self, code, expect, return_value);
}


/*
	Hessian2Parser::binary0
*/
void hessian2_parser_binary0(zval *self, ulong code, ulong num, zval *return_value)
{
	ulong len;

	len = num - 32;
	hessian2_parser_read(self, len, return_value);
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
	hessian2_parser_binary0(self, code, num, return_value);
}


/*
	Hessian2Parser::binary1
*/
void hessian2_parser_binary1(zval *self, ulong  code, ulong num, zval *return_value)
{
	ulong len;
	zval read_str;

	/*
		 $len = (($num - 0x34) << 8) + ord($this->read());
        	return $this->read($len);
        */
    hessian2_parser_read(self, 1, &read_str);
	len = (num - 0x34) << 8;
	len += Z_STRVAL(read_str)[0];
	zval_dtor(&read_str);
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
	hessian2_parser_binary1(self, code, num, return_value);
}

/*
	Hessian2Parser::binaryLongData
*/
void hessian2_parser_binary_long_data(zval *self, zval *return_value)
{
	ulong len;
	zval function_name;
	zval *params[2];
	zval param1;
	zval read_str;
	zval retval;
	zval **find_res;

	
	/*
		 $tempLen = unpack('n', $this->read(2));
       	 $len     = $tempLen[1];
       	 return $this->read($len);
        */

	hessian2_parser_read(self, 2, &read_str);
    ZVAL_STRING(&function_name, "unpack", 1);
    ZVAL_STRING(&param1, "n", 1);
	params[0] = &param1;
	params[1] = &read_str;
	call_user_function(EG(function_table), NULL, &function_name, &retval, 2,  params TSRMLS_DC);
	zval_dtor(&function_name);
	zval_dtor(&param1);
	zval_dtor(&read_str);
	zend_hash_index_find(Z_ARRVAL(retval), 1, (void **)&find_res);
	
	len = Z_LVAL_PP(find_res);
	hessian2_parser_read(self, len, return_value);
}



/*
	Hessian2Parser::binaryLongData
*/
static PHP_METHOD(Hessian2Parser, binaryLongData)
{
	zval *self;

	self = getThis();
	
	hessian2_parser_binary_long_data(self, return_value);
}


/*
	Hessian2Parser::binaryLong
*/
void hessian2_parser_binary_long(zval *self, ulong code, ulong num, zval *return_value)
{
	ulong len;
	zend_bool final=0;
	zval z_code;
	zval function_name;
	zval *params[2];
	zval res;
	zend_uchar *data;
	zval param1;

	
	ZVAL_STRING(&param1, "binary", 1);


	/*
		 $tempLen = unpack('n', $this->read(2));
       	 $len     = $tempLen[1];
       	 return $this->read($len);
        */

	do{
		final = (num != 0x41);
		/*
		 if ($num == 0x41 || $num == 0x42)
                $data .= $this->binaryLongData();
            else
                $data .= $this->parse($code, 'binary');
                */
       if ((num == 0x41) || (num == 0x42)){
	   		hessian2_parser_binary_long_data(self, &res);
			if (!data){
				data = Z_STRVAL(res);
			}else{
				data = strcat(data, Z_STRVAL(res));
			}
       }else{
       		zval expect;
			ZVAL_STRING(&expect, "binary", 1);
       		hessian2_parser_parse(self, &z_code, &expect, &res);
			zval_dtor(&expect);
			if (!data){
				data = Z_STRVAL(res);
			}else{
				data = strcat(data, Z_STRVAL(res));
			}
       }
	}while(!final);

	RETURN_STRING(data, 0);
}



/*
	Hessian2Parser::binaryLong
*/
static PHP_METHOD(Hessian2Parser, binaryLong)
{
	ulong num;
	zval *self, *z_code;
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zl", &z_code, &num)) {
		return;
	}
	
	self = getThis();
	hessian2_parser_binary_long(self, Z_LVAL_P(z_code), num, return_value);
}


/*
	Hessian2Parser::compactInt1
*/
static PHP_METHOD(Hessian2Parser, compactInt1)
{
	ulong code, num, len;
	zval *z_code, *z_num;
	zval *self;


	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &z_code, &z_num)) {
		return;
	}
	self = getThis();


		/*
		  if ($code == 0x90)
            		return 0;
        	return ord($code) - 0x90;
        */

	if((Z_TYPE_P(z_code) == IS_LONG) && (0x90 == Z_LVAL_P(z_code))){
		RETURN_LONG(0);
	}

	if (Z_TYPE_P(z_code) == IS_STRING && Z_STRLEN_P(z_code) > 0){
		zend_uchar *buf;
		buf = Z_STRVAL_P(z_code);
		RETURN_LONG(buf[0] - 0x90);
	}
	zend_error(E_ERROR, "Hessian2Parser:compactInt1 unsuport code");
}



/*
	Hessian2Parser::compactInt2
*/
static PHP_METHOD(Hessian2Parser, compactInt2)
{
	zval *code;
	long res;
	zval *self, read_str;
	zend_uchar *buf, *buf1;


	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &code)) {
		return;
	}
	if (Z_TYPE_P(code) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "code must be a string");
		return;
	}
	self = getThis();


		/*
		 $b0 = ord($this->read());
        	return ((ord($code) - 0xc8) << 8) + $b0;
        */

	hessian2_parser_read(self, 1, &read_str);
	buf = Z_STRVAL(read_str);
	buf1 = Z_STRVAL_P(code);
	res = buf1[0] - 0xc8;
	res = res << 8;
	res += buf[0];
	zval_dtor(&read_str);
	RETURN_LONG(res);
}


/*
	Hessian2Parser::compactInt3
*/
static PHP_METHOD(Hessian2Parser, compactInt3)
{
	zval *code;
	long res;
	zval *self, b1, b0;
	zend_uchar *buf1, *buf0, *buf_code;


	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &code)) {
		return;
	}
	if (Z_TYPE_P(code) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "code must be a string");
		return;
	}
	self = getThis();


	/*
		 $b1 = ord($this->read());
        	$b0 = ord($this->read());
        	return ((ord($code) - 0xd4) << 16) + ($b1 << 8) + $b0;
        */

	hessian2_parser_read(self, 1, &b1);
	hessian2_parser_read(self, 1, &b0);

	buf1 = Z_STRVAL(b1);
	buf0 = Z_STRVAL(b0);
	buf_code = Z_STRVAL_P(code);
	
	res = buf_code[0] - 0xd4;
	res = res << 16;
	res += buf1[0] << 8;
	res += buf0[0];

	zval_dtor(&b1);
	zval_dtor(&b0);

	RETURN_LONG(res);
}


/*
	Hessian2Parser::parseInt
*/
static PHP_METHOD(Hessian2Parser, parseInt)
{
	zval *self, read_str, data, **res;
	zval function_name, param1;
	zval *params[2];


	self = getThis();


	/*
		 $data = unpack('N', $this->read(4));
        	return $data[1];
        */
	hessian2_parser_read(self, 4, &read_str);
	
	ZVAL_STRING(&function_name, "unpack", 1);
	ZVAL_STRING(&param1, "N", 1);
	params[0] = &param1;
	params[1] = &read_str;
	call_user_function(EG(function_table), NULL, &function_name, &data, 2, params TSRMLS_DC);
	zval_dtor(&function_name);
	zval_dtor(&read_str);
	if (Z_TYPE(data) != IS_ARRAY){
		RETURN_FALSE;
	}

	if (SUCCESS == zend_hash_index_find(Z_ARRVAL(data), 1, (void **)&res)){
		RETURN_LONG(Z_LVAL_PP(res));
	}
	RETURN_FALSE;
}

/*
	Hessian2Parser::bool
*/
static PHP_METHOD(Hessian2Parser, bool)
{
	zval *code;
	zend_uchar *buf;


	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &code)) {
		return;
	}
	if (Z_TYPE_P(code) != IS_STRING){
		return;
	}
	if (Z_STRLEN_P(code) < 1){
		RETURN_FALSE;
	}
	buf = Z_STRVAL_P(code);

	RETURN_BOOL(buf[0] == 'T');
}


/*
	Hessian2Parser::date
*/
static PHP_METHOD(Hessian2Parser, date)
{
	zval *self, read_str, ts;
	zval function_name;
	zval *params[1];

	self = getThis();
	/*
		 $ts = HessianUtils::timestampFromBytes64($this->read(8));
        	return $ts;
	*/
	hessian2_parser_read(self, 8, &read_str);
	ZVAL_STRING(&function_name, "HessianUtils::timestampFromBytes64", 1);
	params[0] = &read_str;
	call_user_function(EG(function_table), NULL, &function_name, &ts, 1, params TSRMLS_DC);
	zval_dtor(&function_name);
	zval_dtor(&read_str);
	
	RETURN_ZVAL(&ts, 1, NULL);
}


/*
	Hessian2Parser::compactDate
*/
static PHP_METHOD(Hessian2Parser, compactDate)
{
	ulong ts, num;
	zval *self, read_str, data;
	zval **data1, **data2, **data3, **data4;
	zval function_name, param1;
	zval *params[2];

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
	hessian2_parser_read(self, 4, &read_str);
	ZVAL_STRING(&function_name, "unpack", 1);
	ZVAL_STRING(&param1, "C4", 1);
	params[0] = &param1;
	params[1] = &read_str;
	call_user_function(EG(function_table), NULL, &function_name, &data, 2, params TSRMLS_DC);
	zval_dtor(&function_name);
	zval_dtor(&param1);
	zval_dtor(&read_str);

	if (Z_TYPE(data) != IS_ARRAY){
		RETURN_FALSE;
	}
	zend_hash_index_find(Z_ARRVAL(data), 1, (void **)&data1);
	zend_hash_index_find(Z_ARRVAL(data), 2, (void **)&data2);
	zend_hash_index_find(Z_ARRVAL(data), 3, (void **)&data3);
	zend_hash_index_find(Z_ARRVAL(data), 4, (void **)&data4);

	num = Z_LVAL_PP(data1);
	ts = num << 24;
	num = Z_LVAL_PP(data2);
	ts += num << 16;
	num = Z_LVAL_PP(data3);
	ts += num << 8;
	ts += (Z_LVAL_PP(data4)); 
	ts *= 60;

	RETURN_LONG(ts);
}


/*
	Hessian2Parser::double1
*/
static PHP_METHOD(Hessian2Parser, double1)
{
	ulong code, num;
	zval *self, read_str;
	zend_uchar *buf;

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
	hessian2_parser_read(self, 1, &read_str);
 
	buf = Z_STRVAL(read_str);
	code = buf[0];
	zval_dtor(&read_str);
	RETURN_DOUBLE(code);
}



/*
	Hessian2Parser::double2
*/
static PHP_METHOD(Hessian2Parser, double2)
{
	ulong code, num;
	zval *self, read_str, b, **data;
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
	hessian2_parser_read(self, 2, &read_str);
 	ZVAL_STRING(&function_name, "strrev", 1); 
	params[0] = &read_str; 
	call_user_function(EG(function_table), NULL, &function_name, &read_str, 1, params TSRMLS_DC);
	zval_dtor(&function_name);
	zval_dtor(&read_str);
	
	ZVAL_STRING(&function_name, "unpack", 1); 
	ZVAL_STRING(&param1, "s", 1);
	params[0] = &param1;
	params[1] = &read_str;
	call_user_function(EG(function_table), NULL, &function_name, &b, 2, params TSRMLS_DC);
	zval_dtor(&function_name);

	zend_hash_index_find(Z_ARRVAL(b),  1, (void **)&data);
	
	RETURN_DOUBLE(Z_LVAL_PP(data));
}


/*
	Hessian2Parser::double4
*/
static PHP_METHOD(Hessian2Parser, double4)
{
	ulong code, num;
	zval *self, read_str, *b;
	zend_uchar *buf;


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
	hessian2_parser_read(self, 4, &read_str);
	buf = Z_STRVAL(read_str);
	num = buf[0] << 24;
	num += buf[1] << 16;
	num += buf[2] << 8;
	num += buf[3];
	zval_dtor(&read_str);

	RETURN_DOUBLE(0.001 * num);
}

/*
	Hessian2Parser::double64
*/
static PHP_METHOD(Hessian2Parser, double64)
{
	ulong code, num;
	zval *self, read_str;
	zval *little_endian, z_double, **res;;
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
	hessian2_parser_read(self, 8, &read_str);
	little_endian = zend_read_static_property(hessian_utils_entry, ZEND_STRL("littleEndian"), 1 TSRMLS_DC);
	if (i_zend_is_true(little_endian)){
		ZVAL_STRING(&function_name, "strrev", 1);
		params[0] = &read_str;

		call_user_function(EG(function_table), NULL, &function_name, &read_str, 1, params TSRMLS_DC);
		zval_dtor(&function_name);
	}
	zval_dtor(&read_str);

	ZVAL_STRING(&function_name, "unpack", 1);
	ZVAL_STRING(&param1, "dflt", 1);
	params[0] = &param1;
	params[1] = &read_str;
	call_user_function(EG(function_table), NULL, &function_name, &z_double, 2, params TSRMLS_DC);
	zval_dtor(&function_name);
	zval_dtor(&param1);

	if (SUCCESS == zend_hash_find(Z_ARRVAL(z_double), "flt", 4, (void **)&res)){
		RETURN_ZVAL(*res, 0, NULL);
	}
	RETURN_FALSE;
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
	ulong num, res;
	zval *self, read_str;
	zend_uchar *buf;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &num)) {
		return;
	}
	self = getThis();
	/*
		 return (($num - 0xf8) << 8) + $this->readNum();
	*/ 

	
	hessian2_parser_read(self, 1, &read_str);
	buf = Z_STRVAL(read_str);
	res = ((num - 0xf8) << 8) + buf[0];
	zval_dtor(&read_str);

	RETURN_LONG(res);
}

/*
	Hessian2Parser::long3
*/
static PHP_METHOD(Hessian2Parser, long3)
{
	ulong res;
	zval *code;
	zval *self, read1, read2;
	zend_uchar *buf, *buf1, *buf2;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &code)) {
		return;
	}
	if (Z_TYPE_P(code) != IS_STRING || Z_STRLEN_P(code ) < 1){
		php_error_docref(NULL, E_WARNING, "code must be an string and can't be empty");
		return;
	}
	buf = Z_STRVAL_P(code);
	self = getThis();
	/*
		 return ((ord($code) - 0x3c) << 16)
            + ($this->readNum() << 8)
            + $this->readNum();
	*/ 

	
	hessian2_parser_read(self, 1, &read1);
	hessian2_parser_read(self, 1, &read2);
	buf1 = Z_STRVAL(read1);
	buf2 = Z_STRVAL(read2);
	res = (buf[0] - 0x3c) << 16;
	res += buf1[0] << 8;
	res += buf2[0];

	zval_dtor(&read1);
	zval_dtor(&read2);


	RETURN_LONG(res);
}


/*
	Hessian2Parser::long32
*/
static PHP_METHOD(Hessian2Parser, long32)
{
	ulong res;
	zval *self, read1, read2, read3, read4;
	zend_uchar *buf1, *buf2, *buf3, *buf4;
	
	self = getThis();
	/*
		  return ($this->readNum() << 24) +
            ($this->readNum() << 16) +
            ($this->readNum() << 8) +
            $this->readNum();
	*/ 

	
	hessian2_parser_read(self, 1, &read1);
	hessian2_parser_read(self, 1, &read2);
	hessian2_parser_read(self, 1, &read3);
	hessian2_parser_read(self, 1, &read4);

	buf1 = Z_STRVAL(read1);
	buf2 = Z_STRVAL(read2);
	buf3 = Z_STRVAL(read3);
	buf4 = Z_STRVAL(read4);
	
	res = buf1[0] << 24;
	res += buf2[0] << 16;
	res += buf3[0] << 8;
	res += buf4[0];

	zval_dtor(&read1);
	zval_dtor(&read2);
	zval_dtor(&read3);
	zval_dtor(&read4);

	RETURN_LONG(res);
}


/*
	Hessian2Parser::long64
*/
static PHP_METHOD(Hessian2Parser, long64)
{
	zend_ulong res;
	zval *self, read1, read2, read3, read4, read5, read6, read7, read8;
	zend_uchar *buf1, *buf2, *buf3, *buf4, *buf5, *buf6, *buf7, *buf8;
	

	self = getThis();
	/*
		  return ($this->readNum() << 24) +
            ($this->readNum() << 16) +
            ($this->readNum() << 8) +
            $this->readNum();
	*/ 

	
	hessian2_parser_read(self, 1, &read1);
	hessian2_parser_read(self, 1, &read2);
	hessian2_parser_read(self, 1, &read3);
	hessian2_parser_read(self, 1, &read4);
	hessian2_parser_read(self, 1, &read5);
	hessian2_parser_read(self, 1, &read6);
	hessian2_parser_read(self, 1, &read7);
	hessian2_parser_read(self, 1, &read8);

	buf1 = Z_STRVAL(read1);
	buf2 = Z_STRVAL(read2);
	buf3 = Z_STRVAL(read3);
	buf4 = Z_STRVAL(read4);
	buf5 = Z_STRVAL(read5);
	buf6 = Z_STRVAL(read6);
	buf7 = Z_STRVAL(read7);
	buf8 = Z_STRVAL(read8);

	
	res = (long)buf1[0] << 56;
	res += (long)buf2[0] << 48;
	res += (long)buf3[0] << 40;
	res += (long)buf4[0] << 32;
	res += (long)buf5[0] << 24;
	res += (long)buf6[0] << 16;
	res += (long)buf7[0] << 8;
	res += (long)buf8[0];

	zval_dtor(&read1);
	zval_dtor(&read2);
	zval_dtor(&read3);
	zval_dtor(&read4);
	zval_dtor(&read5);
	zval_dtor(&read6);
	zval_dtor(&read7);
	zval_dtor(&read8);

	RETURN_LONG(res);
}

/*
	Hessian2Parser::parseNull
*/
static PHP_METHOD(Hessian2Parser, parseNull)
{
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
	ulong num;
	zval *self, read_str;
	zval *code;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zl", &code, &num)) {
		return;
	}
	self = getThis();
	/*
		return $this->readUTF8Bytes($num);
	*/ 

	hessian2_parser_read_utf8_bytes(self, num, &read_str);
	RETVAL_ZVAL(&read_str, 1, NULL);
	zval_dtor(&read_str);
}


/*
	Hessian2Parser::string1
*/
static PHP_METHOD(Hessian2Parser, string1)
{
	ulong code, num, len;
	zval *self, read_str;
	zend_uchar *buf;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	if (num < 0x30){
		RETURN_FALSE;
	}
	self = getThis();
	/*
		$len = (($num - 0x30) << 8) + ord($this->read());
        	return $this->readUTF8Bytes($len);
	*/ 

	hessian2_parser_read(self, 1, &read_str);
	buf = Z_STRVAL(read_str);
	len = (num - 0x30) << 8;
	len += buf[0];
	if (len < 1){
		RETURN_FALSE;
	}
	hessian2_parser_read_utf8_bytes(self, len, &read_str);
	RETURN_ZVAL(&read_str, 1, NULL);
}

/*
	Hessian2Parser::stringLongData
*/
static PHP_METHOD(Hessian2Parser, stringLongData)
{
	ulong len;
	zval *self, tmp_len;
	zval function_name, *param1;
	zval *params[2];
	zval **z_len;
	zval *string;

	self = getThis();
	/*
		$tempLen = unpack('n', $this->read(2));
        $len     = $tempLen[1];
        return $this->readUTF8Bytes($len);
	*/ 

	ALLOC_ZVAL(param1);
	ALLOC_ZVAL(string);
	hessian2_parser_read(self, 2, string);
	ZVAL_STRING(&function_name, "unpack", 1);
	ZVAL_STRING(param1, "n", 1);
	params[0] = param1;
	params[1] = string;
	call_user_function(EG(function_table), NULL, &function_name, &tmp_len, 2, params TSRMLS_DC);
	zval_dtor(&function_name);
	zval_dtor(param1);
	FREE_ZVAL(param1);
	zval_dtor(string);
	
	zend_hash_index_find(Z_ARRVAL(tmp_len), 1, (void **)&z_len);
	len = Z_LVAL_PP(z_len);
	hessian2_parser_read_utf8_bytes(self, len, string);
	
	RETURN_ZVAL(string, 1, NULL);
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
	zend_uchar *buf;
	
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
			hessian2_parser_parse(self, z_code, &param_string, &z_string);
			if (!data){
				data = Z_STRVAL(z_string);
			}else{
				buf = Z_STRVAL(z_string);
				data = strcat(data, buf);
			}
		}

		if (!final){
			hessian2_parser_read(self, 1, z_code);
			buf = Z_STRVAL_P(z_code);
			num = buf[0];
			zval_dtor(z_code);
		}
	}while(!final);

	zval_dtor(&function_name);
	zval_dtor(&param_string);
	
	RETURN_STRING(buf, 0);
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
	zval type, *refmap, array, *obj_list;
	zval *z_code;
	zval ret;
	zend_uchar *buf;

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
	hessian_call_class_function_helper(self, &function_name, 0, params ,&type);
	
	refmap = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "incReference", 1);
	hessian_call_class_function_helper(refmap, &function_name, 0, params ,&ret);


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
	
	ALLOC_ZVAL(item);
	ALLOC_ZVAL(is_ref);
	
	while(code != 'Z'){
		hessian2_parser_read(self, 1, z_code);
		buf = Z_STRVAL_P(z_code);
		if (buf[0] != 'Z'){
			params[0] = z_code;
			hessian_call_class_function_helper(self, &function_name, 1, params ,item);
			//call_user_function(NULL, &self, &function_name, item, 1, params TSRMLS_DC);

			zval_dtor(z_code);
			params[0] = item;
			//hessian_call_class_function_helper(self, &function_name, 1, params ,is_ref);
			call_user_function(EG(function_table), NULL, &function_name, is_ref, 1, params TSRMLS_DC);

			if (i_zend_is_true(is_ref)){
				zval *index, **node;
				index = zend_read_property(NULL, item, "index", 5, 1 TSRMLS_DC);
				if (SUCCESS == zend_hash_find(Z_ARRVAL_P(obj_list), Z_STRVAL_P(index), Z_STRLEN_P(index)
					, (void **)&node)){
					zend_hash_next_index_insert(Z_ARRVAL(array), &node, sizeof(zval**), NULL);
				}
			}else{
				zend_hash_next_index_insert(Z_ARRVAL(array), &item, sizeof(zval**), NULL);
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
	zval function_name_parse, function_name;
	zval *params[2];
	zval len, ret;
	zval *type, *refmap, array, *obj_list;
	zval *z_code;
	zval z_null, *z_str_integer;

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
	ALLOC_ZVAL(z_str_integer);
	ZVAL_STRING(&function_name_parse, "parse", 1);
	Z_TYPE(z_null) = IS_NULL;
	ZVAL_STRING(z_str_integer, "integer", 1);
	params[0] = &z_null;
	params[1] = z_str_integer;
	
	call_user_function(NULL, &self, &function_name_parse, &len, 2,params TSRMLS_DC);
	zval_dtor(&function_name);
	
	refmap = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "incReference", 1);
	hessian_call_class_function_helper(refmap, &function_name, 0, params, &ret);


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

	zval item, *is_ref;
	int i;
	
	ZVAL_STRING(&function_name, "HessianRef::isRef", 1);
	for(i=0; i<Z_LVAL(len); i++){
		hessian2_parser_parse(self, NULL, NULL, &item);
		//call_user_function(NULL, &self, &function_name, item, 0, params TSRMLS_DC);


		params[0] = &item;
		call_user_function(NULL, &self, &function_name, is_ref, 1, params TSRMLS_DC);

		if (i_zend_is_true(&item)){
			zval *index, **node;
			index = zend_read_property(NULL, &item, "index", 5, 1 TSRMLS_DC);
			if (SUCCESS == zend_hash_find(Z_ARRVAL_P(obj_list), Z_STRVAL_P(index), Z_STRLEN_P(index), (void **)&node)){
				zend_hash_next_index_insert(Z_ARRVAL(array), node, sizeof(zval**), NULL);
			}
		}else{
			zend_hash_next_index_insert(Z_ARRVAL(array), &item, sizeof(zval**), NULL);
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
	zval ret;
	zval function_name;
	zval *params[2];
	zval *type, *refmap, array, *obj_list;
	zval z_code;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	
	/*
        $array = array();
        $this->refmap->incReference();
	*/

	refmap = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "incReference", 1);
	hessian_call_class_function_helper(refmap, &function_name, 0, params, &ret);
	zval_dtor(&function_name);
	

	// $this->refmap->objectlist[] = &$array;
	array_init_size(&array, 1);
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

	zval item, is_ref;
	zend_uchar *buf;
	
	ZVAL_STRING(&function_name, "HessianRef::isRef", 1);
	while(code != 'Z'){
		hessian2_parser_read(self, 1, &z_code);
		buf = Z_STRVAL(z_code);
		if (buf[0] != 'Z'){
			params[0] = &z_code;
			hessian2_parser_parse(self, &z_code, NULL, &item);
			zval_dtor(&z_code);
			
			params[0] = &item;
			call_user_function(EG(function_table), NULL, &function_name, &is_ref, 1, params TSRMLS_DC);

			if (i_zend_is_true(&is_ref)){
				zval *index, **node;
				index = zend_read_property(NULL, &item, "index", 5, 1 TSRMLS_DC);
				if (SUCCESS == zend_hash_find(Z_ARRVAL_P(obj_list), Z_STRVAL_P(index)
					, Z_STRLEN_P(index), (void **)&node)){
					zend_hash_next_index_insert(Z_ARRVAL(array), node, sizeof(zval**), NULL);
				}
			}else{
				zend_hash_next_index_insert(Z_ARRVAL(array), &item, sizeof(zval**), NULL);
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
	zval len;
	zval *type, *refmap, array, *obj_list;
	zval *z_code, ret;
	zval z_null, z_str_integer;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	
	/*
        $array = array();
        $this->refmap->incReference();
        $this->refmap->objectlist[] = &$array;
       
        return $array;
	*/
	refmap = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "incReference", 1);
	hessian_call_class_function_helper(refmap, &function_name, 0, params, ret);
	zval_dtor(&function_name);


	// $this->refmap->objectlist[] = &$array;
	array_init_size(&array, 0);
	obj_list = zend_read_property(NULL, self, ZEND_STRL("objectlist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(obj_list) != IS_ARRAY){
		array_init_size(obj_list, 1);
	}
	//todo:reference 
	zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &array, sizeof(zval**), NULL);

	/*
	 $len                        = $this->parse(null, 'integer');
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

	zval item, is_ref, *expect;
	int i;

	Z_TYPE(z_null) = IS_NULL;
	ALLOC_ZVAL(expect);
	ZVAL_STRING(expect, "integer", 1);
	hessian2_parser_parse(self, &z_null, expect, &len);
	
	ZVAL_STRING(&function_name, "HessianRef::isRef", 1);
	for(i=0; i<Z_LVAL(len); i++){
		hessian2_parser_parse(self, NULL, NULL, &item);
		call_user_function(NULL, &self, &function_name, &item, 0, params TSRMLS_DC);
		
		params[0] = &item;
		call_user_function(EG(function_table), NULL, &function_name, &is_ref, 1, params TSRMLS_DC);

		if (i_zend_is_true(&item)){
			zval *index, **node;
			index = zend_read_property(NULL, &item, "index", 5, 1 TSRMLS_DC);
			if (SUCCESS == zend_hash_find(Z_ARRVAL_P(obj_list), Z_STRVAL_P(index)
				, Z_STRLEN_P(index), (void **)&node)){
				zend_hash_next_index_insert(Z_ARRVAL(array), node, sizeof(zval**), NULL);
			}
		}else{
			zend_hash_next_index_insert(Z_ARRVAL(array), &item, sizeof(zval**), NULL);
		}
	}
	zval_dtor(expect);
	FREE_ZVAL(expect);

	RETURN_ZVAL(&array, 1, NULL);
}


/*
	Hessian2Parser::directListUntyped
*/
static PHP_METHOD(Hessian2Parser, directListUntyped)
{
	ulong num;
	zval *code, ret;
	zend_uchar *buf;
	zval *self;
	zval function_name;
	zval *params[2];
	ulong len;
	zval *type, *refmap, array, *obj_list;
	zval z_null, z_str_integer;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zl", &code, &num)) {
		return;
	}
	if (Z_TYPE_P(code) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "code must be a string");
		return;
	}
	self = getThis();
	
	buf = Z_STRVAL_P(code);
	len = buf[0] - 0x78;
	/*
	  $len   = ord($code) - 0x78;
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
	array_init_size(&array, 1);
	
	refmap = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "incReference", 1);
	hessian_call_class_function_helper(refmap, &function_name, 0, params, &ret);

	
	obj_list = zend_read_property(NULL, refmap, ZEND_STRL("objectlist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(obj_list) != IS_ARRAY){
		array_init_size(obj_list, 1);
		zend_update_property(NULL, refmap, ZEND_STRL("objectlist"), obj_list TSRMLS_CC);
	}
	//todo:reference 
	zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &array, sizeof(zval**), NULL);


	zval item, is_ref;
	int i;
	
	ZVAL_STRING(&function_name, "HessianRef::isRef", 1);
	for(i=0; i<len; i++){
		hessian2_parser_parse(self, NULL, NULL, &item);

		params[0] = &item;
		call_user_function(EG(function_table), NULL, &function_name, &is_ref, 1, params TSRMLS_DC);

		if (i_zend_is_true(&item)){
			zval *index, **node;
			index = zend_read_property(NULL, &item, "index", 5, 1 TSRMLS_DC);
			if (SUCCESS == zend_hash_find(Z_ARRVAL_P(obj_list), Z_STRVAL_P(index)
				, Z_STRLEN_P(index), (void **)&node)){
				zend_hash_next_index_insert(Z_ARRVAL(array), node, sizeof(zval**), NULL);
			}
		}else{
			zend_hash_next_index_insert(Z_ARRVAL(array), &item, sizeof(zval**), NULL);
		}
	}
	zval_dtor(&function_name);

	RETURN_ZVAL(&array, 1, NULL);
}



/*
	Hessian2Parser::directListTyped
*/
static PHP_METHOD(Hessian2Parser, directListTyped)
{
	ulong num;
	zval *self, *code, ret;
	zend_uchar *buf;
	zval function_name;
	zval *params[2];
	ulong len;
	zval type, *refmap, array, *obj_list;
	zval *z_code;
	zval z_null, z_str_integer;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	buf = Z_STRVAL_P(code);
	len = buf[0] - 0x70;
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
	ZVAL_STRING(&function_name, "parseType", 1);
	hessian_call_class_function_helper(self, &function_name, 0, params, &type);
	array_init_size(&array, 0);
	zval_dtor(&function_name);
	
	refmap = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "incReference", 1);
	hessian_call_class_function_helper(self, &function_name, 0, params, &ret);

	
	obj_list = zend_read_property(NULL,self, ZEND_STRL("objectlist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(obj_list) != IS_ARRAY){
		array_init_size(obj_list, 1);
		zend_update_property(NULL, self, ZEND_STRL("objectlist"), obj_list TSRMLS_CC);
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

	zval item, is_ref;
	int i;
	
	ZVAL_STRING(&function_name, "HessianRef::isRef", 1);
	for(i=0; i<len; i++){
		hessian2_parser_parse(self, NULL, NULL, &item);

		params[0] = &item;
		call_user_function(EG(function_table), NULL, &function_name, &is_ref, 1, params TSRMLS_DC);

		if (i_zend_is_true(&item)){
			zval *index, **node;
			index = zend_read_property(NULL, &item, "index", 5, 1 TSRMLS_DC);
			if (SUCCESS == zend_hash_find(Z_ARRVAL_P(obj_list), Z_STRVAL_P(index)
				, Z_STRLEN_P(index), (void **)&node)){
				zend_hash_next_index_insert(Z_ARRVAL(array), node, sizeof(zval**), NULL);
			}
		}else{
			zend_hash_next_index_insert(Z_ARRVAL(array), &item, sizeof(zval**), NULL);
		}
	}

	RETURN_ZVAL(&array, 1, NULL);
}


/*
	Hessian2Parser::parseType
*/
void hessian2_parser_parse_type(zval *self, zval *return_value)
{
	zval *type;
	zval msg;
	zval function_name, z_null, param1;
	zval *params[2];

	
	/*
		 $this->logMsg('Parsing type');
        $type = $this->parse(null, 'string,integer');
      
	*/
	INIT_ZVAL(z_null);
	INIT_ZVAL(param1);
	MAKE_STD_ZVAL(type);
	Z_TYPE(z_null) = IS_NULL;
	ZVAL_STRING(&param1, "string,integer", 1);
	hessian2_parser_parse(self, &z_null, &param1, type);

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
	zval *type_list, *ref_map, *ref_list;
	
	
	if (Z_TYPE_P(type) == IS_LONG){
		zval **node, **value;

		ref_map = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_CC);
		ref_list = zend_read_property(NULL, ref_map, ZEND_STRL("reflist"), 1 TSRMLS_CC);
		type_list = zend_read_property(NULL, ref_map, ZEND_STRL("typelist"), 1 TSRMLS_CC);
	
		if (zend_hash_find(Z_ARRVAL_P(ref_list), Z_STRVAL_P(type), Z_STRLEN_P(type)
			, (void **)&node) != SUCCESS){
			/*
			zend_throw_exception(hessian_parsing_exception_entry
				, sprintf("Reference index %s not found", Z_STRVAL_P(type))
				, 0 TSRMLS_DC);
			*/
			char err_msg[100];
			sprintf(err_msg, "Reference index %s not found", Z_STRVAL_P(type));
			zend_error(E_WARNING, err_msg, 0);
			return;
		}

		if (SUCCESS !=zend_hash_find(Z_ARRVAL_P(type_list), Z_STRVAL_P(type), Z_STRLEN_P(type)
			, (void **)&value)){
			RETURN_FALSE;
		}

		RETURN_ZVAL(*value, 1, NULL);
	}

	ref_map = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_CC);
	type_list = zend_read_property(NULL, ref_map, ZEND_STRL("typelist"), 1 TSRMLS_CC);
	if (Z_TYPE_P(type_list) != IS_ARRAY){
		array_init_size(type_list, 1);
		zend_update_property(NULL, self, ZEND_STRL("typelist"), type_list TSRMLS_CC);
	}
	zend_hash_next_index_insert(Z_ARRVAL_P(type_list), &type, sizeof(zval *), NULL);
	
	RETVAL_ZVAL(type, 1, NULL);
	zval_dtor(type);
}




/*
	Hessian2Parser::parseType
*/
static PHP_METHOD(Hessian2Parser, parseType)
{
	zval *self;

	self = getThis();
	hessian2_parser_parse_type(self, return_value);
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
	zval *type, *refmap, *map, *obj_list, *ref_list;
	zval z_code, ret;
	zval z_null, z_str_integer;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	len = code - 0x78;
	/*
	  $map = array();
        $this->refmap->incReference();
        $this->refmap->objectlist[] = &$map;
        $code                       = $this->read();
        while ($code != 'Z') {
            $key   = $this->parse($code);
            $value = $this->parse();
            if (HessianRef::isRef($key)) $key = &$this->objectlist->reflist[$key->index];
            if (HessianRef::isRef($value)) $value = &$this->objectlist->reflist[$value->index];

            $map[$key] = $value;
            if ($code != 'Z')
                $code = $this->read();
        }
        return $map;
	*/
	ALLOC_ZVAL(map);
	array_init_size(map, 0);
	
	refmap = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "incReference", 1);
	hessian_call_class_function_helper(refmap, &function_name, 0, params, &ret);
	zval_dtor(&function_name);

	obj_list = zend_read_property(NULL, self, ZEND_STRL("objectlist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(obj_list) != IS_ARRAY){
		array_init_size(obj_list, 1);
		zend_update_property(NULL, self,  ZEND_STRL("objectlist"), obj_list);
	}
	//todo:reference 
	zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &map, sizeof(zval**), NULL);



	zval item, key, *value;
	zend_uchar *buf;
	hessian2_parser_read(self, 1, &z_code);
	
	ZVAL_STRING(&function_name, "HessianRef::isRef", 1);
	ref_list = zend_read_property(NULL, obj_list, ZEND_STRL("reflist"), 1 TSRMLS_CC);
	ALLOC_ZVAL(value);
	buf =  Z_STRVAL(z_code);
	while(buf[0] != 'Z'){
		zval *index, **node;
		
		params[0] = &z_code;
		hessian2_parser_parse(self, &z_code, NULL, &key);
		hessian2_parser_parse(self, NULL, NULL, value);
		zval_dtor(&z_code);
		
		params[0] = &key;
		call_user_function(EG(function_table), NULL, &function_name, &item, 1, params TSRMLS_DC);
		
		if (i_zend_is_true(&item)){
			index = zend_read_property(NULL, &key, "index", 5, 1 TSRMLS_DC);
			if (SUCCESS == zend_hash_find(Z_ARRVAL_P(ref_list), Z_STRVAL_P(index)
				, Z_STRLEN_P(index), (void **)&node)){
				//todo:reference
				key = **node;
			}
		}

		params[0] = value;
		call_user_function(EG(function_table), NULL, &function_name, &item, 1, params TSRMLS_DC);
		
		if (i_zend_is_true(&item)){
			index = zend_read_property(NULL, value, "index", 5, 1 TSRMLS_DC);
			if (SUCCESS == zend_hash_find(Z_ARRVAL_P(ref_list), Z_STRVAL_P(index)
				, Z_STRLEN_P(index), (void **)&node)){
				//todo:reference
				value = *node;
			}
		}

		/*
		$map[$key] = $value;
            	if ($code != 'Z')
                $code = $this->read();
                */

		zend_hash_add(Z_ARRVAL_P(map), Z_STRVAL(key), Z_STRLEN(key)
			, &value, sizeof(zval **), NULL);
		if (buf[0] != 'Z'){
			hessian2_parser_read(self, 1, &z_code);
			buf = Z_STRVAL(z_code);
		}
	}

	RETURN_ZVAL(map, 1, NULL);
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
	zval *map;
	zval type, *refmap, *obj_list, *ref_list;
	zval z_code, ret;
	zval z_null, z_str_integer;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &code, &num)) {
		return;
	}
	self = getThis();
	/*
	 $type = $this->parseType();
        $map  = array();
        $this->refmap->incReference();
        $this->refmap->objectlist[] = &$map;
        // TODO references and objects
        $code = $this->read();
        while ($code != 'Z') {
            $key   = $this->parse($code);
            $value = $this->parse();
            if (HessianRef::isRef($key)) $key = &$this->objectlist->reflist[$key->index];
            if (HessianRef::isRef($value)) $value = &$this->objectlist->reflist[$value->index];

            $map[$key] = $value;
            if ($code != 'Z')
                $code = $this->read();
        }
        return $map;
	*/


	//parseType
	hessian2_parser_parse_type(self, &type);


	ALLOC_ZVAL(map);
	array_init_size(map, 1);

	
	refmap = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "incReference", 1);
	hessian_call_class_function_helper(refmap, &function_name, 0, params, &ret);


	obj_list = zend_read_property(NULL, self, ZEND_STRL("objectlist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(obj_list) != IS_ARRAY){
		array_init_size(obj_list, 1);
		zend_update_property(NULL,  self, ZEND_STRL("objectlist"), obj_list TSRMLS_CC);
	}
	//todo:reference 
	zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &map, sizeof(zval**), NULL);

	
	ref_list = zend_read_property(NULL, obj_list, ZEND_STRL("reflist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(ref_list) != IS_ARRAY){
		array_init_size(ref_list, 1);
		zend_update_property(NULL,  obj_list, ZEND_STRL("reflist"), ref_list TSRMLS_CC);
	}
	
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
	zval *item, key, *value;
	zend_uchar *buf;
	
	hessian2_parser_read(self, 1, &z_code);
	buf = Z_STRVAL(z_code);
	ZVAL_STRING(&function_name, "HessianRef::isRef", 1);
	
	while(buf[0] != 'Z'){
		zval *index, **node;
		
		params[0] = &z_code;
		hessian2_parser_parse(self, &z_code, NULL, &key);
		ALLOC_ZVAL(value);
		hessian2_parser_parse(self, NULL, NULL, value);
		zval_dtor(&z_code);

		
		params[0] = &key;
		call_user_function(EG(function_table), NULL, &function_name, item, 1, params TSRMLS_DC);
		
		if (i_zend_is_true(item)){
			index = zend_read_property(NULL, &key, "index", 5, 1 TSRMLS_DC);
			if (SUCCESS == zend_hash_find(Z_ARRVAL_P(ref_list), Z_STRVAL_P(index)
				, Z_STRLEN_P(index), (void **)&node)){
				//todo:reference
				key = **node;
			}
		}

		params[0] = value;
		call_user_function(EG(function_table), NULL, &function_name, item, 1, params TSRMLS_DC);
		
		if (i_zend_is_true(item)){
			index = zend_read_property(NULL, value, "index", 5, 1 TSRMLS_DC);
			if (SUCCESS == zend_hash_find(Z_ARRVAL_P(ref_list), Z_STRVAL_P(index)
				, Z_STRLEN_P(index), (void **)&node)){
				//todo:reference
				value = *node;
			}
		}

		/*
		$map[$key] = $value;
            	if ($code != 'Z')
                $code = $this->read();
                */

		zend_hash_add(Z_ARRVAL_P(map), Z_STRVAL(key), Z_STRLEN(key)
			, &value, sizeof(zval **), NULL);
		if (buf[0] != 'Z'){
			hessian2_parser_read(self, 1, &z_code);
			buf = Z_STRVAL(z_code);
		}
	}

	RETURN_ZVAL(map, 1, NULL);
}


/*
	Hessian2Parser::typeDefinition
*/
static PHP_METHOD(Hessian2Parser, typeDefinition)
{
	ulong *code, *num;
	zval *self;
	zval function_name;
	zval *params[2];
	ulong len;
	zval *type;
	zval z_null, z_str_integer;
	zval num_fields, *class_def;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &code, &num)) {
		return;
	}
	self = getThis();

	/*
	 $type           = $this->parseType();
        $numfields      = $this->parse(null, 'integer');
	*/
	ALLOC_ZVAL(type);
	hessian2_parser_parse_type(self, type);


	//get property count
	INIT_ZVAL(z_null);
	INIT_ZVAL(z_str_integer);
	Z_TYPE(z_null) = IS_NULL;
	ZVAL_STRING(&z_str_integer, "integer", 1);
	hessian2_parser_parse(self, &z_null, &z_str_integer, &num_fields);
	zval_dtor(&z_str_integer);

	/*
	     $classdef       = new HessianClassDef();
        $classdef->type = $type;
        */
	MAKE_STD_ZVAL(class_def);
	object_init_ex(class_def, hessian_class_def_entry);
	zend_update_property(NULL, class_def, ZEND_STRL("type"), type TSRMLS_CC);

	
	/*
	 for ($i = 0; $i < $numfields; $i++) {
            $classdef->props[] = $this->parse(null, 'string');
        }
        $this->refmap->addClassDef($classdef);
        return $classdef;
        */

	int i;
	zval *props;
	zval ret;

	ZVAL_STRING(&z_str_integer, "string", 1);
	props = zend_read_property(NULL, class_def, ZEND_STRL("props"), 1 TSRMLS_CC);
	if (Z_TYPE_P(props) != IS_ARRAY){
		array_init_size(props, 1);
		zend_update_property(NULL, class_def, ZEND_STRL("props"),props TSRMLS_CC);
	}

	//parse property
	for(i=0; i<Z_LVAL(num_fields); i++){
		zval *node;
		
		MAKE_STD_ZVAL(node);
		hessian2_parser_parse(self, NULL, &z_str_integer, node);
		
		zend_hash_next_index_insert(Z_ARRVAL_P(props), &node, sizeof(zval *), NULL);
	}

	zval *ref_map;
	ref_map = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_CC);
	hessian_reference_map_add_class_def(ref_map, class_def, &ret);

	/*
	ZVAL_STRING(&function_name, "addClassDef", 1);
	params[0] = class_def;
	hessian_call_class_function_helper(ref_map, &function_name, 1, params, &ret);
	zval_dtor(&function_name);
	*/
	
	RETURN_ZVAL(class_def, 0, NULL);
}



/*
	Hessian2Parser::objectInstance
*/
static PHP_METHOD(Hessian2Parser, objectInstance)
{
	zval *code, *num;
	zval *self;
	zval function_name;
	zval *params[2];
	zval res;
	zval z_null, z_str_integer;
	zval index;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &code, &num)) {
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
	hessian2_parser_parse(self, &z_null, &z_str_integer, &index);


	ZVAL_STRING(&function_name, "fillMap", 1);
	params[0] = &index;
	
	hessian_call_class_function_helper(self, &function_name,1, params, &res);

	RETURN_ZVAL(&res, 1, NULL);
}



/*
	Hessian2Parser::objectDirect
*/
static PHP_METHOD(Hessian2Parser, objectDirect)
{
	ulong num;
	zval *self;
	zval function_name;
	zval *params[2];
	zval index, res;
	zval *code;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zl", &code, &num)) {
		return;
	}
	self = getThis();

	/*
	$index = $num - 0x60;
        return $this->fillMap($index);
	*/
	INIT_ZVAL(index);
	INIT_ZVAL(res);
	Z_TYPE(index) = IS_LONG;
	Z_LVAL(index) = num - 0x60;

	hessian2_parser_fill_map(self, &index, &res);


	RETURN_ZVAL(&res, 1, NULL);
}



/*
	Hessian2Parser::fillMap
*/
void hessian2_parser_fill_map(zval *self, zval *index, zval *return_value)
{
	zval *ref_map, *class_list, *object_factory, *object_list;
	zval **class_def, local_type, *class_def_type, *obj, *type_map;
	zval function_name;
	zval *params[2], param1;
	zval ret;
	
	
	/*
	  if (!isset($this->refmap->classlist[$index]))
            throw new HessianParsingException("Class def index $index not found");
        $classdef = $this->refmap->classlist[$index];

        $localType = $this->typemap->getLocalType($classdef->type);
        $obj       = $this->objectFactory->getObject($localType);

        $this->refmap->incReference();
        $this->refmap->objectlist[] = $obj;

        foreach ($classdef->props as $prop) {
            $item = $this->parse();
            if (HessianRef::isRef($item))
                $item = &$this->refmap->objectlist[$item->index];
            $obj->$prop = $item;
        }

        return $obj;
       */

	ref_map = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_CC);
	if (Z_TYPE_P(ref_map) != IS_OBJECT){
		zend_error(E_WARNING, "refmap is not an oject", 0);
		return;
	}
	class_list = zend_read_property(NULL, ref_map, ZEND_STRL("classlist"), 1 TSRMLS_CC);
	if (Z_TYPE_P(class_list) != IS_ARRAY){
		zend_error(E_WARNING, "ref_map->classlist is not an array", 0);
		return;
	}


	if (SUCCESS != zend_hash_index_find(Z_ARRVAL_P(class_list), Z_LVAL_P(index)
		, (void **)&class_def)){
		char buf[100];
		sprintf(buf, "Class def index %s not found", Z_STRVAL_P(index));
		zend_throw_exception(hessian_parsing_exception_entry, buf, 0 TSRMLS_CC);
		return;
	}


	/*
	 	$localType = $this->typemap->getLocalType($classdef->type);
        	$obj       = $this->objectFactory->getObject($localType);
        */
    type_map = zend_read_property(NULL, self, ZEND_STRL("typemap"), 1 TSRMLS_CC);
	class_def_type = zend_read_property(NULL, *class_def, ZEND_STRL("type"), 1 TSRMLS_CC);
	ZVAL_STRING(&function_name, "getLocalType", 1);
	params[0] = class_def_type;
	hessian_call_class_function_helper(type_map, &function_name, 1, params,  &local_type);
	zval_dtor(&function_name);

	object_factory = zend_read_property(NULL, self, ZEND_STRL("objectFactory"), 1 TSRMLS_CC);
	ZVAL_STRING(&function_name, "getObject", 1);
	ALLOC_ZVAL(obj);
	params[0] = &local_type;
	hessian_call_class_function_helper(object_factory, &function_name, 1, params,  obj);
	zval_dtor(&function_name);


	/*
		$this->refmap->incReference();
        	$this->refmap->objectlist[] = $obj;
        */
    ZVAL_STRING(&function_name, "incReference", 1);
	INIT_ZVAL(param1);
	Z_TYPE(param1) = IS_NULL;
	params[0] = &param1;
	hessian_call_class_function_helper(ref_map, &function_name, 1, params,  &ret);
	zval_dtor(&function_name);

	object_list = zend_read_property(NULL, ref_map, ZEND_STRL("objectlist"), 1 TSRMLS_CC);
	if (Z_TYPE_P(object_list) != IS_ARRAY){
		array_init_size(object_list, 2);
		zend_update_property(NULL, ref_map, ZEND_STRL("objectlist"), object_list TSRMLS_CC);
	}
	zend_hash_next_index_insert(Z_ARRVAL_P(object_list), &obj, sizeof(zval*), NULL);

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
	zval **src_entry;
	zval is_ref;
	
	
	props = zend_read_property(NULL, *class_def, ZEND_STRL("props"), 1 TSRMLS_CC);

	ZVAL_STRING(&function_name, "HessianRef::isRef", 1);
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(props), &pos);
	
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(props), (void **)&src_entry, &pos) == SUCCESS) {
		zval *item;
		MAKE_STD_ZVAL(item);
		hessian2_parser_parse(self,NULL, NULL, item);
		
		params[0] = item;
		call_user_function(EG(function_table), NULL, &function_name, &is_ref, 1, params TSRMLS_CC);
		if (i_zend_is_true(&is_ref)){
			zval *index, **tmp;
			index = zend_read_property(NULL,  item, ZEND_STRL("index"), 1 TSRMLS_CC);
			if (SUCCESS == zend_hash_find(Z_ARRVAL_P(object_list), Z_STRVAL_P(index)
				, Z_STRLEN_P(index), (void **)&tmp)){
				item = *tmp;
			}
		}
		zend_update_property(NULL, obj, Z_STRVAL_PP(src_entry), Z_STRLEN_PP(src_entry), item TSRMLS_CC);
		zend_hash_move_forward_ex(Z_ARRVAL_P(props), &pos);
	}

	RETURN_ZVAL(obj, 1, NULL);
}


/*
	Hessian2Parser::fillMap
*/
static PHP_METHOD(Hessian2Parser, fillMap)
{
	zval *self;
	zval *index;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &index)) {
		return;
	}
	if (Z_TYPE_P(index) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "index must be an string");
		return;
	}
	self = getThis();
	hessian2_parser_fill_map(self, index, return_value);
}


/*
	Hessian2Parser::reference
*/
static PHP_METHOD(Hessian2Parser, reference)
{
	zval *self;
	zval index, *ref_map, *ref_list, **ref_list_index;
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
	hessian2_parser_parse(self, &param_null, &param_integer, &index);

	ref_map = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_DC);
	ref_list = zend_read_property(NULL, ref_map, ZEND_STRL("reflist"), 1 TSRMLS_DC);

	if (zend_hash_find(Z_ARRVAL_P(ref_list), Z_STRVAL(index), Z_STRLEN(index)
			, (void **)&ref_list_index) != SUCCESS){
		char buf[200];

		sprintf(buf, "Reference index %s not found", Z_STRVAL(index));
		zend_throw_exception(hessian_parsing_exception_entry, buf, 0 TSRMLS_DC);
		return;
	}

	RETURN_ZVAL(*ref_list_index, 1, NULL);
}




//Hessian2Parser functions
const zend_function_entry hessian2_parser_functions[] = {
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
	PHP_ME(Hessian2Parser, long64, arginfo_hessian2_compact_int1, ZEND_ACC_PUBLIC)
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
