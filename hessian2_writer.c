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
#include <math.h>
#include "hessian_common.h"
#include "php_hessian_int.h"

#define BETWEEN(value, min, max) ((min <= value) && (value <= max))

//params
ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_construct, 0, 0, 1)
	ZEND_ARG_INFO(0, options) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_log_msg, 0, 0, 1)
	ZEND_ARG_INFO(0, msg) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_set_typemap, 0, 0, 1)
	ZEND_ARG_INFO(0, typemap) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_set_filters, 0, 0, 1)
	ZEND_ARG_INFO(0, container) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_write_value, 0, 0, 1)
	ZEND_ARG_INFO(0, value) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_resolve_dispatch, 0, 0, 1)
	ZEND_ARG_INFO(0, type) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_write_null, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_write_map, 0, 0, 2)
	ZEND_ARG_INFO(0, map) /* string */
	ZEND_ARG_INFO(0, type) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_write_object_data, 0, 0, 1)
	ZEND_ARG_INFO(0, value) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_write_array, 0, 0, 1)
	ZEND_ARG_INFO(0, array) /* string */
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_write_object, 0, 0, 1)
	ZEND_ARG_INFO(0, value) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_write_type, 0, 0, 1)
	ZEND_ARG_INFO(0, type) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_write_reference, 0, 0, 1)
	ZEND_ARG_INFO(0, value) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_write_date, 0, 0, 1)
	ZEND_ARG_INFO(0, value) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_between, 0, 0, 3)
	ZEND_ARG_INFO(0, value) /* string */
	ZEND_ARG_INFO(0, min) /* string */
	ZEND_ARG_INFO(0, max) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_write_int, 0, 0, 1)
	ZEND_ARG_INFO(0, value) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_write_string, 0, 0, 1)
	ZEND_ARG_INFO(0, value) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_write_small_string, 0, 0, 1)
	ZEND_ARG_INFO(0, value) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_write_string_data, 0, 0, 1)
	ZEND_ARG_INFO(0, string) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_write_double, 0, 0, 1)
	ZEND_ARG_INFO(0, value) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_write_resource, 0, 0, 1)
	ZEND_ARG_INFO(0, handle) /* string */
ZEND_END_ARG_INFO()



//entry
zend_class_entry *hessian2_writer_entry;


//compare zval
//@todo:is right?
char hessian2_writer_compare(zval *func1, zval *func2)
{
	char ret;
	if (Z_TYPE_P(func1) == IS_STRING && Z_TYPE_P(func2) == IS_STRING) {
		ret = (zend_binary_zval_strcmp(func1, func2) == 0);
	} else if (Z_TYPE_P(func1) == IS_ARRAY && Z_TYPE_P(func2) == IS_ARRAY) {
		zval result;
		zend_compare_arrays(&result, func1, func2 TSRMLS_CC);
		ret = (Z_LVAL(result) == 0);
	} else if (Z_TYPE_P(func1) == IS_OBJECT && Z_TYPE_P(func2) == IS_OBJECT) {
		zval result;
		zend_compare_objects(&result, func1, func2 TSRMLS_CC);
		ret = (Z_LVAL(result) == 0);
	} else {
		ret = 0;
	}
	return ret;
}



/*
	Hessian2Writer::writeStringData
*/
void hessian2_writer_write_string_data(zval *self, zval *string, zval *return_value)
{
	//return HessianUtils::writeUTF8($string);
	hessian_utils_write_utf8(string, return_value);
}




/*
	Hessian2Writer::writeString
*/
void hessian2_writer_write_string(zval *self, zval *value, zval *return_value)
{
	zval  z_len;
	zval function_name, param1, param2;
	zval *params[2];
	char *buf, *p;
	int i;
	long buf_len;



	//$len = HessianUtils::stringLength($value);
	INIT_ZVAL(z_len);
	ZVAL_LONG(&z_len, Z_STRLEN_P(value));
	//hessian_utils_string_length(value, &z_len);
	INIT_ZVAL(param1);
	ZVAL_STRING(&function_name, "pack", 1);

	if (Z_LVAL(z_len) < 32){
		/*
			return pack('C', $len) 
				. $this->writeStringData($value);
		*/

		zval res[2];
		hessian2_writer_write_string_data(self, value, &res[1]);

		ZVAL_STRING(&param1, "C", 1);
		params[0] = &param1;
		params[1] = &z_len;
		call_user_function(EG(function_table), NULL, &function_name, &res[0], 2, params TSRMLS_DC);
		
		buf_len = Z_STRLEN(res[0]) + Z_STRLEN(res[1]);
		buf = pemalloc(buf_len+1, 0);
		memcpy(buf, Z_STRVAL(res[0]), Z_STRLEN(res[0]));
		p = buf +  Z_STRLEN(res[0]);
		memcpy(p, Z_STRVAL(res[1]), Z_STRLEN(res[1]));
		p[Z_STRLEN(res[1])]= 0;

		//free
		zval_dtor(&function_name);
		zval_dtor(&param1);
		
		RETURN_STRING(buf, 0);
	}else if (Z_LVAL(z_len) < 1024){
		/*
			$b0 = 0x30 + ($len >> 8);
			$stream = pack('C', $b0);
			$stream .= pack('C', $len);
			return $stream . $this->writeStringData($value);
		*/
		zval res[3];
		
		
		ZVAL_STRING(&param1, "C", 1);
		ZVAL_LONG(&param2, 0x30 + (Z_LVAL(z_len) >> 8));
		params[0] = &param1;
		params[1] = &param2;

		call_user_function(EG(function_table), NULL, &function_name, &res[0], 2, params TSRMLS_DC);

		params[1] = &z_len;
		call_user_function(EG(function_table), NULL, &function_name, &res[1], 2, params TSRMLS_DC);

		hessian2_writer_write_string_data(self, value, &res[2]);

		buf_len = Z_STRLEN(res[0]) + Z_STRLEN(res[1]) + Z_STRLEN(res[2]);
		buf = pemalloc(buf_len+1, 0);
		if (!buf){
			php_error_docref(NULL, E_WARNING, "Hessian2Writer::writeString alloc memory error");
			return;
		}
		p = buf;
		for(i=0; i<3; i++){
			memcpy(p, Z_STRVAL(res[i]), Z_STRLEN(res[i]));
			p +=  Z_STRLEN(res[i]);
		}
		*p = 0;



		//free
		zval_dtor(&function_name);
		zval_dtor(&param1);

		
		RETURN_STRING(buf, 0);
	}else{
		/*
			$stream = '';
			$tag = 'S';
			$stream .= $tag . pack('n', $len);
			$stream .= $this->writeStringData($value);
			return $stream;
		*/

		zval res[2];

		ZVAL_STRING(&param1, "n", 1);
		params[0] = &param1;
		params[1] = &z_len;
		call_user_function(EG(function_table), NULL, &function_name, &res[0], 2, params TSRMLS_DC);

		hessian2_writer_write_string_data(self, value, &res[1]);

		buf_len = Z_STRLEN(res[0]) + Z_STRLEN(res[1]);
		buf = pemalloc(buf_len+1, 0);
		if (!buf){
			php_error_docref(NULL, E_WARNING, "Hessian2Writer::writeString alloc memory error");
			return;
		}
		buf[0] = 'S';
		p = buf+1;
		for(i=0; i<2; i++){
			memcpy(p, Z_STRVAL(res[i]), Z_STRLEN(res[i]));
			p +=  Z_STRLEN(res[i]);
		}
		*p = 0;
		

		//free
		zval_dtor(&function_name);
		zval_dtor(&param1);
		
		RETURN_STRING(buf, 0);
	}
}



/*
	Hessian2Writer::writeInt
*/
void hessian2_writer_write_int(zval *self, long value, zval *return_value)
{
	zval function_name, param1, param2;
	zval *params[2];
	char *buf, *p;
	int i;
	long buf_len;
	


	/*
		if($this->between($value, -16, 47)){
			return pack('c', $value + 0x90);
	*/
	INIT_ZVAL(param1);
	INIT_ZVAL(param2);
	ZVAL_STRING(&function_name, "pack", 1);
	if (BETWEEN(value, -16, 47)){
		zval res;
		
		//return pack('c', $value + 0x90);
		ZVAL_STRING(&param1, "c", 1);
		ZVAL_LONG(&param2, value + 0x90);
		params[0] = &param1;
		params[1] = &param2;

		call_user_function(EG(function_table), NULL, &function_name, &res, 2, params TSRMLS_CC);
		zval_dtor(&param1);
		zval_dtor(&function_name);
		
		RETURN_ZVAL(&res, 1, NULL);
	}else if(BETWEEN(value, -2048, 2047)){
		/*
			$b0 = 0xc8 + ($value >> 8);
			$stream = pack('c', $b0);
			$stream .= pack('c', $value);
			return $stream;
		*/

		ulong b0;
		zval res[2];
		
		b0 = 0xc8 + (value >> 8);
		ZVAL_STRING(&param1, "c", 1);
		ZVAL_LONG(&param2, b0);
		params[0] = &param1;
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, &res[0], 2, params TSRMLS_DC);
		
		
		ZVAL_LONG(&param2, value);
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, &res[1], 2, params TSRMLS_DC);


		buf_len = Z_STRLEN(res[0]) + Z_STRLEN(res[1]);
		buf = pemalloc(buf_len , 0);
		if (!buf){
			php_error_docref(NULL, E_WARNING, "Hessian2Writer::writeInt alloc memory error");
			return;
		}
		p = buf;
		for(i=0; i<2; i++){
			memcpy(p, Z_STRVAL(res[i]), Z_STRLEN(res[i]));
			p +=  Z_STRLEN(res[i]);
		}


		zval_dtor(&param1);
		zval_dtor(&function_name);

		
		RETURN_STRINGL(buf, buf_len, 0);
	}else if (BETWEEN(value, -262144, 262143)){
		/*
			$b0 = 0xd4 + ($value >> 16);
			$b1 = $value >> 8;
			$stream = pack('c', $b0);
			$stream .= pack('c', $b1);
			$stream .= pack('c', $value);
			return $stream;
		*/
		

		ulong b0, b1;
		zval res[3];

		b0 = 0xd4 + (value >> 16);
		b1 = value >> 8;
		
		ZVAL_STRING(&param1, "c", 1);
		ZVAL_LONG(&param2, b0);
		params[0] = &param1;
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, &res[0], 2, params TSRMLS_DC);
		
		ZVAL_LONG(&param2, b1);
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, &res[1], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, value);
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, &res[2], 2, params TSRMLS_DC);

		buf_len = Z_STRLEN(res[0]) + Z_STRLEN(res[1]) + Z_STRLEN(res[2]);
		buf = pemalloc(buf_len, 0);
		if (!buf){
			php_error_docref(NULL, E_WARNING, "Hessian2Writer::writeInt alloc memory error");
			return;
		}
		p = buf;
		for(i=0; i<3; i++){
			memcpy(p, Z_STRVAL(res[i]), Z_STRLEN(res[i]));
			p +=  Z_STRLEN(res[i]);
		}
		

		zval_dtor(&param1);
		zval_dtor(&function_name);
		
		RETURN_STRINGL(buf, buf_len, 0);
	}else{
		/*
			$stream = 'I';
			$stream .= pack('c', ($value >> 24));
			$stream .= pack('c', ($value >> 16));
			$stream .= pack('c', ($value >> 8));
			$stream .= pack('c', $value);
			return $stream;
		*/

		zval res[4];

		ZVAL_STRING(&param1, "c", 1);
		ZVAL_LONG(&param2, value >> 24);
		params[0] = &param1;
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, &res[0], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, value >> 16);
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, &res[1], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, value >> 8);
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, &res[2], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, value);
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, &res[3], 2, params TSRMLS_DC);

		buf_len = Z_STRLEN(res[0]) + Z_STRLEN(res[1]) + Z_STRLEN(res[2]) + Z_STRLEN(res[3]);
		buf = pemalloc(buf_len, 0);
		if (!buf){
			php_error_docref(NULL, E_WARNING, "Hessian2Writer::writeInt alloc memory error");
			return;
		}
		p = buf;
		for(i=0; i<4; i++){
			memcpy(p, Z_STRVAL(res[i]), Z_STRLEN(res[i]));
			p +=  Z_STRLEN(res[i]);
		}


		zval_dtor(&param1);
		zval_dtor(&function_name);
		
		RETURN_STRINGL(buf, buf_len, 0);
	}
}


//write bool
void hessian2_writer_write_bool(zval *value, zval *return_value){
	if (i_zend_is_true(value)){
		RETURN_STRING("T", 1);
	}else{
		RETURN_STRING("F", 1);
	}
}



/*
	Hessian2Writer::writeDouble
*/
void hessian2_writer_write_double(zval *self, zval *param_value, zval *return_value)
{
	zval res[5];
	double value, frac;
	zval function_name, param1, param2;
	zval *params[2];
	zend_uchar *buf, *p;
	ulong buf_len;
	int i;
	

	if (Z_TYPE_P(param_value) == IS_DOUBLE){
		value = Z_DVAL_P(param_value);
	}else{
		value = Z_LVAL_P(param_value);
	}


	/*
		$frac = abs($value) - floor(abs($value));
		if($value == 0.0){
			return pack('c', 0x5b);
		}
		if($value == 1.0){
			return pack('c', 0x5c);
		}
	*/

	if  (value < 0){
		frac = -1 * value - floor(-1 * value);
	}else if (value > 0){
		frac = value - floor(value);
	}

	if (value == 0.0){
		ZVAL_STRING(&function_name, "pack", 1);
		ZVAL_STRING(&param1, "c", 1);
		ZVAL_LONG(&param2, 0x5b);
		params[0] = &param1;
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, &res[0], 2, params TSRMLS_DC);

		RETURN_ZVAL(&res[0], 1, NULL);
	}

	if (value == 1.0){
		ZVAL_STRING(&function_name, "pack", 1);
		ZVAL_STRING(&param1, "c", 1);
		ZVAL_LONG(&param2, 0x5c);
		params[0] = &param1;
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, &res[0], 2, params TSRMLS_DC);
		
		RETURN_ZVAL(&res[0], 1, NULL);
	}

	/*
		if($frac == 0 && $this->between($value, -127, 128)){
			return pack('c', 0x5d) . pack('c', $value);
		}
	*/
	if((0 == frac) && BETWEEN(value, -127, 128)){
		ZVAL_STRING(&function_name, "pack", 1);
		ZVAL_STRING(&param1, "c", 1);
		ZVAL_LONG(&param2, 0x5d);
		params[0] = &param1;
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, &res[0], 2, params TSRMLS_DC);

		params[1] = param_value;
		call_user_function(EG(function_table), NULL, &function_name, &res[1], 2, params TSRMLS_DC);


		buf_len = Z_STRLEN(res[0]) + Z_STRLEN(res[1]);
		buf = pemalloc(buf_len, 0);
		if (!buf){
			zend_error(E_ERROR, "Hessian2Writer::writeDouble 1 alloc memory error", 0);
			return;
		}
		memcpy(buf, Z_STRVAL(res[0]), Z_STRLEN(res[0]));
		memcpy(buf+Z_STRLEN(res[0]), Z_STRVAL(res[1]), Z_STRLEN(res[1]));
		RETURN_STRINGL(buf, buf_len, 0);
	}


	/*
		if($frac == 0 && $this->between($value, -32768, 32767)){
			$stream = pack('c', 0x5e);
			$stream .= HessianUtils::floatBytes($value);
			return $stream;
		}
	*/

	if((0 == frac) && BETWEEN(value, -32768, 32767)){
		ZVAL_STRING(&function_name, "pack", 1);
		ZVAL_STRING(&param1, "c", 1);
		ZVAL_LONG(&param2, 0x5e);
		params[0] = &param1;
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, &res[0], 2, params TSRMLS_DC);

		ZVAL_STRING(&function_name, "HessianUtils::floatBytes", 1);
		ZVAL_DOUBLE(&param1, value);
		params[0] = &param1;
		call_user_function(EG(function_table), NULL, &function_name, &res[1], 1, params TSRMLS_DC);

		buf_len = 0;
		for(i=0; i<2; i++){
			buf_len += Z_STRLEN(res[i]);
		}
		buf = pemalloc(buf_len, 0);
		if (!buf){
			zend_error(E_ERROR, "Hessian2Writer::writeDouble alloc memory error", 0);
			return;
		}
		p = buf;
		for(i=0; i<2; i++){
			memcpy(p, Z_STRVAL(res[i]), Z_STRLEN(res[i]));
			p +=  Z_STRLEN(res[i]);
		}

		RETURN_STRINGL(buf, buf_len, 0);
	}


	/*
		$mills = (int) ($value * 1000);
	    if (0.001 * $mills == $value) {
	    	$stream = pack('c', 0x5f);
	      	$stream .= pack('c', $mills >> 24);
	      	$stream .= pack('c', $mills >> 16);
	      	$stream .= pack('c', $mills >> 8);
	      	$stream .= pack('c', $mills);
			return $stream;
	    }
	*/

	int mills;

	mills = value * 1000;

	if (0.001  * mills == value){
		ZVAL_STRING(&function_name, "pack", 1);
		ZVAL_STRING(&param1, "c", 1);
		ZVAL_LONG(&param2, 0x5f);
		params[0] = &param1;
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, &res[0], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, mills >> 24);
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, &res[1], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, mills >> 16);
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, &res[2], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, mills >> 8);
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, &res[3], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, mills);
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, &res[4], 2, params TSRMLS_DC);

		buf_len = 0;
		for(i=0; i<5; i++){
			buf_len += Z_STRLEN(res[i]);
		}

		
		buf = pemalloc(buf_len, 0);
		if (!buf){
			zend_error(E_ERROR, "Hessian2Writer::writeDouble alloc memory error", 0);
			return;
		}
		p = buf;
		for(i=0; i<5; i++){
			memcpy(p, Z_STRVAL(res[i]), Z_STRLEN(res[i]));
			p +=  Z_STRLEN(res[i]);
		}

		RETURN_STRINGL(buf, buf_len, 0);
	}


	/*
		$stream = 'D';
		$stream .= HessianUtils::doubleBytes($value);
		return $stream;
	*/

	ZVAL_STRING(&function_name, "HessianUtils::doubleBytes", 1);
	params[0] = param_value;
	call_user_function(EG(function_table), NULL, &function_name, &res[0], 1, params TSRMLS_DC);

	buf_len = 1 + Z_STRLEN(res[0]);
	buf = pemalloc(buf_len + 1, 0);
	buf[0] = 'D';
	memcpy(buf+1, Z_STRVAL(res[0]), Z_STRLEN(res[0]));
	
	RETURN_STRINGL(buf, buf_len, 0);
}



/*
	Hessian2Writer::writeArray
*/
void hessian2_writer_write_array(zval *self, zval *array, zval *return_value)
{
	zval *ref_map, ref_index;
	zval function_name;
	zval *params[2];
	zval param1, param2;
	

	/*
		if(empty($array))
			return 'N';
	*/
	
	if (!i_zend_is_true(array)){
		RETURN_STRING("N", 1);
	}

	/*
		$refindex = $this->refmap->getReference($array);
		if($refindex !== false){
			return $this->writeReference($refindex);
		}
	*/
	ref_map = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_CC);

	ZVAL_STRING(&function_name, "getReference", 1);
	params[0] = array;
	hessian_call_class_function_helper(ref_map, &function_name, 1, params, &ref_index);
	zval_dtor(&function_name);
	
	if (!(Z_TYPE(ref_index) == IS_BOOL && Z_BVAL(ref_index) <= 0)){
		zval res;
		ZVAL_STRING(&function_name, "writeReference", 1);
		params[0] = &ref_index;
		hessian_call_class_function_helper(ref_map, &function_name, 1, params, &res);
		zval_dtor(&function_name);
		RETURN_ZVAL(&res, 1, NULL);
	}


	/*
		$total = count($array);		
		if(HessianUtils::isListFormula($array)){
			$this->refmap->objectlist[] = &$array;
			$stream = '';
			if($total <= 7){
				$len = $total + 0x78;
				$stream = pack('c', $len);
			} else {
				
			}
			foreach($array as $key => $value){
				$stream .= $this->writeValue($value); 
			}
			return $stream;
		} else{
			return $this->writeMap($array);
		}
	*/

	int total;
	zval call_res;
	
	total = zend_hash_num_elements(Z_ARRVAL_P(array));
	ZVAL_STRING(&function_name, "HessianUtils::isListFormula", 1);
	params[0] = array;
	call_user_function(EG(function_table), NULL, &function_name, &call_res, 1, params TSRMLS_DC);
	zval_dtor(&function_name);

	if (i_zend_is_true(&call_res)){
		zval *object_list;
		char *buf;
		
		ulong old_buf_len=0, buf_len=0;

		object_list = zend_read_property(NULL, ref_map, ZEND_STRL("objectlist"), 1 TSRMLS_CC);
		if (Z_TYPE_P(object_list ) != IS_ARRAY){
			array_init_size(object_list, 1);
			zend_update_property(NULL, ref_map,  ZEND_STRL("objectlist"), object_list TSRMLS_CC);
			object_list = zend_read_property(NULL, ref_map, ZEND_STRL("objectlist"), 1 TSRMLS_CC);
		}
		//todo:for reference
		zend_hash_next_index_insert(Z_ARRVAL_P(object_list), &array, sizeof(zval *), NULL);

		INIT_ZVAL(param1);
		INIT_ZVAL(param2);
		ZVAL_STRING(&function_name, "pack", 1);
		ZVAL_STRING(&param1, "c", 1);
		if (total <= 7){
			int len;

			
			len = total + 0x78;
			ZVAL_LONG(&param2, len);
			params[0] = &param1;
			params[1] = &param2;
			call_user_function(EG(function_table), NULL, &function_name, &call_res, 2, params TSRMLS_DC);
			buf = Z_STRVAL(call_res);
			buf_len = Z_STRLEN(call_res);
		}else{
			/*
				$stream = pack('c', 0x58);
				$stream .= $this->writeInt($total);
			*/
			zval write_int_res;
			ZVAL_LONG(&param2, 0x58);
			params[0] = &param1;
			params[1] = &param2;
			call_user_function(EG(function_table), NULL, &function_name, &call_res, 2, params TSRMLS_DC);
			buf = Z_STRVAL(call_res);
			buf_len = Z_STRLEN(call_res);

			hessian2_writer_write_int(self, total, &write_int_res);

			buf_len += Z_STRLEN(write_int_res);
			perealloc(buf, buf_len, 0);
		}
		zval_dtor(&function_name);

		

		/*
			foreach($array as $key => $value){
				$stream .= $this->writeValue($value); 
			}
			return $stream;
		*/
		zval *zval_ptr, **src_entry;
		int i=0;
		HashPosition pos;

		old_buf_len = buf_len;
		zval_ptr = pemalloc(sizeof(zval)*total, 0);
		if (!zval_ptr){
			php_error_docref(NULL, E_ERROR, "Hessian2Writer::writeArray alloc memory error");
			return;
		}

		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(array), &pos);
		while (zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void **)&src_entry, &pos) == SUCCESS) {
			hessian2_writer_write_value(self, *src_entry, &zval_ptr[i]);
			buf_len += Z_STRLEN(zval_ptr[i]);
			++i;
			zend_hash_move_forward_ex(Z_ARRVAL_P(array), &pos);
		}

		char *new_buf, *p;
		new_buf = pemalloc(buf_len+1,  0);
		if (!new_buf){
			php_error_docref(NULL, E_ERROR, "Hessian2Writer::writeArray alloc memory error");
			return;
		}

		p = new_buf;
		memcpy(p, buf, old_buf_len);
		p += old_buf_len;
		zval_dtor(&call_res);
		//pefree(buf, 0);
		for(i=0; i<total; i++){
			memcpy(p, Z_STRVAL(zval_ptr[i]), Z_STRLEN(zval_ptr[i]));
			p += Z_STRLEN(zval_ptr[i]);
			zval_dtor(&zval_ptr[i]);
		}
		pefree(zval_ptr, 0);
		*p = 0;

		RETURN_STRINGL(new_buf, buf_len, 0);
	}else{
		//return $this->writeMap($array);
		zval *res;

		ZVAL_STRING(&function_name, "writeMap", 1);
		params[0] = array;
		hessian_call_class_function_helper(self, &function_name, 1, params);
		//call_user_function(NULL, &self, &function_name, res, 1, params TSRMLS_DC);

		RETURN_ZVAL(res, 1, NULL);
	}
}



/*
	Hessian2Writer::writeObject
*/
void hessian2_writer_write_object(zval *self, zval *value, zval *return_value)
{
	zval *ref_map,ref_index;
	zval function_name;
	zval *params[2];
	zval res;
	

	/*
		$refindex = $this->refmap->getReference($value);
		if($refindex !== false){
			return $this->writeReference($refindex);
		}
		return $this->writeObjectData($value);
	*/

	ref_map = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_CC);
	ZVAL_STRING(&function_name, "getReference", 1);
	params[0] = value;
	hessian_call_class_function_helper(ref_map, &function_name, 1, params, &ref_index);
	zval_dtor(&function_name);

	if (!(Z_TYPE(ref_index) == IS_BOOL && Z_BVAL(ref_index) < 1)){
		
		ZVAL_STRING(&function_name, "writeReference", 1);
		params[0] = &ref_index;
		hessian_call_class_function_helper(self, &function_name, 1, params, &res);
		zval_dtor(&function_name);
		
		RETURN_ZVAL(&res, 1, NULL);
	}

	ZVAL_STRING(&function_name, "writeObjectData", 1);
	params[0] = value;
	//@todo: direct call
	hessian_call_class_function_helper(self, &function_name, 1, params, &res);
	zval_dtor(&function_name);
	
	RETURN_STRINGL(Z_STRVAL(res), Z_STRLEN(res), 1);
}


/*
	Hessian2Writer::writeNull
*/
void hessian2_writer_write_null(zval *return_value)
{
	RETURN_STRING("N", 1);
}


/*
	Hessian2Writer::writeResource
*/
void hessian2_writer_write_resource(zval *self, zval *handle, zval *return_value)
{
	zval  type, res[3];
	zval function_name, param1, param2;
	zval *params[2];
	zend_uchar *buf, *p;
	int old_buf_len = 0;
	int total_buf_len = 0;
	


	//$type = get_resource_type($handle);
	params[0] = handle;
	ZVAL_STRING(&function_name, "get_resource_type", 1);
	call_user_function(EG(function_table), NULL, &function_name, &type, 1, params TSRMLS_DC);
	zval_dtor(&function_name);
	

	/*
		if($type == 'file' || $type == 'stream'){
			while (!feof($handle)) {
				$content = fread($handle, 32768);
				$len = count(str_split($content));
				if($len < 15){ // short binary
					
				} else {
					$tag = 'b';
					if(feof($handle))
						$tag = 'B';
					$stream .= $tag . pack('n', $len);
					$stream .= $content;
				}
			}
			fclose($handle);
		} else {
			throw new Exception("Cannot handle resource of type '$type'");	
		}
	*/

	buf = Z_STRVAL(type);
	if ( (strncmp(buf, "file", 4) == 0) 
		|| (strncmp(buf, "stream", 6) == 0) ){
		zval content, function_name_fread, function_name_pack;
		ZVAL_STRING(&function_name, "feof", 1);
		ZVAL_STRING(&function_name_fread, "fread", 1);
		ZVAL_STRING(&function_name_pack, "pack", 1);

		
		params[0] = handle;
		call_user_function(EG(function_table), NULL, &function_name, &res[0], 1, params TSRMLS_DC);
		buf = NULL;
		
		while(!i_zend_is_true(&res[0])){
			//read content
			ZVAL_LONG(&param1, 32768);
			params[0] = handle;
			params[1] = &param1;
			call_user_function(EG(function_table), NULL, &function_name_fread, &content, 2, params TSRMLS_DC);


			//
			if (Z_STRLEN(content) < 15){
				/*
					$stream .= pack('C', $len + 0x20);
					$stream .= $content;
				*/
				ZVAL_STRING(&param1, "C", 1);
				ZVAL_LONG(&param2, Z_STRLEN(content) + 0x20);
				params[0] = &param1;
				params[1] = &param2;

				total_buf_len += Z_STRLEN(res[1]) + Z_STRLEN(content);
				call_user_function(EG(function_table), NULL, &function_name_pack, &res[1], 2, params TSRMLS_DC);
				p = pemalloc(total_buf_len, 0);
				
				if (old_buf_len > 0){
					memcpy(p, buf, old_buf_len);
					pefree(buf, 0);
				}
				buf = p;
				memcpy(p+old_buf_len, Z_STRVAL(content), Z_STRLEN(content));
				old_buf_len = total_buf_len;
			}else{
				/*
					$tag = 'b';
					if(feof($handle))
						$tag = 'B';
					$stream .= $tag . pack('n', $len);
					$stream .= $content;
				*/

				
				//test if eof
				params[0] = handle;
				call_user_function(EG(function_table), NULL, &function_name, &res[1], 1, params TSRMLS_DC);

				//pack length
				ZVAL_STRING(&param1, "n", 1);
				ZVAL_LONG(&param2, Z_STRLEN(content));
				params[0] = &param1;
				params[1] = &param2;
				call_user_function(EG(function_table), NULL, &function_name_pack, &res[2], 2, params TSRMLS_DC);
		
				//new buf len
				total_buf_len += 1 + Z_STRLEN(res[2]) + Z_STRLEN(content);
				p = pemalloc(total_buf_len, 0);
				if (old_buf_len > 0){
					memcpy(p, buf, old_buf_len);
					pefree(buf, 0);
				}
				buf = p;
				p += old_buf_len;
				if (i_zend_is_true(&res[1])){
					*p = 'B';
					++p;
					memcpy(p, Z_STRVAL(res[2]), Z_STRLEN(res[2]));
					p += Z_STRLEN(res[2]);
					memcpy(p, Z_STRVAL(content), Z_STRLEN(content));
				}else{
					*p = 'b';
					++p;
					memcpy(p, Z_STRVAL(res[2]), Z_STRLEN(res[2]));
					p += Z_STRLEN(res[2]);
					memcpy(p, Z_STRVAL(content), Z_STRLEN(content));
				}

				old_buf_len = total_buf_len;
			}

			params[0] = handle;
			call_user_function(EG(function_table), NULL, &function_name, &res[0], 1, params TSRMLS_DC);
		}


		ZVAL_STRING(&param1, "fclose", 1);
		params[0] = handle;
		call_user_function(EG(function_table), NULL, &function_name, &res[0], 1, params TSRMLS_DC);
		
		RETURN_STRING(buf, 0);
	}else{
		zend_class_entry **ce_exception;
		
		zend_hash_find(CG(class_table), "exception", sizeof("exception"), (void **) &ce_exception);
		zend_throw_exception(ce_exception, sprintf("Cannot handle resource of type '%s'", Z_STRVAL(type))
			, 0 TSRMLS_DC);
	}
}



//end for api functions




//
void hessian2_writer_resolve_dispatch(zval *self, zval *type, zval *return_value){
	char *buf;

	
	buf = Z_STRVAL_P(type);
	if (strncmp(buf, "integer", strlen("integer")-1) == 0){
		RETURN_STRING("writeInt", 1);
	}else if (strncmp(buf, "boolean", strlen("boolean")-1) == 0){
		RETURN_STRING("writeBool", 1);
	}else if (strncmp(buf, "string", strlen("string")-1) == 0){
		RETURN_STRING("writeString", 1);
	}else if (strncmp(buf, "double", strlen("double")-1) == 0){
		RETURN_STRING("writeDouble", 1);
	}else if (strncmp(buf, "array", strlen("array")-1) == 0){
		RETURN_STRING("writeArray", 1);
	}else if (strncmp(buf, "object", strlen("object")-1) == 0){
		RETURN_STRING("writeObject", 1);
	}else if (strncmp(buf, "NULL", strlen("NULL")-1) == 0){
		RETURN_STRING("writeNull", 1);
	}else if (strncmp(buf, "resource", strlen("resource")-1) == 0){
		RETURN_STRING("writeResource", 1);
	}else{
		//throw new Exception("Handler for type $type not implemented");
		zend_class_entry **ce_exception;
		zend_hash_find(CG(class_table), "exception", sizeof("exception"), (void **) &ce_exception);
		zend_throw_exception(*ce_exception, sprintf("Handler for type %s not implemented", Z_STRVAL_P(type)), 0 TSRMLS_CC);
	}

	//$this->logMsg("dispatch $dispatch");

	/*
	zval function_name, *param1;
	zval *params[1];
	char err_buf[256];


	//todo:call method adjust

	
	sprintf(err_buf, "dispatch %s", dispatch);
	ALLOC_ZVAL(param1);
	ZVAL_STRING(param1, err_buf, 1);
	ZVAL_STRING(&function_name, "logMsg", 1);
	params[0] = param1;
	hessian_call_class_function_helper(self, &function_name, 1, params, &retval);
	zval_dtor(&function_name);
	*/


	//RETURN_STRINGL(dispatch, len, 1);
}






/*
	Hessian2Writer::__construct
*/
static PHP_METHOD(Hessian2Writer, __construct)
{
	zval *self, *options;
	zval *ref_map, *type_map, *log_msg;
	zval function_name;
	zval *params[1];
	zval *arr;
	zval ret;
	

	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &options)) {
		return;
	}


	ALLOC_ZVAL(arr);
	array_init_size(arr, 0);


	ALLOC_ZVAL(ref_map);
	object_init_ex(ref_map, hessian_reference_map_entry);
	zend_update_property(NULL, self, ZEND_STRL("refmap"), ref_map TSRMLS_CC);

	ALLOC_ZVAL(type_map);
	object_init_ex(type_map, hessian_type_map_entry);
	ZVAL_STRING(&function_name, "__construct", 1);
	Z_ADDREF_P(arr);
	params[0] = arr;
	hessian_call_class_function_helper(type_map, &function_name, 1, params, &ret);
	zval_dtor(&function_name);
	zend_update_property(NULL, self, ZEND_STRL("typemap"), type_map TSRMLS_CC);

	zend_update_property(NULL, self, ZEND_STRL("options"), options TSRMLS_CC);


	//ALLOC_ZVAL(log_msg);
	//array_init_size(log_msg, 1);
	//zend_update_property(NULL, self, ZEND_STRL("logMsg"), log_msg TSRMLS_CC);


	//FREE_ZVAL(arr);
	/*
		$this->refmap = new HessianReferenceMap();
		$this->typemap = new HessianTypeMap();
		$this->options = $options;
	*/

}


/*
	Hessian2Writer::logMsg
*/
static PHP_METHOD(Hessian2Writer, logMsg)
{
	zval *self, *msg;
	zval *log_msg;
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &msg)) {
		return;
	}
	if (Z_TYPE_P(msg) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "msg must be a string");
		return;
	}

	log_msg = zend_read_property(NULL, self, ZEND_STRL("logMsg"), 1 TSRMLS_DC);
	zend_hash_next_index_insert(Z_ARRVAL_P(log_msg), &msg, sizeof(zval **), NULL);
	//zend_update_property(NULL, self, ZEND_STRL("logMsg"), log_msg TSRMLS_CC);
}


/*
	Hessian2Writer::setTypeMap
*/
void hessian2_writer_set_type_map(zval *self, zval *type_map)
{
	zend_update_property(NULL, self, ZEND_STRL("typemap"), type_map TSRMLS_CC);
}


/*
	Hessian2Writer::setTypeMap
*/
static PHP_METHOD(Hessian2Writer, setTypeMap)
{
	zval *self, *type_map;
	zval *log_msg;
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &type_map)) {
		return;
	}
	hessian2_writer_set_type_map(self, type_map);
}

/*
	Hessian2Writer::setFilters
*/
static PHP_METHOD(Hessian2Writer, setFilters)
{
	zval *self, *container;

	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &container)) {
		return;
	}
	zend_update_property(NULL, self, ZEND_STRL("filterContainer"), container TSRMLS_CC);
}



/*
	Hessian2Writer::writeValue
*/
void hessian2_writer_write_value(zval *self, zval *value, zval *return_value)
{
	zval type, dispatch;
	zval function_name;
	zval *params[2];
	zend_uchar *buf;


	/*
		$type = gettype($value);
		$dispatch = $this->resolveDispatch($type);
	*/
	
	//Z_ADDREF_P(value);
	params[0] = value;
	ZVAL_STRING(&function_name, "gettype", 1);
	call_user_function(EG(function_table), NULL, &function_name, &type, 1, params TSRMLS_CC);
	zval_dtor(&function_name);
	//Z_DELREF_P(value);

	hessian2_writer_resolve_dispatch(self, &type, &dispatch);


	/*
		if(is_object($value)){
			$filter = $this->filterContainer->getCallback($value);
			if($filter) {
				
			}
		}
		$data = $this->$dispatch($value);
		return $data;	
	*/


	/*
	if (Z_TYPE_P(value)== IS_OBJECT){
		zval *filter_container,  *filter;

		ALLOC_ZVAL(filter);
		filter_container = zend_read_property(NULL, self, ZEND_STRL("filterContainer"), 1 TSRMLS_DC);
		ZVAL_STRING(&function_name, "getCallback", 1);
		params[0] = value;
		hessian_call_class_function_helper(filter_container, &function_name, 1, params, filter);

		if (i_zend_is_true(filter)){

			zval arr, res;
			
			array_init_size(&arr, 2);
			zend_hash_next_index_insert(Z_ARRVAL(arr), &value, sizeof(zval *), NULL);
			zend_hash_next_index_insert(Z_ARRVAL(arr), &self, sizeof(zval *), NULL);

			ZVAL_STRING(&function_name, "doCallback", 1);
			params[0] = filter;
			params[1] = &arr;
			hessian_call_class_function_helper(filter_container, &function_name, 2, params, &res);

			//call_user_function(NULL, &filter_container, &function_name, res, 2, params TSRMLS_DC);
			if ((IS_OBJECT == Z_TYPE(res))
					&& instanceof_function(Z_OBJCE(res), hessian_stream_result_entry TSRMLS_DC)){
				zval *stream;
				
				stream = zend_read_property(NULL, &res, ZEND_STRL("stream"), 1 TSRMLS_DC);
				FREE_ZVAL(filter);
				RETURN_ZVAL(stream, 1, NULL);
			}

			zval ntype;
			ZVAL_STRING(&function_name, "gettype", 1);
			params[0] = value;
			call_user_function(EG(function_table), NULL, &function_name, &ntype, 1, params TSRMLS_DC);

			if (hessian2_writer_compare(&type, &ntype) == 0){
				ZVAL_STRING(&function_name, "resolveDispatch", 1);
				hessian2_writer_resolve_dispatch(self, &ntype, &ntype);
				//params[0] = &ntype;
				//hessian_call_class_function_helper(self, &function_name, 1, params, &ntype);
			}
		}
		FREE_ZVAL(filter);
	}
	*/

	//$data = $this->$dispatch($value);
	zval  data;

	buf = Z_STRVAL(dispatch);
	if(strncmp(buf, "writeInt", strlen("writeInt")) == 0){
		hessian2_writer_write_int(self, Z_LVAL_P(value), &data);
	}else if (strncmp(buf, "writeBool", strlen("writeBool")) == 0){
		hessian2_writer_write_bool(value, &data);
	}else if (strncmp(buf, "writeString", strlen("writeString")) == 0){
		hessian2_writer_write_string(self, value, &data);
	}else if (strncmp(buf, "writeDouble", strlen("writeDouble")) == 0){
		hessian2_writer_write_double(self, value, &data);
	}else if (strncmp(buf, "writeArray", strlen("writeArray")) == 0){
		hessian2_writer_write_array(self, value, &data);
	}else if (strncmp(buf, "writeObject", strlen("writeObject")) == 0){
		hessian2_writer_write_object(self, value, &data);
	}else if (strncmp(buf, "writeNull", strlen("writeNull")) == 0){
		hessian2_writer_write_null(&data);
	}else if (strncmp(buf, "writeResource", strlen("writeResource")) == 0){
		hessian2_writer_write_resource(self, value,  &data);
	}
	

	/*
	params[0] = value;
	ZVAL_STRING(&function_name, Z_STRVAL_P(dispatch), 1);
	hessian_call_class_function_helper(self, &function_name, 1, params, &data);
	*/

	//zval_dtor(&dispatch);

	RETURN_STRINGL(Z_STRVAL(data), Z_STRLEN(data), 1);
}




/*
	Hessian2Writer::writeValue
*/
static PHP_METHOD(Hessian2Writer, writeValue)
{
	zval *self, *value;
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value)) {
		return;
	}

	hessian2_writer_write_value(self, value, return_value);
}



/*
	Hessian2Writer::resolveDispatch
*/
static PHP_METHOD(Hessian2Writer, resolveDispatch)
{
	zval *self, *type;
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &type)) {
		return;
	}
	if ((Z_TYPE_P(type) !=  IS_STRING) || Z_STRLEN_P(type) < 1){
		php_error_docref(NULL,E_WARNING, "type must be a string");
		return;
	}

	hessian2_writer_resolve_dispatch(self, type, return_value);
}

/*
	Hessian2Writer::writeNull
*/
static PHP_METHOD(Hessian2Writer, writeNull)
{
	hessian2_writer_write_null(return_value);
}

/*
	Hessian2Writer::writeArray
*/
static PHP_METHOD(Hessian2Writer, writeArray)
{
	zval *self, *array;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &array)) {
		return;
	}
	if(Z_TYPE_P(array) != IS_ARRAY){
		php_error_docref(NULL, E_WARNING, "array type must be array");
		return;
	}
	self = getThis();

	hessian2_writer_write_array(self, array, return_value);
}


/*
	Hessian2Writer::writeMap
*/
static PHP_METHOD(Hessian2Writer, writeMap)
{
	zval *self, *map, *type;
	zval *ref_map, *ref_index, *object_list;
	zval function_name;
	zval *params[2];
	char *buf;
	ulong buf_len, old_buf_len;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &map, &type)) {
		return;
	}

	if (!i_zend_is_true(map)){
		RETURN_STRING("N", 1);
	}

	/*
		$refindex = $this->refmap->getReference($map);
		if($refindex !== false){
			return $this->writeReference($refindex);
		}
	*/

	self = getThis();
	ref_map = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_CC);
	ALLOC_ZVAL(ref_index);
	ZVAL_STRING(&function_name, "getReference", 1);
	params[0] = map;
	hessian_call_class_function_helper(ref_map, &function_name, 1, params, ref_index);
	zval_dtor(&function_name);

	
	if (!(Z_TYPE_P(ref_index) == IS_BOOL && Z_BVAL_P(ref_index) <= 0)){
		zval res;
		ZVAL_STRING(&function_name, "writeReference", 1);
		params[0] = ref_index;
		hessian_call_class_function_helper(ref_map, &function_name, 1, params, res);
		zval_dtor(&function_name);
		
		RETURN_ZVAL(&res, 1, NULL);
	}

	//$this->refmap->objectlist[] = &$map;
	object_list = zend_read_property(NULL, ref_map, ZEND_STRL("objectlist"), 1 TSRMLS_CC);
	if (Z_TYPE_P(object_list ) != IS_ARRAY){
		array_init_size(object_list, 1);
		zend_update_property(NULL, ref_map, ZEND_STRL("objectlist"), object_list TSRMLS_CC);
	}
	zend_hash_next_index_insert(Z_ARRVAL_P(object_list), &map, sizeof(zval **), NULL);

	/*
		if($type == '') {
			$stream = 'H';
		} else{
			$stream = 'M';
			$stream .= $this->writeType($type);
		}
	*/

	
	if ((Z_TYPE_P(type) == IS_STRING)&&(0 == Z_STRLEN_P(type))){
		buf = pemalloc(1, 0);
		buf[0] = 'H';
		old_buf_len = 1;
	}else{
		zval call_res;
		char *tmp_buf;
		
		ZVAL_STRING(&function_name, "writeType", 1);
		params[0] = type;
		hessian_call_class_function_helper(self, &function_name, 1, params, &call_res);
		zval_dtor(&function_name);
		
		buf = pemalloc(Z_STRLEN(call_res) + 1, 0);
		buf[0] = 'M';
		memcpy(buf+1, Z_STRVAL(call_res), Z_STRLEN(call_res));
		old_buf_len = Z_STRLEN(call_res) + 1;
	}

	/*
		foreach($map as $key => $value){
			$stream .= $this->writeValue($key);
			$stream .= $this->writeValue($value);
		}
		$stream .= 'Z';
	*/

	HashPosition pos;
	zval **src_entry;
	char *string_key, *new_buf, *p;
	uint string_key_len;
	ulong num_key;
	int total;
	zval *zval_ptr;
	zval *param1;

	buf_len = old_buf_len;
	total = zend_hash_num_elements(Z_ARRVAL_P(map));

	zval_ptr = (zval  *)pemalloc((total * 2) * (sizeof(zval)) , 0);
	if (!zval_ptr){
		php_error_docref(NULL, E_ERROR, "Hessian2Writer::writeMap alloc mem error");
		return;
	}

	
	int i = 0 ;
	ALLOC_ZVAL(param1);
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(map), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(map), (void **)&src_entry, &pos) == SUCCESS) {
		zend_hash_get_current_key_ex(Z_ARRVAL_P(map), &string_key, &string_key_len, &num_key, 0, &pos);
		ZVAL_STRINGL(param1, string_key, string_key_len-1, 1);
		hessian2_writer_write_value(self, param1, &zval_ptr[i]);


		buf_len += Z_STRLEN(zval_ptr[i]);
		i++;

		hessian2_writer_write_value(self, *src_entry, &zval_ptr[i]);
		buf_len += Z_STRLEN(zval_ptr[i]);
		i++;
		
		zend_hash_move_forward_ex(Z_ARRVAL_P(map), &pos);
	}
	//zval_dtor(&function_name);
	FREE_ZVAL(param1);


	//for Z
	buf_len += 1;

	//cpy
	new_buf = pemalloc(buf_len, 0);
	if (!new_buf){
		php_error_docref(NULL, E_ERROR, "Hessian2Writer::writeMap alloc memory error");
		return;
	}

	p = new_buf;
	memcpy(p, buf, old_buf_len);
	p += old_buf_len;
	
	for(i=0; i<total*2; i++){
		memcpy(p, Z_STRVAL(zval_ptr[i]), Z_STRLEN(zval_ptr[i]));
		p +=  Z_STRLEN(zval_ptr[i]);
	}
	*p = 'Z';

	for(i=0; i<total*2; i++){
		zval_dtor(&zval_ptr[i]);
	}
	pefree(zval_ptr, 0);
	pefree(buf, 0);

	RETURN_STRINGL(new_buf, buf_len, 0);
}


/*
	Hessian2Writer::writeObjectData
*/
static PHP_METHOD(Hessian2Writer, writeObjectData)
{
	zval *self, *value;
	zval *ref_map, index, *class_def, *z_class, type;
	zval function_name, param1;
	zval *params[2];
	int total, i;
	zval  *zval_ptr;
	ulong buf_len = 0, old_buf_len;
	char *buf, *p, *new_buf;
	HashPosition pos;
	zval **src_entry, *props;

	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value)) {
		return;
	}


	/*
		$stream = '';
		$class = get_class($value);
		$index = $this->refmap->getClassIndex($class);
	*/

	
	ALLOC_ZVAL(z_class);
	ZVAL_STRING(&function_name, "get_class", 1);
	Z_ADDREF_P(value);
	params[0] = value;
	call_user_function(EG(function_table), NULL, &function_name, z_class, 1, params TSRMLS_CC);
	zval_dtor(&function_name);
	//zend_get_object_classname(value, (const char**)&name, &name_len TSRMLS_CC);


	ref_map = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_CC);
	ZVAL_STRING(&function_name, "getClassIndex", 1);
	params[0] = z_class;
	hessian_call_class_function_helper(ref_map, &function_name, 1, params,  &index);
	zval_dtor(&function_name);



	if (Z_TYPE(index) == IS_BOOL && Z_BVAL(index) < 1){
		zval target, *props;
		/*
			$classdef = new HessianClassDef();
			$classdef->type = $class;
			if($class == 'stdClass'){
				$classdef->props = array_keys(get_object_vars($value));
			} else
				$classdef->props = array_keys(get_class_vars($class));
		*/

		
		ALLOC_ZVAL(class_def);
		object_init_ex(class_def, hessian_class_def_entry);
		zend_update_property(NULL, class_def, ZEND_STRL("type"), z_class TSRMLS_CC);

		if (strncmp(Z_STRVAL_P(z_class), "stdClass", 8) == 0){
			params[0] = value;
			//Z_ADDREF_P(value);
			ZVAL_STRING(&function_name, "get_object_vars", 1);
			call_user_function(EG(function_table), NULL, &function_name, &target, 1, params TSRMLS_CC);
			zval_dtor(&function_name);
		
		}else{
			params[0] = z_class;
			//Z_ADDREF_P(z_class);
			//params[1] = value;
			//Z_ADDREF_P(value);
			ZVAL_STRING(&function_name, "get_class_vars", 1);
			call_user_function(EG(function_table), NULL, &function_name, &target, 1, params TSRMLS_CC);
			zval_dtor(&function_name);
		}

		ALLOC_ZVAL(props);
		ZVAL_STRING(&function_name, "array_keys", 1);
		params[0] = &target;
		call_user_function(EG(function_table), NULL, &function_name, props, 1, params TSRMLS_DC);
		zval_dtor(&function_name);

		zend_update_property(NULL, class_def, ZEND_STRL("props"), props TSRMLS_CC);

		

		/*
			$index = $this->refmap->addClassDef($classdef);
			$total = count($classdef->props);
		*/

		hessian_reference_map_add_class_def(ref_map, class_def, &index);


		/*
			$total = count($classdef->props);
			
			$type = $this->typemap->getRemoteType($class);
			$class = $type ? $type : $class;
		*/
		total = zend_hash_num_elements(Z_ARRVAL_P(props));

		zval *type_map;

		type_map = zend_read_property(NULL, self, ZEND_STRL("typemap"), 1 TSRMLS_CC);
		hessian_type_map_get_remote_type(type_map, z_class, &type);
		
		if (i_zend_is_true(&type)){
			z_class = &type;
		}

		/*
			$stream .= 'C';
			$stream .= $this->writeString($class);
			$stream .= $this->writeInt($total);
			foreach($classdef->props as $name){
				$stream .= $this->writeString($name);
			}
		*/
		
		zval_ptr = (zval *)pemalloc(sizeof(zval) * (zend_hash_num_elements(Z_ARRVAL_P(props)) + 2), 0);
		if (!zval_ptr){
			php_error_docref(NULL, E_ERROR, "Hessian2Writer::writeObjectData alloc memorty error");
			return;
		}
		i = 0;
		hessian2_writer_write_string(self, z_class, &zval_ptr[i]);
		++i;

		hessian2_writer_write_int(self, total, &zval_ptr[i]);
		++i;
		
		buf_len = 1;
		buf_len += Z_STRLEN_P(&zval_ptr[0]);
		buf_len += Z_STRLEN_P(&zval_ptr[1]);
		
		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(props), &pos);
		while (zend_hash_get_current_data_ex(Z_ARRVAL_P(props), (void **)&src_entry, &pos) == SUCCESS) {
			//Z_ADDREF_PP(src_entry);
			hessian2_writer_write_string(self, *src_entry, &zval_ptr[i]);
			buf_len += Z_STRLEN(zval_ptr[i]);
			++i;
			zend_hash_move_forward_ex(Z_ARRVAL_P(props), &pos);
		}

		buf = pemalloc(buf_len, 0);
		if (!buf){
			php_error_docref(NULL, E_ERROR, "Hessian2Writer::writeObjectData alloc memorty error");
			return;
		}
		p = buf;
		p[0] = 'C';
		++p;
		ulong zval_count;

		zval_count = i;
		for(i=0; i<zval_count; i++){
			memcpy(p, Z_STRVAL(zval_ptr[i]),  Z_STRLEN(zval_ptr[i]));
			p +=  Z_STRLEN(zval_ptr[i]);

			zval_dtor(&zval_ptr[i]);
		}
		pefree(zval_ptr, 0);
	}else{

		/*
		zval_ptr = pemalloc(zend_hash_num_elements(Z_ARRVAL_P(props)) * 2 + 1, 0);
		if (!zval_ptr){
			php_error_docref(NULL, E_ERROR, "Hessian2Writer::writeObjectData alloc memorty error");
			return;
		}
		*/
		
	}


	/*
		if($index < 16){
			$stream .= pack('c', $index + 0x60);
		} else{
			$stream .= 'O';
			$stream .= $this->writeInt($index);
		}
	*/

	if (Z_LVAL(index) < 16){
		zval param2, pack_res;
		zval *param_c;

		ALLOC_ZVAL(param_c);
		ZVAL_STRING(&function_name, "pack", 1);
		ZVAL_STRING(param_c, "c", 1);
		ZVAL_LONG(&param2, Z_LVAL(index) + 0x60);
		params[0] = param_c;
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, &pack_res, 2, params TSRMLS_CC);
		zval_dtor(&function_name);
		FREE_ZVAL(param_c);
		
		if (buf_len > 0){
			new_buf  = pemalloc(buf_len + Z_STRLEN(pack_res), 0);
			memcpy(new_buf,  buf, buf_len);
			memcpy(new_buf + buf_len,  Z_STRVAL(pack_res),  Z_STRLEN(pack_res));
			pefree(buf, 0);
			buf = new_buf;
			buf_len += Z_STRLEN(pack_res);
		}else{
			buf_len = Z_STRLEN(pack_res);
			buf = pemalloc(buf_len, 0);
			memcpy(buf, Z_STRVAL(pack_res), buf_len);
		}
	}else{
		zval res;
		hessian2_writer_write_int(self, Z_LVAL(index), &res);

		if (buf_len > 0){
			new_buf  = pemalloc(buf_len + Z_STRLEN(res) + 1, 0);
			new_buf[0] = 'O';
			memcpy(new_buf+1,  buf, buf_len);
			memcpy(new_buf + 1 + buf_len,  Z_STRVAL(res),  Z_STRLEN(res));
			pefree(buf, 0);
			buf = new_buf;
			buf_len += Z_STRLEN(res);
		}else{
			buf_len = Z_STRLEN(res);
			buf = pemalloc(buf_len + 1, 0);
			buf[0] = 'O';
			memcpy(buf+1, Z_STRVAL(res), buf_len);
		}
	}


	/*
		$this->refmap->objectlist[] = $value;
		$classdef = $this->refmap->classlist[$index];
		foreach($classdef->props as $key){
			$val = $value->$key;
			$stream .= $this->writeValue($val);
		}	
	*/
	zval *object_list, *class_list,  *val;
	zval function_name2;

	object_list = zend_read_property(NULL, ref_map, ZEND_STRL("objectlist"), 1 TSRMLS_CC);
	if (Z_TYPE_P(object_list) != IS_ARRAY){
		array_init_size(object_list, 2);
		zend_update_property(NULL, ref_map, ZEND_STRL("objectlist"), object_list TSRMLS_CC);
	}
	zend_hash_next_index_insert(Z_ARRVAL_P(object_list), &value, sizeof(zval **), NULL);

	
	class_list = zend_read_property(NULL, ref_map, ZEND_STRL("classlist"), 1 TSRMLS_CC);
	if (SUCCESS != zend_hash_index_find(Z_ARRVAL_P(class_list), Z_LVAL(index)
		, (void **)&src_entry)){
		zend_error(E_WARNING, "Hessian2Writer::writeObjectData find index error");
		return;
	}
	class_def = *src_entry;


	props = zend_read_property(NULL, class_def, ZEND_STRL("props"), 1 TSRMLS_CC);
	if(!props || (Z_TYPE_P(props) != IS_ARRAY)){
		RETURN_STRINGL(buf, buf_len, 0);
	}


	old_buf_len = buf_len;
	total = zend_hash_num_elements(Z_ARRVAL_P(props));
	zval_ptr = (zval *)pemalloc(sizeof(zval) * total, 0);

	
	i = 0;
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(props), &pos);
	
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(props), (void **)&src_entry, &pos) == SUCCESS) {
		val = zend_read_property(NULL, value, Z_STRVAL_PP(src_entry), Z_STRLEN_PP(src_entry), 1 TSRMLS_CC);
		//add ref?
		hessian2_writer_write_value(self, val, &zval_ptr[i]);
		buf_len += Z_STRLEN(zval_ptr[i]);
		i++;
		
		zend_hash_move_forward_ex(Z_ARRVAL_P(props), &pos);
	}


	//alloc_memory
	new_buf = pemalloc(buf_len, 0);
	if (!zval_ptr){
		php_error_docref(NULL, E_ERROR, "Hessian2Writer::writeObjectData alloc memorty error");
		return;
	}
	p = new_buf;
	if (old_buf_len > 0){
		memcpy(p, buf, old_buf_len);
		p += old_buf_len;
		pefree(buf, 0);
	}

	

	for(i=0; i<total; i++){
		memcpy(p, Z_STRVAL(zval_ptr[i]), Z_STRLEN(zval_ptr[i]));
		p += Z_STRLEN(zval_ptr[i]);
	}

	RETURN_STRINGL(new_buf, buf_len, 0);
}


/*
	Hessian2Writer::writeObject
*/
static PHP_METHOD(Hessian2Writer, writeObject)
{
	zval *self, *value;
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value)) {
		return;
	}
	hessian2_writer_write_object(self, value, return_value);
}

/*
	Hessian2Writer::writeType
*/
static PHP_METHOD(Hessian2Writer, writeType)
{
	zval *self, *type;
	zval *ref_map, ref_index, *references, *type_list;
	zval function_name;
	zval *params[2];
	zval param1;
	zval res;
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &type)) {
		return;
	}
	if (Z_TYPE_P(type) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "type must be a string");
		return;
	}

	/*
	$this->logMsg("writeType $type");
		$refindex = $this->refmap->getTypeIndex($type);
		if($refindex !== false){
			return $this->writeInt($refindex);
		}
		$this->references->typelist[] = $type;
		return $this->writeString($type);
	*/

	/*
	char buf[256];

	sprintf(buf, "writeType %s", Z_STRVAL_P(type));
	ZVAL_STRING(&param1, buf, 1);
	params[0] = &param1;
	ZVAL_STRING(&function_name, "logMsg",1);
	hessian_call_class_function_helper(self, &function_name, 1, params, &res);
	zval_dtor(&function_name);
	*/
	
	ref_map = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_CC);
	ZVAL_STRING(&function_name, "getTypeIndex", 1);
	params[0] = type;
	hessian_call_class_function_helper(ref_map, &function_name, 1, params, &ref_index);

	if (Z_TYPE(ref_index) != IS_BOOL){
		hessian2_writer_write_int(self, Z_LVAL(ref_index), &res);
		RETURN_ZVAL(&res, 1, NULL);
	}

	references = zend_read_property(NULL, self, ZEND_STRL("references"), 1 TSRMLS_CC);
	type_list = zend_read_property(NULL, self, ZEND_STRL("typelist"), 1 TSRMLS_CC);
	if (Z_TYPE_P(type_list) != IS_ARRAY){
		array_init_size(type_list, 1);
		zend_update_property(NULL, self, ZEND_STRL("typelist"), type_list TSRMLS_CC);
	}
	zend_hash_next_index_insert(Z_ARRVAL_P(type_list), &type, sizeof(zval **), NULL);

	hessian2_writer_write_string(self, type, &res);
	RETURN_ZVAL(&res, 1, NULL);
}


/*
	Hessian2Writer::writeReference
*/
static PHP_METHOD(Hessian2Writer, writeReference)
{
	zval *self, *value;
	zval pack_res, write_res;
	zval function_name;
	zval *params[2];
	zval param1, param2;
	zval *log_param;
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value)) {
		return;
	}
	if (Z_TYPE_P(value) != IS_LONG){
		php_error_docref(NULL, E_WARNING, "type must be int");
		return;
	}

	/*
	$this->logMsg("writeReference $value");
		$stream = pack('c', 0x51);
		$stream .= $this->writeInt($value);
		return $stream;
	*/


	/*
	char buf[100];
	ALLOC_ZVAL(log_param);
	sprintf(buf, "writeReference %d", Z_LVAL_P(value));
	
	ZVAL_STRING(log_param, buf, 1);
	params[0] = log_param;
	ZVAL_STRING(&function_name, "logMsg",1);
	hessian_call_class_function_helper(self, &function_name, 1, params, &pack_res);
	zval_dtor(&function_name);
	*/

	

	//pack
	ZVAL_STRING(&function_name, "pack",1);
	ZVAL_STRING(&param1, "c", 1);
	ZVAL_LONG(&param2, 0x51);
	params[0] = &param1;
	params[1] = &param2;
	call_user_function(EG(function_table), NULL, &function_name, &pack_res, 2, params TSRMLS_CC);
	zval_dtor(&function_name);


	hessian2_writer_write_int(self, Z_LVAL_P(value), &write_res);

	zend_uchar *ret_buf;
	int buf_len=0;

	buf_len = Z_STRLEN(pack_res) + Z_STRLEN(write_res);
	ret_buf = pemalloc(buf_len, 0);
	memcpy(ret_buf, Z_STRVAL(pack_res), Z_STRLEN(pack_res));
	memcpy(ret_buf + Z_STRLEN(pack_res), Z_STRVAL(write_res), Z_STRLEN(write_res));


	RETURN_STRINGL(ret_buf, buf_len, 0);
}



/*
	Hessian2Writer::writeDate
*/
static PHP_METHOD(Hessian2Writer, writeDate)
{
	zval *self, *value;
	zval function_name, param1, param2;
	zval *params[2];
	zval res;
	long ts;
	zend_uchar *buf, *p;
	int buf_len;
	int i;
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value)) {
		return;
	}
	if (Z_TYPE_P(value) != IS_LONG){
		php_error_docref(NULL, E_WARNING, "value must be an long");
		return;
	}
	/*
		$ts = $value;
		$this->logMsg("writeDate $ts");
	*/

	ts = Z_LVAL_P(value);

	/*
	char str_buf[256];
	
	sprintf(str_buf, "writeDate %d", Z_LVAL_P(value));
	ZVAL_STRING(&param1, str_buf, 1);
	params[0] = &param1;
	ZVAL_STRING(&function_name, "logMsg",1);
	call_user_function(NULL, &self, &function_name, &res, 1, params TSRMLS_DC);
	*/
	

	/*
		if($ts % 60 != 0){
			$stream = pack('c', 0x4a);
			$ts = $ts * 1000;
			$res = $ts / HessianUtils::pow32;
			$stream .= pack('N', $res);
			$stream .= pack('N', $ts);
		} else { // compact date, only minutes
			
		}
	*/

	ZVAL_STRING(&function_name, "pack", 1);
	if (0 != (ts % 60)){
		zend_class_entry *ce;
		zval **pow32;
		zval zval_ptr[3];
		zval res;
		
		ZVAL_STRING(&param1, "c",1);
		ZVAL_LONG(&param2, 0x4a);
		params[0] = &param1;
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, &zval_ptr[0], 2,  params TSRMLS_DC);

		ts *= 1000;
		ce = hessian_utils_entry;
		zend_hash_find(&ce->constants_table, "pow32", sizeof("pow32"), (void **) &pow32);

		ZVAL_DOUBLE(&res, ts/Z_LVAL_PP(pow32));
		ZVAL_STRING(&param1, "N", 1);
		params[0] = &param1;
		params[1] = &res;
		call_user_function(EG(function_table), NULL, &function_name, &zval_ptr[1], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, ts);
		params[0] = &param1;
		params[1] = &param2;
		call_user_function(EG(function_table), NULL,  &function_name, &zval_ptr[2], 2, params TSRMLS_DC);



		buf_len = Z_STRLEN(zval_ptr[0]) + Z_STRLEN(zval_ptr[1]) + Z_STRLEN(zval_ptr[2]);
		buf = pemalloc(buf_len, 0);
		if (!buf){
			php_error_docref(NULL, E_WARNING, "Hessian2Writer::writeDate alloc memory error");
			return;
		}

		p = buf;
		for(i=0; i<3; i++){
			memcpy(p, Z_STRVAL(zval_ptr[i]), Z_STRLEN(zval_ptr[i]));
			p += Z_STRLEN(zval_ptr[i]);
		}
	}else{
		zval zval_ptr[5];

		/*
			$ts = intval($ts / 60);
			$stream = pack('c', 0x4b);
			$stream .= pack('c', ($ts >> 24));
			$stream .= pack('c', ($ts >> 16));
			$stream .= pack('c', ($ts >> 8));
			$stream .= pack('c', $ts);
		*/
		ts = ts / 60;
		
		ZVAL_STRING(&param1, "c", 1);
		ZVAL_LONG(&param2, 0x4b);
		params[0] = &param1;
		params[1] = &param2;
		call_user_function(EG(function_table), NULL,  &function_name, &zval_ptr[0], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, (long)(ts >> 24));
		params[1] = &param2;
		call_user_function(EG(function_table), NULL,  &function_name, &zval_ptr[1], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, (long)(ts >> 16));
		params[1] = &param2;
		call_user_function(EG(function_table), NULL,  &function_name, &zval_ptr[2], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, (long)(ts >> 8));
		params[1] = &param2;
		call_user_function(EG(function_table), NULL,  &function_name, &zval_ptr[3], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, ts);
		params[1] = &param2;
		call_user_function(EG(function_table), NULL,  &function_name, &zval_ptr[4], 2, params TSRMLS_DC);



		buf_len = Z_STRLEN(zval_ptr[0]) + Z_STRLEN(zval_ptr[1]) + Z_STRLEN(zval_ptr[2]) 
			+ Z_STRLEN(zval_ptr[3]) + Z_STRLEN(zval_ptr[4]);
		buf = pemalloc(buf_len, 0);
		if (!buf){
			php_error_docref(NULL, E_WARNING, "Hessian2Writer::writeDate alloc memory error");
			return;
		}

		p = buf;
		for(i=0; i<5; i++){
			memcpy(p, Z_STRVAL(zval_ptr[i]), Z_STRLEN(zval_ptr[i]));
			p += Z_STRLEN(zval_ptr[i]);
		}
	}

	zval_dtor(&function_name);

	
	RETURN_STRINGL(buf, buf_len, 0);
}

/*
	Hessian2Writer::writeBool
*/
static PHP_METHOD(Hessian2Writer, writeBool)
{
	zval *self, *value;

	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value)) {
		return;
	}

	hessian2_writer_write_bool(value, return_value);
}


/*
	Hessian2Writer::between
*/
static PHP_METHOD(Hessian2Writer, between)
{
	zval *self;
	long value, min, *max;

	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll", &value, &min, &max)) {
		return;
	}

	//return $min <= $value && $value <= $max;
	if (BETWEEN(value, min, max)){
		RETURN_TRUE;
	}else{
		RETURN_FALSE;
	}
}

/*
	Hessian2Writer::writeInt
*/
static PHP_METHOD(Hessian2Writer, writeInt)
{
	zval *self;
	long value;

	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &value)) {
		return;
	}

	hessian2_writer_write_int(self, value, return_value);
}



/*
	Hessian2Writer::writeString
*/
static PHP_METHOD(Hessian2Writer, writeString)
{
	zval *self;
	zval  *value, *len;
	zval function_name, param1, param2;
	zval *params[2];
	char *buf, *p;
	int i;
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value)) {
		return;
	}

	if (Z_TYPE_P(value) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "value must be a string");
		return;
	}

	hessian2_writer_write_string(self, value, return_value);
}


/*
	Hessian2Writer::writeSmallString
*/
static PHP_METHOD(Hessian2Writer, writeSmallString)
{
	zval *self;
	zval  *value, len;
	zval function_name, *param1, *param2;
	zval *params[2];
	zend_uchar *buf;
	long buf_len;
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value)) {
		return;
	}

	if (Z_TYPE_P(value) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "value must be a string");
		return;
	}

	//$len = HessianUtils::stringLength($value);
	ZVAL_STRING(&function_name, "HessianUtils::stringLength", 1);
	params[0] = value;
	call_user_function(EG(function_table), NULL, &function_name, &len, 1, params TSRMLS_DC);
	zval_dtor(&function_name);

	ALLOC_ZVAL(param1);
	if(Z_LVAL(len) < 32){
			//return pack('C', $len) . $this->writeStringData($value);
			zval  res[2];
			
			ZVAL_STRING(&function_name, "pack", 1);
			ZVAL_STRING(param1, "C", 1);
			params[0] = param1;
			params[1] = &len;
			
			call_user_function(EG(function_table), NULL, &function_name, &res[0], 2, params TSRMLS_CC);
			zval_dtor(param1);
			hessian2_writer_write_string_data(self, value, &res[1]);
			zval_dtor(&function_name);
			
			buf_len = Z_STRLEN(res[0]) + Z_STRLEN(res[1]);
			buf = pemalloc(buf_len, 0);
			if (!buf){
				php_error_docref(NULL, E_ERROR, "Hessian2Writer::writeSmallString alloc memory error");
				return;
			}
			
			memcpy(buf, Z_STRVAL(res[0]), Z_STRLEN(res[0]));
			memcpy(buf+Z_STRLEN(res[0]), Z_STRVAL(res[1]), Z_STRLEN(res[1]));

			zval_dtor(&res[0]);
			zval_dtor(&res[1]);

			
			RETURN_STRINGL(buf, buf_len, 0);
	}else{
		/*
			$b0 = 0x30 + ($len >> 8);
			$stream .= pack('C', $b0);
			$stream .= pack('C', $len);
			return $stream . $this->writeStringData($value);
		*/
		zval res[3];
		char *buf, *p;
		int i;

		ALLOC_ZVAL(param2);
		ZVAL_STRING(param1, "C", 1);
		ZVAL_STRING(&function_name, "pack", 1);

		
		ZVAL_LONG(param2, 0X30 + (Z_LVAL(len) >> 8));
		params[0] = param1;
		params[1] = param2;
		call_user_function(EG(function_table), NULL, &function_name, &res[0], 2, params TSRMLS_CC);

		params[0] = param1;
		ZVAL_LONG(param2, Z_LVAL(len));
		params[1] = param2;
		call_user_function(EG(function_table), NULL, &function_name, &res[1], 2, params TSRMLS_CC);
		zval_dtor(&function_name);

		hessian2_writer_write_string_data(self, value, &res[2]);

		buf = pemalloc(Z_STRLEN(res[0]) + Z_STRLEN(res[1]) +  Z_STRLEN(res[2]), 0);
		if (!buf){
			FREE_ZVAL(param1);
			FREE_ZVAL(param2);
			php_error_docref(NULL, E_WARNING, "Hessian2Writer::writeSmallString alloc memory error");
			return;
		}
		p = buf;
		for(i=0; i<3; i++){
			memcpy(p, Z_STRVAL(res[i]), Z_STRLEN(res[i]));
			p +=  Z_STRLEN(res[i]);
		}

		FREE_ZVAL(param1);
		FREE_ZVAL(param2);

		RETURN_STRING(buf, 0);
	}
}


/*
	Hessian2Writer::writeStringData
*/
static PHP_METHOD(Hessian2Writer, writeStringData)
{
	zval *self;
	zval  *string;
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &string)) {
		return;
	}

	if (Z_TYPE_P(string) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "string must be a string");
		return;
	}
	hessian2_writer_write_string_data(self, string, return_value);
}

/*
	Hessian2Writer::writeDouble
*/
static PHP_METHOD(Hessian2Writer, writeDouble)
{
	zval *self;
	zval  *param_value, res[5];
	double value, frac;
	zval function_name, param1, param2;
	zval *params[2];
	zend_uchar *buf, *p;
	ulong buf_len;
	int i;
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &param_value)) {
		return;
	}

	if ((Z_TYPE_P(param_value) != IS_DOUBLE) && (Z_TYPE_P(param_value) != IS_LONG)){
		php_error_docref(NULL, E_WARNING, "value must be a double or long");
		return;
	}

	hessian2_writer_write_double(self, param_value, return_value);
}



/*
	Hessian2Writer::writeResource
*/
static PHP_METHOD(Hessian2Writer, writeResource)
{
	zval *self;
	zval  *handle;

	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &handle)) {
		return;
	}

	if (Z_TYPE_P(handle) != IS_RESOURCE){
		php_error_docref(NULL, E_WARNING, "handle must be a handle");
		return;
	}

	hessian2_writer_write_resource(self, handle, return_value);
}


//Hessian2Writer functions
const zend_function_entry hessian2_writer_functions[] = {
	PHP_ME(Hessian2Writer, __construct, arginfo_hessian2_writer_construct, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Writer, logMsg, arginfo_hessian2_writer_log_msg, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Writer, setTypeMap, arginfo_hessian2_writer_set_typemap, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Writer, setFilters, arginfo_hessian2_writer_set_filters, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Writer, writeValue, arginfo_hessian2_writer_write_value, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Writer, resolveDispatch, arginfo_hessian2_writer_resolve_dispatch, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Writer, writeNull, arginfo_hessian2_writer_write_null, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Writer, writeArray, arginfo_hessian2_writer_write_array, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Writer, writeMap, arginfo_hessian2_writer_write_map, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Writer, writeObjectData, arginfo_hessian2_writer_write_object_data, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Writer, writeObject, arginfo_hessian2_writer_write_object, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Writer, writeType, arginfo_hessian2_writer_write_type, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Writer, writeReference, arginfo_hessian2_writer_write_reference, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Writer, writeDate, arginfo_hessian2_writer_write_date, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Writer, writeBool, arginfo_hessian2_writer_write_date, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Writer, between, arginfo_hessian2_writer_between, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Writer, writeInt, arginfo_hessian2_writer_write_int, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Writer, writeString, arginfo_hessian2_writer_write_string, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Writer, writeSmallString, arginfo_hessian2_writer_write_small_string, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Writer, writeStringData, arginfo_hessian2_writer_write_string_data, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Writer, writeDouble, arginfo_hessian2_writer_write_double, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2Writer, writeResource, arginfo_hessian2_writer_write_resource, ZEND_ACC_PUBLIC)
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
