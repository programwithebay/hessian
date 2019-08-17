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

//ZEND_DECLARE_MODULE_GLOBALS(date)


//params
ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_utils_set_timezone, 0, 0, 1)
	ZEND_ARG_INFO(0, timezone) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_utils_is_listkeys, 0, 0, 1)
	ZEND_ARG_INFO(0, arr) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_utils_is_list_iterate, 0, 0, 1)
	ZEND_ARG_INFO(0, arr) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_utils_is_list_formula, 0, 0, 1)
	ZEND_ARG_INFO(0, arr) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_utils_is_little_endian, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_utils_float_bytes, 0, 0, 1)
	ZEND_ARG_INFO(0, number) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_utils_double_bytes, 0, 0, 1)
	ZEND_ARG_INFO(0, number) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_utils_long_from_bytes64, 0, 0, 1)
	ZEND_ARG_INFO(0, bytes) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_utils_timestamp_from_bytes64, 0, 0, 1)
	ZEND_ARG_INFO(0, bytes) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_utils_is_internal_utf8, 0, 0, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_utils_string_length, 0, 0, 1)
	ZEND_ARG_INFO(0, string) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_utils_write_utf8, 0, 0, 1)
	ZEND_ARG_INFO(0, string) /* string */
ZEND_END_ARG_INFO()






//entry
zend_class_entry *hessian_utils_entry;



/*
	HessianUtils::setTimeZone
*/
static PHP_METHOD(HessianUtils, setTimeZone)
{
	zval *timezone;
	zval *self;
	zval function_name;
	zval *params[1];
	zval ret;
	
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &timezone)) {
		return;
	}
	self = getThis();

	/*
		if($timezone)
			date_default_timezone_set($timezone);
		else {
			$origError = error_reporting(0);
			$tz = date_default_timezone_get();
			error_reporting($origError);
			date_default_timezone_set($tz);
		}
	*/
	
	if (i_zend_is_true(timezone)){
		ZVAL_STRING(&function_name, "date_default_timezone_set", 1);
		params[0] = timezone;
		call_user_function(EG(function_table), NULL, &function_name, &ret, 1, params TSRMLS_DC);
		zval_dtor(&function_name);
	}else{
		int old_error_reporting;
		zval *tz;
		
		old_error_reporting = EG(error_reporting);

		ZVAL_STRING(&function_name, "date_default_timezone_get", 1);
		call_user_function(EG(function_table), NULL, &function_name, &ret, 0, params TSRMLS_DC);
		zval_dtor(&function_name);
		
		zend_alter_ini_entry("error_reporting", sizeof("error_reporting"), "0", 1, ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME);

		ZVAL_STRING(&function_name, "date_default_timezone_set", 1);
		params[0] = tz;
		call_user_function(EG(function_table), NULL, &function_name, &ret, 1, params TSRMLS_DC);
		zval_dtor(&function_name);
	}
}


/*
	HessianUtils::isListKeys
	//@todo:not implements now, not found be used
*/
static PHP_METHOD(HessianUtils, isListKeys)
{
	zval *arr;
	zval *self;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &arr)) {
		return;
	}
	self = getThis();

	if (Z_TYPE_P(arr) != IS_ARRAY){
		php_error_docref(NULL, E_WARNING, "arr must be an array");
		return;
	}

	/*
		$vals = array_values($arr);
		$keys = array_keys($arr);
		$numkeys = array_keys($vals);
		return $numkeys === $keys;
	*/

	
}


/*
	HessianUtils::isListIterate
*/
static PHP_METHOD(HessianUtils, isListIterate)
{
	zval *arr, **src_entry;
	zval *self;
	HashPosition pos;
	char *string_key;
	uint string_key_len;
	ulong num_key;
	int k;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &arr)) {
		return;
	}
	self = getThis();

	if (Z_TYPE_P(arr) != IS_ARRAY){
		php_error_docref(NULL, E_WARNING, "arr must be an array");
		return;
	}

	/*
		$k = 0;
		foreach($arr as $key => $val) {
			if($key !== $k)
				return false;
			$k++;
		}
		return true;
	*/

	k = 0;
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(arr), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(arr), (void **)&src_entry, &pos) == SUCCESS) {
		string_key_len = 0;
		zend_hash_get_current_key_ex(Z_ARRVAL_P(arr), &string_key, &string_key_len, &num_key, 0, &pos);
		if (string_key_len > 0){
			if (atoi(string_key) != k){
				RETURN_FALSE;
			}
		}else{
			if (num_key != k){
				RETURN_FALSE;
			}
		}
		k++;
		zend_hash_move_forward_ex(Z_ARRVAL_P(arr), &pos);
	}

	RETURN_TRUE;
}


/*
	HessianUtils::isListFormula
*/
static PHP_METHOD(HessianUtils, isListFormula)
{
	zval *arr, **src_entry;
	HashPosition pos;
	zval *self;
	uint n,sum;
	char *string_key;
	uint string_key_len;
	ulong num_key;
	uint key_sum;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &arr)) {
		return;
	}
	self = getThis();

	if (Z_TYPE_P(arr) != IS_ARRAY){
		php_error_docref(NULL, E_WARNING, "arr must be an array");
		return;
	}


	/*
		$n = count($arr);
		$sum = (0*$n) + ( ($n*($n-1)*1)/2 );
		$keys = array_keys($arr);
		$keysum = array_sum($keys);
		return $sum === $keysum;
	*/

	n = zend_hash_num_elements(Z_ARRVAL_P(arr));
	sum = n * (n - 1) / 2;

	key_sum = 0;
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(arr), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(arr), (void **)&src_entry, &pos) == SUCCESS) {
		string_key_len = 0;
		zend_hash_get_current_key_ex(Z_ARRVAL_P(arr), &string_key, &string_key_len, &num_key, 0, &pos);
		if (string_key_len > 0){
			key_sum += atoi(string_key);
		}else{
			key_sum  += num_key;
		}
		zend_hash_move_forward_ex(Z_ARRVAL_P(arr), &pos);
	}

	if (key_sum == sum){
		RETURN_TRUE;
	}else{
		RETURN_FALSE;
	}
}


/*
	HessianUtils::isLittleEndian
	@todo: not imeplements
*/
static PHP_METHOD(HessianUtils, isLittleEndian)
{
	zval* little_endian, matchine_long, index_long, compare_ret;
	zval function_name;
	zval *param1, *param2;
	zval *params[2];
	
	little_endian = zend_read_static_property(hessian_utils_entry, ZEND_STRL("littleEndian"), 1 TSRMLS_DC);
	if ( !(Z_TYPE_P(little_endian) == IS_NULL)){
		RETURN_ZVAL(little_endian, 1, NULL);
	}

	/*
		$machineLong = pack("L", 1);  // Machine dependent
		$indepLong  = pack("N", 1);  // Machine independent
		self::$littleEndian = $machineLong[0] != $indepLong[0];
		return self::$littleEndian;
	*/

	MAKE_STD_ZVAL(param1);
	MAKE_STD_ZVAL(param2);

	ZVAL_STRING(&function_name, "pack", 1);
	ZVAL_STRING(param1, "L", 1);
	ZVAL_LONG(param2, 1);
	params[0] = param1;
	params[1] = param2;

	call_user_function(EG(function_table), NULL, &function_name, &matchine_long, 2, params TSRMLS_DC);
	zval_dtor(param1);

	
	ZVAL_STRING(param1, "N", 1);
	params[0] = param1;

	call_user_function(EG(function_table), NULL, &function_name, &index_long, 2, params TSRMLS_DC);
	if (SUCCESS == compare_function(&compare_ret, &matchine_long, &index_long TSRMLS_DC)){
		if (0 == Z_LVAL(compare_ret)){
			ZVAL_BOOL(little_endian, 0);
		}else{
			ZVAL_BOOL(little_endian, 1);
		}
	}else{
		ZVAL_BOOL(little_endian, 1);
	}
	
	zval_dtor(param2);
	zval_dtor(&function_name);
	FREE_ZVAL(param1);
	FREE_ZVAL(param2);

	RETURN_ZVAL(little_endian, 1, NULL);
}



/*
	HessianUtils::floatBytes
*/
static PHP_METHOD(HessianUtils, floatBytes)
{
	zval *number, *little_endian;
	zval *self, *param1;
	zval bytes;
	zval function_name;
	zval *params[2];
	
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &number)) {
		return;
	}
	self = getThis();

	if (Z_TYPE_P(number) != IS_DOUBLE){
		php_error_docref(NULL, E_WARNING, "number must be an float");
		return;
	}

	/*
		$bytes = pack("s", $number);
		return self::$littleEndian ? strrev($bytes) : $bytes; 
	*/

	little_endian = zend_read_static_property(hessian_utils_entry, ZEND_STRL("littleEndian"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "pack", 1);
	MAKE_STD_ZVAL(param1);
	ZVAL_STRING(param1, "s", 1);
	params[0] = param1;
	params[1] = number;
	call_user_function(EG(function_table), NULL, &function_name, &bytes, 2, params TSRMLS_DC);
	FREE_ZVAL(param1);
	zval_dtor(&function_name);
	
	if (i_zend_is_true(little_endian)){
		char *n, *p, *e;
		char *str;

		str = Z_STRVAL(bytes);
		n = emalloc(Z_STRLEN(bytes)+1);
		p = n;

		e = str + Z_STRLEN(bytes);

		while (--e>=str) {
			*p++ = *e;
		}

		*p = '\0';

		RETVAL_STRINGL(n, Z_STRLEN(bytes), 0);
	}else{
		RETURN_ZVAL(&bytes, 1, NULL);
	}
}

/*
*
	HessianUtils::doubleBytes
*/
static PHP_METHOD(HessianUtils, doubleBytes)
{
	zval *number, *little_endian;
	zval *self, *param1;
	zval bytes;
	zval function_name;
	zval *params[2];
	
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &number)) {
		return;
	}
	self = getThis();

	if (Z_TYPE_P(number) != IS_DOUBLE){
		php_error_docref(NULL, E_WARNING, "number must be an float");
		return;
	}

	/*
		$bytes = pack("s", $number);
		return self::$littleEndian ? strrev($bytes) : $bytes; 
	*/

	little_endian = zend_read_static_property(hessian_utils_entry, ZEND_STRL("littleEndian"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "pack", 1);
	MAKE_STD_ZVAL(param1);
	ZVAL_STRING(param1, "d", 1);
	params[0] = param1;
	params[1] = number;
	call_user_function(EG(function_table),NULL, &function_name, &bytes, 2, params TSRMLS_DC);
	FREE_ZVAL(param1);
	
	if (i_zend_is_true(little_endian)){
		char *n, *p, *e;
		char *str;

		str = Z_STRVAL(bytes);
		n = emalloc(Z_STRLEN(bytes)+1);
		p = n;

		e = str + Z_STRLEN(bytes);

		while (--e>=str) {
			*p++ = *e;
		}

		*p = '\0';

		RETVAL_STRINGL(n, Z_STRLEN(bytes), 0);
	}else{
		RETURN_ZVAL(&bytes, 1, NULL);
	}
}


/*
	HessianUtils::longFromBytes64
	//@todo:not implements
*/
static PHP_METHOD(HessianUtils, longFromBytes64)
{
	//done use
	zend_error(E_CORE_ERROR, "not implement");
	
}

/*
	HessianUtils::timestampFromBytes64
*/
static PHP_METHOD(HessianUtils, timestampFromBytes64)
{
	zval *bytes, *little_endian;
	zval *self, *param1;
	zval function_name;
	zval *params[2];
	char sec1[4], sec2[4];
	char *buf_bytes;
	zval **z_pow32;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &bytes)) {
		return;
	}
	self = getThis();

	if (Z_TYPE_P(bytes) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "bytes must be an string");
		return;
	}

	/*
	$sec1 = $bytes[0].$bytes[1].$bytes[2].$bytes[3]; // primeros 4
		$sec2 = $bytes[4].$bytes[5].$bytes[6].$bytes[7]; // segundos 4
		$b1 = unpack('N', $sec1);
		$b2 = unpack('N', $sec2);
		
	*/

	buf_bytes = Z_STRVAL_P(bytes);
	if (Z_STRLEN_P(bytes) < 8){
		RETURN_FALSE;
	}

	memcpy(sec1, buf_bytes, 4);
	sec1[4] = 0 ;
	memcpy(sec2, buf_bytes+4, 4);
	sec2[4] = 0 ;

	zval b1, b2;
	zval *format;


	MAKE_STD_ZVAL(format);
	MAKE_STD_ZVAL(param1);


	ZVAL_STRING(&function_name, "unpack", 1);
	ZVAL_STRING(format, "N", 1);
	params[0 ] = format;
	ZVAL_STRINGL(param1, sec1, 4, 1);
	params[1] = param1;
	call_user_function(EG(function_table), NULL, &function_name, &b1, 2, params TSRMLS_DC);

	ZVAL_STRINGL(param1, sec2,4,  1);
	params[1] = param1;
	call_user_function(EG(function_table), NULL, &function_name, &b2, 2, params TSRMLS_DC);
	zval_dtor(&function_name);

	FREE_ZVAL(format);
	FREE_ZVAL(param1);
	/*
		$res = $b2[1];
		$num = ($b1[1] * self::pow32);
		if($res < 0){ // overflow
			$comp = PHP_INT_MAX + $res;
			$num += PHP_INT_MAX + $comp + 1000;
		} else {
			$num += $res; 
		}
		$num = $num / 1000;
		return $num;
	*/

	
	double res;
	double num;
	zval **unpack_b1, **unpack_b2;

	zend_hash_find(&hessian_utils_entry->constants_table, "pow32", sizeof("pow32"), (void **) &z_pow32);

	//b1 and b2 type is array
	zend_hash_index_find(Z_ARRVAL(b1), 1, (void**)&unpack_b1);
	zend_hash_index_find(Z_ARRVAL(b2), 1, (void**)&unpack_b2);
	
	res = Z_LVAL_PP(unpack_b2);
	num =  Z_LVAL_PP(unpack_b1) * Z_LVAL_PP(z_pow32);

	//why small than 0?
	if (res < 0){
		num = LONG_MAX + res + LONG_MAX + 1000;
	}else{
		num += res;
	}
	num = num / 1000;

	RETURN_DOUBLE(num);
}

/*
	HessianUtils::isInternalUTF8
*/
static PHP_METHOD(HessianUtils, isInternalUTF8)
{
	RETURN_TRUE;
}


/*
	HessianUtils::stringLength
*/
void hessian_utils_string_length(zval *string, zval *return_value)
{
	int len;
	zval function_name;
	zval *charset;
	zval *params[1];
	zval res;


	Z_ADDREF_P(string);
	ALLOC_ZVAL(charset);
	ZVAL_STRINGL(charset, "UTF-8", 5, 1);
	params[0] = string;
	params[1] = charset;
	ZVAL_STRING(&function_name, "mb_strlen", 1);
	call_user_function(EG(function_table), NULL, &function_name, &res, 2, params TSRMLS_DC);
	len = Z_LVAL(res);

	FREE_ZVAL(charset);

	RETURN_LONG(len);
}



/*
	HessianUtils::stringLength
*/
static PHP_METHOD(HessianUtils, stringLength)
{
	zval *string;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &string)) {
		return;
	}
	if (Z_TYPE_P(string) != IS_STRING){
		return;
	}

	hessian_utils_string_length(string, return_value);
}


/*
	HessianUtils::writeUTF8
*/
void hessian_utils_write_utf8(zval *string, zval *return_value)
{

	//$encoding = ini_get('mbstring.internal_encoding'); must be utf8
	//*return_value = *string;
	*return_value = *string;
	//RETURN_ZVAL(string, 1, NULL);
}




/*
	HessianUtils::writeUTF8
*/
static PHP_METHOD(HessianUtils, writeUTF8)
{
	zval *string;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &string)) {
		return;
	}
	if (Z_TYPE_P(string) != IS_STRING){
		return;
	}

	hessian_utils_write_utf8(string, return_value);
}




//HessianUtils functions
const zend_function_entry hessian_utils_functions[] = {
	PHP_ME(HessianUtils, setTimeZone, arginfo_hessian_utils_set_timezone, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(HessianUtils, isListKeys, arginfo_hessian_utils_is_listkeys, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(HessianUtils, isListIterate, arginfo_hessian_utils_is_list_iterate, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(HessianUtils, isListFormula, arginfo_hessian_utils_is_list_formula, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(HessianUtils, isLittleEndian, arginfo_hessian_utils_is_little_endian, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(HessianUtils, floatBytes, arginfo_hessian_utils_float_bytes, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(HessianUtils, doubleBytes, arginfo_hessian_utils_double_bytes, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(HessianUtils, longFromBytes64, arginfo_hessian_utils_long_from_bytes64, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(HessianUtils, timestampFromBytes64, arginfo_hessian_utils_timestamp_from_bytes64, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(HessianUtils, isInternalUTF8, arginfo_hessian_utils_is_internal_utf8, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(HessianUtils, stringLength, arginfo_hessian_utils_string_length, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(HessianUtils, writeUTF8, arginfo_hessian_utils_write_utf8, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
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
