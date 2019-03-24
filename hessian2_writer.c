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
	Hessian2Writer::__construc
*/
static PHP_METHOD(Hessian2Writer, __construc)
{
	zval *self, *options;
	zval *ref_map, *type_map, *log_msg;
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &options)) {
		return;
	}
	if (Z_TYPE_P(options) != IS_ARRAY){
		php_error_docref(NULL, E_WARNING, "options must be an array");
		return;
	}

	ALLOC_ZVAL(ref_map);
	object_init_ex(ref_map, hessian_reference_map_entry);
	zend_update_property(NULL, self, ZEND_STRL("refmap"), ref_map TSRMLS_CC);

	ALLOC_ZVAL(type_map);
	object_init_ex(type_map, hessian_type_map_entry);
	zend_update_property(NULL, self, ZEND_STRL("typemap"), type_map TSRMLS_CC);

	zend_update_property(NULL, self, ZEND_STRL("options"), options TSRMLS_CC);

	ALLOC_ZVAL(log_msg);
	array_init_size(log_msg, 1);
	zend_update_property(NULL, self, ZEND_STRL("logMsg"), log_msg TSRMLS_CC);

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
	zend_update_property(NULL, self, ZEND_STRL("logMsg"), log_msg TSRMLS_CC);
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
	zend_update_property(NULL, self, ZEND_STRL("typemap"), type_map TSRMLS_CC);
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
static PHP_METHOD(Hessian2Writer, writeValue)
{
	zval *self, *value;
	zval *type, *dispatch;
	zval function_name;
	zval *params[2];
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value)) {
		return;
	}

	/*
		$type = gettype($value);
		$dispatch = $this->resolveDispatch($type);
	*/
	params[0] = value;
	ZVAL_STRING(&function_name, "gettype", 1);
	call_user_function(EG(function_table), NULL, &function_name, type, 1, params TSRMLS_DC);

	ZVAL_STRING(&function_name, "resolveDispatch", 1);
	params[0] = type;
	call_user_function(NULL, &self, &function_name, dispatch, 1, params TSRMLS_DC);

	/*
		if(is_object($value)){
			$filter = $this->filterContainer->getCallback($value);
			if($filter) {
				
			}
		}
		$data = $this->$dispatch($value);
		return $data;	
	*/

	if (Z_TYPE_P(value)== IS_OBJECT){
		zval *filter_container,  *filter;

		filter_container = zend_read_property(NULL, self, ZEND_STRL("filterContainer"), 1 TSRMLS_DC);
		ZVAL_STRING(&function_name, "getCallback", 1);
		params[0] = value;
		call_user_function(NULL, &filter_container, &function_name, filter, 1, params TSRMLS_DC);

		if (i_zend_is_true(filter)){
			/*
				$value = $this->filterContainer->doCallback($filter, array($value, $this));
				if($value instanceof HessianStreamResult){
					return $value->stream;
				}
				$ntype = gettype($value);
				
			*/
			zval arr, *res;
			
			array_init_size(&arr, 2);
			zend_hash_next_index_insert(Z_ARRVAL(arr), &value, sizeof(zval **), NULL);
			zend_hash_next_index_insert(Z_ARRVAL(arr), &self, sizeof(zval **), NULL);

			ZVAL_STRING(&function_name, "doCallback", 1);
			params[0] = filter;
			params[1] = &arr;

			call_user_function(NULL, &filter_container, &function_name, res, 2, params TSRMLS_DC);
			if (instanceof_function(Z_OBJCE_P(res), hessian_stream_result_entry TSRMLS_DC)){
				zval *stream;
				
				stream = zend_read_property(NULL, res, ZEND_STRL("stream"), 1 TSRMLS_DC);
				RETURN_ZVAL(stream, 1, NULL);
			}

			zval *ntype;
			ZVAL_STRING(&function_name, "gettype", 1);
			params[0] = value;
			call_user_function(EG(function_table), NULL, &function_name, ntype, 1, params TSRMLS_DC);

			/*
				if($type != $ntype)
					$dispatch = $this->resolveDispatch($ntype);
			*/

			if (hessian2_writer_compare(type, ntype) == 0){
				ZVAL_STRING(&function_name, "resolveDispatch", 1);
				params[0] = ntype;
				call_user_function(NULL, &self, &function_name, ntype, 1, params TSRMLS_DC);
			}
		}
	}

	//$data = $this->$dispatch($value);
	zval  *data;

	params[0] = value;
	call_user_function(NULL, &self, dispatch, data, 1, params TSRMLS_DC);

	RETURN_ZVAL(data, 1, NULL);
}


/*
	Hessian2Writer::resolveDispatch
*/
static PHP_METHOD(Hessian2Writer, resolveDispatch)
{
	zval *self, *type;
	char dispatch[16], *buf;
	int len;
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &type)) {
		return;
	}
	if ((Z_TYPE_P(type) !=  IS_STRING) || Z_STRLEN_P(type) < 1){
		php_error_docref(NULL,E_WARNING, "type must be a string");
		return;
	}

	buf = Z_STRVAL_P(type);
	if (strncmp(buf, "integer", strlen("integer")-1) == 0){
		len = sprintf(dispatch, "writeInt");
		dispatch[len] = 0;
	}else if (strncmp(buf, "boolean", strlen("boolean")-1) == 0){
		len = sprintf(dispatch, "writeBool");
		dispatch[len] = 0;
	}else if (strncmp(buf, "string", strlen("string")-1) == 0){
		len = sprintf(dispatch, "writeString");
		dispatch[len] = 0;
	}else if (strncmp(buf, "double", strlen("double")-1) == 0){
		len = sprintf(dispatch, "writeDouble");
		dispatch[len] = 0;
	}else if (strncmp(buf, "array", strlen("array")-1) == 0){
		len = sprintf(dispatch, "writeArray");
		dispatch[len] = 0;
	}else if (strncmp(buf, "object", strlen("object")-1) == 0){
		len = sprintf(dispatch, "writeObject");
		dispatch[len] = 0;
	}else if (strncmp(buf, "NULL", strlen("NULL")-1) == 0){
		len = sprintf(dispatch, "writeNull");
		dispatch[len] = 0;
	}else if (strncmp(buf, "resource", strlen("resource")-1) == 0){
		len = sprintf(dispatch, "writeResource");
		dispatch[len] = 0;
	}else{
		//throw new Exception("Handler for type $type not implemented");
		zend_class_entry **ce_exception;
		zend_hash_find(CG(class_table), "exception", sizeof("exception"), (void **) &ce_exception);
		zend_throw_exception(*ce_exception, sprintf("Handler for type %s not implemented", Z_STRVAL_P(type)), 0 TSRMLS_CC);
	}

	//$this->logMsg("dispatch $dispatch");
	zval function_name, param1;
	zval *params[1];
	char err_buf[256];

	sprintf(err_buf, "dispatch %s", dispatch);
	ZVAL_STRING(&param1, err_buf, 1);
	ZVAL_STRING(&function_name, "logMsg", 1);
	params[0] = &param1;
	call_user_function(NULL, &self, &function_name, NULL, 1, params TSRMLS_DC);

	RETURN_STRING(buf, 1);
}

/*
	Hessian2Writer::writeNull
*/
static PHP_METHOD(Hessian2Writer, writeNull)
{
	RETURN_STRING("N", 1);
}

/*
	Hessian2Writer::writeArray
*/
static PHP_METHOD(Hessian2Writer, writeArray)
{
	zval *self, *array;
	zval *ref_map, *ref_index;
	zval function_name;
	zval *params[2];
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &array)) {
		return;
	}
	if(Z_TYPE_P(array) != IS_ARRAY){
		php_error_docref(NULL, E_WARNING, "array type must be array");
		return;
	}

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
	self = getThis();
	ref_map = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_CC);
	ZVAL_STRING(&function_name, "getReference", 1);
	params[0] = array;
	call_user_function(NULL, &ref_map, &function_name, ref_index, 1, params TSRMLS_DC);
	if (!(Z_TYPE_P(ref_index) == IS_BOOL && Z_BVAL_P(ref_index) <= 0)){
		zval *res;
		ZVAL_STRING(&function_name, "writeReference", 1);
		params[0] = ref_index;
		call_user_function(NULL, &ref_map, &function_name, res, 1, params TSRMLS_DC);
		RETURN_ZVAL(res, 1, NULL);
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
	zval *call_res;
	
	total = zend_hash_num_elements(Z_ARRVAL_P(array));
	ZVAL_STRING(&function_name, "HessianUtils::isListFormula", 1);
	params[0] = array;
	call_user_function(EG(function_table), NULL, &function_name, call_res, 1, params TSRMLS_DC);

	if (i_zend_is_true(call_res)){
		zval *object_list;
		char *buf;
		zval param1, param2;
		ulong old_buf_len=0, buf_len=0;

		object_list = zend_read_property(NULL, ref_map, ZEND_STRL("objectlist"), 1 TSRMLS_CC);
		if (Z_TYPE_P(object_list ) != IS_ARRAY){
			array_init_size(object_list, 1);
		}
		zend_hash_next_index_insert(Z_ARRVAL_P(object_list), &array, sizeof(zval **), NULL);

		ZVAL_STRING(&function_name, "pack", 1);
		ZVAL_STRING(&param1, "c", 1);
		if (total <= 7){
			int len;

			
			len = total += 0x78;
			ZVAL_LONG(&param2, len);
			params[0] = &param1;
			params[1] = &param2;
			call_user_function(EG(function_table), NULL, &function_name, call_res, 2, params TSRMLS_DC);
			buf = Z_STRVAL_P(call_res);
			buf_len = Z_STRLEN_P(call_res);
		}else{
			/*
				$stream = pack('c', 0x58);
				$stream .= $this->writeInt($total);
			*/
			zval *write_int_res;
			ZVAL_LONG(&param2, 0x58);
			params[0] = &param1;
			params[1] = &param2;
			call_user_function(EG(function_table), NULL, &function_name, call_res, 2, params TSRMLS_DC);
			buf = Z_STRVAL_P(call_res);
			buf_len = Z_STRLEN_P(call_res);

			ZVAL_LONG(&param1, total);
			params[0] = &param1;

			call_user_function(NULL, &self, &function_name, write_int_res, 1, params TSRMLS_DC);	
			buf_len += Z_STRLEN_P(write_int_res);
			perealloc(buf, buf_len, 0);
		}

		/*
			foreach($array as $key => $value){
				$stream .= $this->writeValue($value); 
			}
			return $stream;
		*/
		zval **zval_ptr, *src_entry;
		int i=0;
		HashPosition pos;

		old_buf_len = buf_len;
		zval_ptr = pemalloc(sizeof(zval*)*total, 0);
		if (!zval_ptr){
			php_error_docref(NULL, E_ERROR, "Hessian2Writer::writeArray alloc memory error");
			return;
		}

		ZVAL_STRING(&function_name, "writeValue", 1);
		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(array), &pos);
		while (zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void **)&src_entry, &pos) == SUCCESS) {
			//$stream .= $this->writeValue($value); 
			params[0] = src_entry;
			call_user_function(NULL, &self, &function_name, zval_ptr[i], 1, params TSRMLS_DC);
			buf_len += Z_STRLEN_P(zval_ptr[i]);
			++i;
			zend_hash_move_forward_ex(Z_ARRVAL_P(array), &pos);
		}

		char *new_buf, *p;
		new_buf = pemalloc(buf_len,  0);
		if (!new_buf){
			php_error_docref(NULL, E_ERROR, "Hessian2Writer::writeArray alloc memory error");
			return;
		}

		p = new_buf;
		memcpy(p, buf, old_buf_len);
		p += old_buf_len;
		for(i=0; i<total; i++){
			memcpy(p, Z_STRVAL_P(zval_ptr[i]), Z_STRLEN_P(zval_ptr[i]));
			p += Z_STRLEN_P(zval_ptr[i]);
		}

		RETURN_STRING(new_buf, 0);
	}else{
		//return $this->writeMap($array);
		zval *res;

		ZVAL_STRING(&function_name, "writeMap", 1);
		params[0] = array;
		call_user_function(NULL, &self, &function_name, res, 1, params TSRMLS_DC);

		RETURN_ZVAL(res, 1, NULL);
	}
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
	ZVAL_STRING(&function_name, "getReference", 1);
	params[0] = map;
	call_user_function(NULL, &ref_map, &function_name, ref_index, 1, params TSRMLS_DC);
	if (!(Z_TYPE_P(ref_index) == IS_BOOL && Z_BVAL_P(ref_index) <= 0)){
		zval *res;
		ZVAL_STRING(&function_name, "writeReference", 1);
		params[0] = ref_index;
		call_user_function(NULL, &ref_map, &function_name, res, 1, params TSRMLS_DC);
		RETURN_ZVAL(res, 1, NULL);
	}

	//$this->refmap->objectlist[] = &$map;
	object_list = zend_read_property(NULL, ref_map, ZEND_STRL("objectlist"), 1 TSRMLS_CC);
	if (Z_TYPE_P(object_list ) != IS_ARRAY){
		array_init_size(object_list, 1);
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

	
	if ((Z_TYPE_P(type) == IS_STRING) || (0 == Z_STRLEN_P(type))){
		char tmp_buf[1];
		tmp_buf[0] = 'H';
		old_buf_len = 1;
		buf = tmp_buf;
	}else{
		zval *call_res;
		char *tmp_buf;
		
		ZVAL_STRING(&function_name, "writeType", 1);
		params[0] = type;
		call_user_function(NULL, &self, &function_name, call_res, 1, params TSRMLS_DC);
		buf = pemalloc(Z_STRLEN_P(call_res) + 1, 0);
		buf[0] = 'M';
		memcpy(buf+1, Z_STRVAL_P(call_res), Z_STRLEN_P(call_res));
		old_buf_len = Z_STRLEN_P(call_res) + 1;
	}

	/*
		foreach($map as $key => $value){
			$stream .= $this->writeValue($key);
			$stream .= $this->writeValue($value);
		}
		$stream .= 'Z';
	*/

	HashPosition pos;
	zval *src_entry;
	char *string_key, *new_buf, *p;
	uint string_key_len;
	ulong num_key;
	int total;
	zval **zval_ptr;
	zval param1;

	buf_len = old_buf_len;
	total = zend_hash_num_elements(Z_ARRVAL_P(map));

	zval_ptr = pemalloc(sizeof(zval *) * total *2, 0);
	int i = 0 ;
	ZVAL_STRING(&function_name, "writeValue", 1);
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(map), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(map), (void **)&src_entry, &pos) == SUCCESS) {
		zend_hash_get_current_key_ex(Z_ARRVAL_P(map), &string_key, &string_key_len, &num_key, 0, &pos);
		ZVAL_STRING(&param1, string_key, 0);
		params[0] = &param1;
		call_user_function(NULL, &self, &function_name, zval_ptr[i], 1, params TSRMLS_DC);
		buf_len += Z_STRLEN_P(zval_ptr[i]);
		i++;

		params[0] = src_entry;
		call_user_function(NULL, &self, &function_name, zval_ptr[i], 1, params TSRMLS_DC);
		buf_len += Z_STRLEN_P(zval_ptr[i]);
		i++;
		
		zend_hash_move_forward_ex(Z_ARRVAL_P(map), &pos);
	}

	//cpy
	new_buf = pemalloc(buf_len, 0);
	if (!new_buf){
		php_error_docref(NULL, E_ERROR, "Hessian2Writer::writeMap alloc memory error");
		return;
	}

	p = new_buf;
	mempcy(p, buf, old_buf_len);
	
	for(i=0; i<total*2; i++){
		memcpy(p, Z_STRVAL_P(zval_ptr[i]), Z_STRLEN_P(zval_ptr[i]));
		p +=  Z_STRLEN_P(zval_ptr[i]);
	}

	RETURN_STRING(new_buf, 0);
}


/*
	Hessian2Writer::writeObjectData
*/
static PHP_METHOD(Hessian2Writer, writeObjectData)
{
	zval *self, *value;
	char *name;
	zend_uint name_len;
	zval *ref_map, *index, *class_def, *z_class, *type;
	zval function_name, param1;
	zval *params[2];
	int total, i;
	zval  **zval_ptr;
	ulong buf_len = 0, old_buf_len;
	char *buf, *p, *new_buf;
	HashPosition pos;
	zval *src_entry, *props;

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
	ZVAL_STRING(z_class, name, 1);

	zend_get_object_classname(value, (const char**)&name, &name_len TSRMLS_CC);
	ref_map = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_CC);
	ZVAL_STRING(&function_name, "getClassIndex", 1);
	params[0] = z_class;

	call_user_function(NULL, &ref_map, &function_name, index, 1, params TSRMLS_DC);

	if (Z_TYPE_P(index) == IS_BOOL && Z_BVAL_P(index) < 1){
		zval *target, *props;
		/*
			$classdef = new HessianClassDef();
			$classdef->type = $class;
			if($class == 'stdClass'){
				$classdef->props = array_keys(get_object_vars($value));
			} else
				$classdef->props = array_keys(get_class_vars($class));
		*/

		object_init_ex(class_def, hessian_class_def_entry);
		zend_update_property(NULL, class_def, ZEND_STRL("type"), z_class TSRMLS_CC);

		if (strncmp(name, "stdClass", 7) == 0){
			target = value;
		}else{
			target = z_class;
		}
		ZVAL_STRING(&function_name, "get_object_vars", 1);
		params[0] = target;
		call_user_function(NULL, &ref_map, &function_name, target, 1, params TSRMLS_DC);

		ZVAL_STRING(&function_name, "array_keys", 1);
		params[0] = target;
		call_user_function(NULL, &ref_map, &function_name, props, 1, params TSRMLS_DC);

		zend_update_property(NULL, class_def, ZEND_STRL("props"), props TSRMLS_CC);

		/*
			$index = $this->refmap->addClassDef($classdef);
			$total = count($classdef->props);
		*/

		ZVAL_STRING(&function_name, "addClassDef", 1);
		params[0] = class_def;
		call_user_function(NULL, &ref_map, &function_name, index, 1, params TSRMLS_DC);


		/*
			$total = count($classdef->props);
			
			$type = $this->typemap->getRemoteType($class);
			$class = $type ? $type : $class;
		*/
		total = Z_LVAL_P(props);

		zval *type_map;

		type_map = zend_read_property(NULL, self, ZEND_STRL("typemap"), 1 TSRMLS_CC);
		ZVAL_STRING(&function_name, "getRemoteType", 1);
		params[0] = z_class;
		call_user_function(NULL, &ref_map, &function_name, type, 1, params TSRMLS_DC);
		if (i_zend_is_true(type)){
			z_class = type;
		}

		/*
			$stream .= 'C';
			$stream .= $this->writeString($class);
			$stream .= $this->writeInt($total);
			foreach($classdef->props as $name){
				$stream .= $this->writeString($name);
			}
		*/
		
		zval_ptr = pemalloc(sizeof(zval*) * zend_hash_num_elements(Z_ARRVAL_P(props)) + 2 
		+ zend_hash_num_elements(Z_ARRVAL_P(props)) * 2 + 1, 0);
		if (!zval_ptr){
			php_error_docref(NULL, E_ERROR, "Hessian2Writer::writeObjectData alloc memorty error");
			return;
		}
		i = 0;
		ZVAL_STRING(&function_name, "writeString", 1);
		params[0] = z_class;
		call_user_function(NULL, &ref_map, &function_name, zval_ptr[i], 1, params TSRMLS_DC);
		++i;

		ZVAL_STRING(&function_name, "writeInt", 1);
		ZVAL_LONG(&param1, total);
		params[0] = &param1;
		call_user_function(NULL, &ref_map, &function_name, zval_ptr[i], 1, params TSRMLS_DC);
		++i;
		

		

		ZVAL_STRING(&function_name, "writeString", 1);
		buf_len = 1;
		buf_len += Z_STRLEN_P(zval_ptr[0]);
		buf_len += Z_STRLEN_P(zval_ptr[1]);
		
		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(props), &pos);
		while (zend_hash_get_current_data_ex(Z_ARRVAL_P(props), (void **)&src_entry, &pos) == SUCCESS) {
			params[0] = src_entry;
			call_user_function(NULL, &ref_map, &function_name, zval_ptr[i], 1, params TSRMLS_DC);
			buf_len += Z_STRLEN_P(zval_ptr[i]);
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

		ulong zval_count;

		zval_count = i;
		for(i=0; i<zval_count; i++){
			memcpy(p, Z_STRVAL_P(zval_ptr[i]),  Z_STRLEN_P(zval_ptr[i]));
			p +=  Z_STRLEN_P(zval_ptr[i]);
		}
	}else{
		zval_ptr = pemalloc(zend_hash_num_elements(Z_ARRVAL_P(props)) * 2 + 1, 0);
		if (!zval_ptr){
			php_error_docref(NULL, E_ERROR, "Hessian2Writer::writeObjectData alloc memorty error");
			return;
		}
		i = 0;
	}
	int last;
	last = i;
	
	old_buf_len  = buf_len;

	/*
		if($index < 16){
			$stream .= pack('c', $index + 0x60);
		} else{
			$stream .= 'O';
			$stream .= $this->writeInt($index);
		}
	*/

	if (Z_LVAL_P(index) < 16){
		ZVAL_STRING(&function_name, "pack", 1);
		zval param2;
		ZVAL_STRING(&param1, "c", 1);
		ZVAL_LONG(&param2, Z_LVAL_P(index) + 0x60);
		params[0] = &param1;
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, zval_ptr[i], 2, params TSRMLS_DC);
		buf_len += Z_STRLEN_P(zval_ptr[i]);
	}else{
		ZVAL_STRING(&function_name, "writeInt", 1);
		params[0] = index;
		call_user_function(NULL, &self, &function_name, zval_ptr[i], 1, params TSRMLS_DC);
		buf_len += Z_STRLEN_P(zval_ptr[i]) + 1;
	}
	++i;


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
	}
	zend_hash_next_index_insert(Z_ARRVAL_P(object_list), &value, sizeof(zval **), NULL);
	class_list = zend_read_property(NULL, ref_map, ZEND_STRL("classlist"), 1 TSRMLS_CC);
	if (SUCCESS == zend_hash_find(Z_ARRVAL_P(class_list), Z_STRVAL_P(index), Z_STRLEN_P(index), (void **)&src_entry)){
		class_def = src_entry;
	}

	props = zend_read_property(NULL, class_def, ZEND_STRL("props"), 1 TSRMLS_CC);
	ZVAL_STRING(&function_name, "writeValue", 1);
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(props), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(props), (void **)&src_entry, &pos) == SUCCESS) {
		val = zend_read_property(NULL, value, Z_STRVAL_P(src_entry), Z_STRLEN_P(src_entry), 1 TSRMLS_CC);
		params[0] = val;
		call_user_function(NULL, &self, &function_name, zval_ptr[i], 1, params TSRMLS_DC);
		buf_len += Z_STRLEN_P(zval_ptr[i]);
		++i;
		zend_hash_move_forward_ex(Z_ARRVAL_P(props), &pos);
	}

	//alloc_memory
	new_buf = pemalloc(buf_len, 0);
	if (!zval_ptr){
		php_error_docref(NULL, E_ERROR, "Hessian2Writer::writeObjectData alloc memorty error");
		return;
	}
	p = new_buf;
	if (buf){
		memcpy(p, buf, old_buf_len);
		p += old_buf_len;
	}

	if (Z_LVAL_P(index) < 16){
		memcpy(p, Z_STRVAL_P(zval_ptr[last]), Z_STRLEN_P(zval_ptr[last]));
		p += Z_STRLEN_P(zval_ptr[last]);
		++last;
	}else{
		*p = 0;
		++p;
		memcpy(p, Z_STRVAL_P(zval_ptr[last]), Z_STRLEN_P(zval_ptr[last]));
		p += Z_STRLEN_P(zval_ptr[last]);
		++last;
	}

	while(last < i){
		memcpy(p, Z_STRVAL_P(zval_ptr[last]), Z_STRLEN_P(zval_ptr[last]));
		p += Z_STRLEN_P(zval_ptr[last]);
		last++;
	}

	RETURN_STRING(new_buf, 0);
}


/*
	Hessian2Writer::writeObject
*/
static PHP_METHOD(Hessian2Writer, writeObject)
{
	zval *self, *value;
	zval *ref_map,ref_index;
	zval function_name;
	zval *params[2];
	zval *res;
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value)) {
		return;
	}

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
	call_user_function(NULL, &self, &function_name, &ref_index, 1, params TSRMLS_DC);

	if (Z_TYPE(ref_index) != IS_BOOL){
		
		ZVAL_STRING(&function_name, "writeReference", 1);
		params[0] = &ref_index;
		call_user_function(NULL, &self, &function_name, res, 1, params TSRMLS_DC);
		RETURN_ZVAL(res, 1, NULL);
	}

	ZVAL_STRING(&function_name, "writeObjectData", 1);
	params[0] = value;
	call_user_function(NULL, &self, &function_name, res, 1, params TSRMLS_DC);
	RETURN_ZVAL(res, 1, NULL);
}

/*
	Hessian2Writer::writeType
*/
static PHP_METHOD(Hessian2Writer, writeType)
{
	zval *self, *type;
	zval *ref_map, *ref_index, *references, *type_list;
	zval function_name;
	zval *params[2];
	zval param1;
	zval *res;
	
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
	char buf[256];

	sprintf(buf, "writeType %s", Z_STRVAL_P(type));
	ZVAL_STRING(&param1, buf, 1);
	params[0] = &param1;
	ZVAL_STRING(&function_name, "logMsg",1);
	call_user_function(NULL, &self, &function_name, NULL, 1, params TSRMLS_DC);

	ref_map = zend_read_property(NULL, self, ZEND_STRL("refmap"), 1 TSRMLS_CC);
	ZVAL_STRING(&function_name, "getTypeIndex", 1);
	params[0] = type;
	call_user_function(NULL, &self, &function_name, ref_index, 1, params TSRMLS_DC);

	if (Z_TYPE_P(ref_index) != IS_BOOL){
		
		ZVAL_STRING(&function_name, "writeInt", 1);
		params[0] = ref_index;
		call_user_function(NULL, &self, &function_name, res, 1, params TSRMLS_DC);
		RETURN_ZVAL(res, 1, NULL);
	}

	references = zend_read_property(NULL, self, ZEND_STRL("references"), 1 TSRMLS_CC);
	type_list = zend_read_property(NULL, self, ZEND_STRL("typelist"), 1 TSRMLS_CC);
	if (Z_TYPE_P(type_list) != IS_ARRAY){
		array_init_size(type_list, 1);
	}
	zend_hash_next_index_insert(Z_ARRVAL_P(type_list), &type, sizeof(zval **), NULL);

	ZVAL_STRING(&function_name, "writeString", 1);
	params[0] = type;
	call_user_function(NULL, &self, &function_name, res, 1, params TSRMLS_DC);
	RETURN_ZVAL(res, 1, NULL);
}


/*
	Hessian2Writer::writeReference
*/
static PHP_METHOD(Hessian2Writer, writeReference)
{
	zval *self, *value, *stream;
	zval *pack_res, *write_res;
	zval function_name;
	zval *params[2];
	zval param1, param2;
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value)) {
		return;
	}
	if (Z_TYPE_P(value) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "type must be a string");
		return;
	}

	/*
	$this->logMsg("writeReference $value");
		$stream = pack('c', 0x51);
		$stream .= $this->writeInt($value);
		return $stream;
	*/

	ZVAL_STRING(&param1, sprintf("writeReference %s", Z_STRVAL_P(value)),1);
	params[0] = &param1;
	ZVAL_STRING(&function_name, "logMsg",1);
	call_user_function(NULL, &self, &function_name, pack_res, 1, params TSRMLS_DC);

	ZVAL_STRING(&function_name, "pack",1);
	ZVAL_STRING(&param1, "c", 1);
	ZVAL_LONG(&param2, 0x51);
	params[0] = &param1;
	params[1] = &param2;

	ZVAL_STRING(&function_name, "writeInt",1);
	params[0] = value;
	call_user_function(NULL, &self, &function_name, write_res, 1, params TSRMLS_DC);

	char *buf;
	buf = strcat(Z_STRVAL_P(pack_res), Z_STRVAL_P(write_res));

	RETURN_STRING(buf, 0);
}



/*
	Hessian2Writer::writeDate
*/
static PHP_METHOD(Hessian2Writer, writeDate)
{
	zval *self, *value;
	zval function_name, param1, param2;
	zval *params[2];
	zval *res;
	long ts;
	char *buf, *p;
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
	char str_buf[256];
	
	ts = Z_LVAL_P(value);
	sprintf(str_buf, "writeDate %d", Z_LVAL_P(value));
	ZVAL_STRING(&param1, str_buf, 1);
	params[0] = &param1;
	ZVAL_STRING(&function_name, "logMsg",1);
	call_user_function(NULL, &self, &function_name, NULL, 1, params TSRMLS_DC);


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
	if (0 == (ts % 60)){
		zend_class_entry *ce;
		zval *pow32;
		zval* zval_ptr[3];
		zval res;
		
		ZVAL_STRING(&param1, "c",1);
		ZVAL_LONG(&param2, 0x4a);
		params[0] = &param1;
		params[1] = &param2;
		call_user_function(NULL, &self, &function_name, zval_ptr[0], 2,  params TSRMLS_DC);

		ts *= 1000;
		ce = zend_fetch_class("HessianUtils", strlen("HessianUtils")- 1, 0);
		zend_hash_find(&ce->constants_table, "pow32", 6, (void **) &pow32);

		ZVAL_LONG(&res, ts/Z_LVAL_P(pow32));
		ZVAL_STRING(&param1, "N",1);
		params[0] = &param1;
		params[1] = &res;
		call_user_function(NULL, &self, &function_name, zval_ptr[1], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, ts);
		params[0] = &param1;
		params[1] = &param2;
		call_user_function(NULL, &self, &function_name, zval_ptr[2], 2, params TSRMLS_DC);

		buf = pemalloc(Z_STRLEN_P(zval_ptr[0]) + Z_STRLEN_P(zval_ptr[1]) + Z_STRLEN_P(zval_ptr[2]), 0);
		if (!buf){
			php_error_docref(NULL, E_WARNING, "Hessian2Writer::writeDate alloc memory error");
			return;
		}

		p = buf;
		for(i=0; i<3; i++){
			memcpy(p, Z_STRVAL_P(zval_ptr[i]), Z_STRLEN_P(zval_ptr[i]));
			p += Z_STRLEN_P(zval_ptr[i]);
		}
	}else{
		zval *zval_ptr[5];

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
		call_user_function(NULL, &self, &function_name, zval_ptr[0], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, ts >> 24);
		params[1] = &param2;
		call_user_function(NULL, &self, &function_name, zval_ptr[1], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, ts >> 16);
		params[1] = &param2;
		call_user_function(NULL, &self, &function_name, zval_ptr[2], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, ts >> 8);
		params[1] = &param2;
		call_user_function(NULL, &self, &function_name, zval_ptr[3], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, ts);
		params[1] = &param2;
		call_user_function(NULL, &self, &function_name, zval_ptr[4], 2, params TSRMLS_DC);

		buf = pemalloc(Z_STRLEN_P(zval_ptr[0]) + Z_STRLEN_P(zval_ptr[1]) 
			+ Z_STRLEN_P(zval_ptr[2]) + Z_STRLEN_P(zval_ptr[3]) + Z_STRLEN_P(zval_ptr[4]), 0);
		if (!buf){
			php_error_docref(NULL, E_WARNING, "Hessian2Writer::writeDate alloc memory error");
			return;
		}

		p = buf;
		for(i=0; i<5; i++){
			memcpy(p, Z_STRVAL_P(zval_ptr[i]), Z_STRLEN_P(zval_ptr[i]));
			p += Z_STRLEN_P(zval_ptr[i]);
		}
	}

	RETURN_STRING(buf, 0);
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

	if (i_zend_is_true(value)){
		RETURN_STRING("T", 1);
	}else{
		RETURN_STRING("F", 1);
	}
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
	zval function_name, param1, param2;
	zval *params[2];
	char *buf, *p;
	int i;
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &value)) {
		return;
	}

	/*
		if($this->between($value, -16, 47)){
			return pack('c', $value + 0x90);
	*/

	ZVAL_STRING(&function_name, "pack", 1);
	if (BETWEEN(value, -16, 47)){
		zval *res;
		
		//return pack('c', $value + 0x90);
		ZVAL_STRING(&param1, "c", 1);
		ZVAL_LONG(&param2, value + 0x90);
		params[0] = &param1;
		params[1] = &param2;

		call_user_function(EG(function_table), NULL, &function_name, res, 2, params TSRMLS_DC);
		RETURN_ZVAL(res, 1, NULL);
	}else if(BETWEEN(value, -2048, 2047)){
		/*
			$b0 = 0xc8 + ($value >> 8);
			$stream = pack('c', $b0);
			$stream .= pack('c', $value);
			return $stream;
		*/

		ulong b0;
		zval *res[2];
		
		b0 = 0xc8 + (value >> 8);
		ZVAL_STRING(&param1, "c", 1);
		ZVAL_LONG(&param2, b0);
		call_user_function(EG(function_table), NULL, &function_name, res[0], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, value);
		call_user_function(EG(function_table), NULL, &function_name, res[1], 2, params TSRMLS_DC);

		RETURN_STRING(strcat(Z_STRVAL_P(res[0]), Z_STRVAL_P(res[1])), 1);
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
		zval *res[3];

		b0 = 0xd4 + (value >> 16);
		b1 = value >> 8;
		
		ZVAL_STRING(&param1, "c", 1);
		ZVAL_LONG(&param2, b0);
		params[0] = &param1;
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, res[0], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, b1);
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, res[1], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, value);
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, res[2], 2, params TSRMLS_DC);

		buf = pemalloc(Z_STRLEN_P(res[0]) + Z_STRLEN_P(res[1]) + Z_STRLEN_P(res[2]), 0);
		if (!buf){
			php_error_docref(NULL, E_WARNING, "Hessian2Writer::writeInt alloc memory error");
			return;
		}
		p = buf;
		for(i=0; i<3; i++){
			memcpy(p, Z_STRVAL_P(res[i]), Z_STRLEN_P(res[i]));
			p +=  Z_STRLEN_P(res[i]);
		}

		RETURN_STRING(buf, 0);
	}else{
		/*
			$stream = 'I';
			$stream .= pack('c', ($value >> 24));
			$stream .= pack('c', ($value >> 16));
			$stream .= pack('c', ($value >> 8));
			$stream .= pack('c', $value);
			return $stream;
		*/

		zval *res[4];

		ZVAL_STRING(&param1, "c", 1);
		ZVAL_LONG(&param2, value >> 24);
		params[0] = &param1;
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, res[0], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, value >> 16);
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, res[1], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, value >> 8);
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name,  res[2], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, value);
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, res[3], 2, params TSRMLS_DC);

		buf = pemalloc(Z_STRLEN_P(res[0]) + Z_STRLEN_P(res[1]) + Z_STRLEN_P(res[2]) + Z_STRLEN_P(res[3]), 0);
		if (!buf){
			php_error_docref(NULL, E_WARNING, "Hessian2Writer::writeInt alloc memory error");
			return;
		}
		p = buf;
		for(i=0; i<4; i++){
			memcpy(p, Z_STRVAL_P(res[i]), Z_STRLEN_P(res[i]));
			p +=  Z_STRLEN_P(res[i]);
		}

		RETURN_STRING(buf, 0);
	}
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


	//$len = HessianUtils::stringLength($value);
	ZVAL_STRING(&function_name, "HessianUtils::stringLength", 1);
	params[0] = value;
	call_user_function(EG(function_table), NULL, &function_name, len, 2, params TSRMLS_DC);

	if (Z_LVAL_P(len) < 32){
		/*
			return pack('C', $len) 
				. $this->writeStringData($value);
		*/

		zval *res[2];
		ZVAL_STRING(&function_name, "writeStringData", 1);
		params[0] = value;
		call_user_function(NULL, &self, &function_name, res[0], 1, params TSRMLS_DC);

		ZVAL_STRING(&function_name, "pack", 1);
		ZVAL_STRING(&param1, "C", 1);
		params[0] = &param1;
		params[1] = len;

		call_user_function(EG(function_table), NULL, &function_name, res[1], 1, params TSRMLS_DC);

		RETURN_STRING(strcat(Z_STRVAL_P(res[0]), Z_STRVAL_P(res[1])), 0);
	}else if (Z_LVAL_P(len) < 1024){
		/*
			$b0 = 0x30 + ($len >> 8);
			$stream = pack('C', $b0);
			$stream .= pack('C', $len);
			return $stream . $this->writeStringData($value);
		*/
		zval *res[3];
		
		
		ZVAL_STRING(&param1, "C", 1);
		ZVAL_LONG(&param2, 0x30 + (Z_LVAL_P(len) >> 8));
		params[0] = &param1;
		params[1] = &param2;

		call_user_function(EG(function_table), NULL, &function_name, res[0], 2, params TSRMLS_DC);

		params[1] = len;
		call_user_function(EG(function_table), NULL, &function_name, res[1], 2, params TSRMLS_DC);

		ZVAL_STRING(&function_name, "writeStringData", 1);
		params[0]= value;
		call_user_function(NULL, &self, &function_name, res[2], 1, params TSRMLS_DC);


		buf = pemalloc(Z_STRLEN_P(res[0]) + Z_STRLEN_P(res[1]) + Z_STRLEN_P(res[2]), 0);
		if (!buf){
			php_error_docref(NULL, E_WARNING, "Hessian2Writer::writeString alloc memory error");
			return;
		}
		p = buf;
		for(i=0; i<3; i++){
			memcpy(p, Z_STRVAL_P(res[i]), Z_STRLEN_P(res[i]));
			p +=  Z_STRLEN_P(res[i]);
		}

		RETURN_STRING(buf, 0);
	}else{
		/*
			$stream = '';
			$tag = 'S';
			$stream .= $tag . pack('n', $len);
			$stream .= $this->writeStringData($value);
			return $stream;
		*/

		zval *res[2];

		ZVAL_STRING(&param1, "n", 1);
		params[0] = &param1;
		params[1] = len;
		call_user_function(EG(function_table), NULL, &function_name, res[0], 2, params TSRMLS_DC);


		ZVAL_STRING(&function_name, "writeStringData", 1);
		params[0] = value;
		call_user_function(NULL, &self, &function_name, res[1], 1, params TSRMLS_DC);

		buf = pemalloc(Z_STRLEN_P(res[0]) + Z_STRLEN_P(res[1]) + 1, 0);
		if (!buf){
			php_error_docref(NULL, E_WARNING, "Hessian2Writer::writeString alloc memory error");
			return;
		}
		buf[0] = 'S';
		p = buf+1;
		for(i=0; i<2; i++){
			memcpy(p, Z_STRVAL_P(res[i]), Z_STRLEN_P(res[i]));
			p +=  Z_STRLEN_P(res[i]);
		}

		RETURN_STRING(buf, 0);
	}
}


/*
	Hessian2Writer::writeSmallString
*/
static PHP_METHOD(Hessian2Writer, writeSmallString)
{
	zval *self;
	zval  *value, *len;
	zval function_name, param1, param2;
	zval *params[2];
	
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
	call_user_function(EG(function_table), NULL, &function_name, len, 1, params TSRMLS_DC);

	if(Z_LVAL_P(len) < 32){
			//return pack('C', $len) . $this->writeStringData($value);
			zval  *res[2];
			
			ZVAL_STRING(&function_name, "pack", 1);
			ZVAL_STRING(&param1, "C", 1);
			params[0] = &param1;
			params[1] = len;
			
			call_user_function(NULL, &self, &function_name, res[0], 2, params TSRMLS_DC);

			
			ZVAL_STRING(&function_name, "writeStringData", 1);
			params[0] = value;
			call_user_function(NULL, &self, &function_name, res[1], 1, params TSRMLS_DC);

			RETURN_STRING(strcat(Z_STRVAL_P(res[0]), Z_STRVAL_P(res[1])), 1);
	}else{
		/*
			$b0 = 0x30 + ($len >> 8);
			$stream .= pack('C', $b0);
			$stream .= pack('C', $len);
			return $stream . $this->writeStringData($value);
		*/
		zval *res[3];
		char *buf, *p;
		int i;
		
		ZVAL_STRING(&param1, "C", 1);
		ZVAL_LONG(&param2, 0X30 + Z_LVAL_P(len) >> 8);
		params[0] = &param1;
		params[1] = &param2;
		call_user_function(NULL, &self, &function_name, res[0], 3, params TSRMLS_DC);

		ZVAL_LONG(&param2, Z_LVAL_P(len));
		params[1] = &param2;
		call_user_function(NULL, &self, &function_name, res[1], 3, params TSRMLS_DC);

		ZVAL_STRING(&function_name, "writeStringData", 1);
		params[0] = value;
		call_user_function(NULL, &self, &function_name, res[2], 1, params TSRMLS_DC);

		buf = pemalloc(Z_STRLEN_P(res[0]) + Z_STRLEN_P(res[1]) +  Z_STRLEN_P(res[2]), 0);
		if (!buf){
			php_error_docref(NULL, E_WARNING, "Hessian2Writer::writeSmallString alloc memory error");
			return;
		}
		p = buf;
		for(i=0; i<3; i++){
			memcpy(p, Z_STRVAL_P(res[i]), Z_STRLEN_P(res[i]));
			p +=  Z_STRLEN_P(res[i]);
		}

		RETURN_STRING(buf, 0);
	}
}


/*
	Hessian2Writer::writeStringData
*/
static PHP_METHOD(Hessian2Writer, writeStringData)
{
	zval *self;
	zval  *string, *res;
	zval function_name;
	zval *params[1];
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &string)) {
		return;
	}

	if (Z_TYPE_P(string) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "string must be a string");
		return;
	}

	//return HessianUtils::writeUTF8($string);
	ZVAL_STRING(&function_name, "HessianUtils::writeUTF8", 1);
	params[0] = string;

	call_user_function(NULL, &self, &function_name, res, 1, params TSRMLS_DC);

	RETURN_ZVAL(res, 1, NULL);
}

/*
	Hessian2Writer::writeDouble
*/
static PHP_METHOD(Hessian2Writer, writeDouble)
{
	zval *self;
	zval  *param_value, *res[5];
	double value, frac;
	zval function_name, param1, param2;
	zval *params[2];
	char *buf, *p;
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &param_value)) {
		return;
	}

	if (Z_TYPE_P(param_value) != IS_DOUBLE){
		php_error_docref(NULL, E_WARNING, "value must be a double");
		return;
	}

	value = Z_DVAL_P(param_value);


	/*
		$frac = abs($value) - floor(abs($value));
		if($value == 0.0){
			return pack('c', 0x5b);
		}
		if($value == 1.0){
			return pack('c', 0x5c);
		}
	*/

	frac = abs(value) - floor(abs(value));

	if (value == 0.0){
		ZVAL_STRING(&function_name, "pack", 1);
		ZVAL_STRING(&param1, "c", 1);
		ZVAL_LONG(&param2, 0x5b);
		params[0] = &param1;
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, res[0], 2, params TSRMLS_DC);

		RETURN_ZVAL(res[0], 1, NULL);
	}

	if (value == 1.0){
		ZVAL_STRING(&function_name, "pack", 1);
		ZVAL_STRING(&param1, "c", 1);
		ZVAL_LONG(&param2, 0x5c);
		params[0] = &param1;
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, res[0], 2, params TSRMLS_DC);

		RETURN_ZVAL(res[0], 1, NULL);
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
		call_user_function(EG(function_table), NULL, &function_name, res[0], 2, params TSRMLS_DC);

		params[1] = param_value;
		call_user_function(EG(function_table), NULL, &function_name, res[1], 2, params TSRMLS_DC);

		RETURN_STRING(strcat(Z_STRVAL_P(res[0]), Z_STRVAL_P(res[1])), 1);
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
		call_user_function(EG(function_table), NULL, &function_name, res[0], 2, params TSRMLS_DC);

		ZVAL_STRING(&function_name, "HessianUtils::floatBytes", 1);
		params[0] = &value;
		call_user_function(EG(function_table), NULL, &function_name, res[1], 1, params TSRMLS_DC);

		RETURN_STRING(strcat(Z_STRVAL_P(res[0]), Z_STRVAL_P(res[1])), 1);
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
		call_user_function(EG(function_table), NULL, &function_name, res[0], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, mills >> 24);
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, res[1], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, mills >> 16);
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, res[2], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, mills >> 8);
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, res[3], 2, params TSRMLS_DC);

		ZVAL_LONG(&param2, mills);
		params[1] = &param2;
		call_user_function(EG(function_table), NULL, &function_name, res[4], 2, params TSRMLS_DC);

		buf = pemalloc(Z_STRLEN_P(res[0]) + Z_STRLEN_P(res[1]) +  Z_STRLEN_P(res[2])
			+  Z_STRLEN_P(res[3]) +  Z_STRLEN_P(res[4]), 0);
		if (!buf){
			php_error_docref(NULL, E_WARNING, "Hessian2Writer::writeDouble alloc memory error");
			return;
		}
		p = buf;
		int i;
		for(i=0; i<5; i++){
			memcpy(p, Z_STRVAL_P(res[i]), Z_STRLEN_P(res[i]));
			p +=  Z_STRLEN_P(res[i]);
		}

		RETURN_STRING(buf, 0);
	}


	/*
		$stream = 'D';
		$stream .= HessianUtils::doubleBytes($value);
		return $stream;
	*/

	ZVAL_STRING(&function_name, "HessianUtils::doubleBytes", 1);
	params[0] = param_value;
	call_user_function(EG(function_table), NULL, &function_name, res[0], 2, params TSRMLS_DC);

	RETURN_STRING(strcat("D", Z_STRVAL_P(res[0])), 1);
}

/*
	Hessian2Writer::writeResource
*/
static PHP_METHOD(Hessian2Writer, writeResource)
{
	zval *self;
	zval  *handle, *type, *res[1];
	zval function_name, param1, param2;
	zval *params[1];
	char *buf, *p;
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &handle)) {
		return;
	}

	if (Z_TYPE_P(handle) != IS_RESOURCE){
		php_error_docref(NULL, E_WARNING, "handle must be a handle");
		return;
	}

	//$type = get_resource_type($handle);
	params[0] = handle;
	ZVAL_STRING(&function_name, "get_resource_type", 1);
	call_user_function(EG(function_table), NULL, &function_name, type, 1, params TSRMLS_DC);

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

	buf = Z_STRVAL_P(type);
	if (strncmp(buf, "file", 4) == 0 || strncmp(buf, "stream", 6) == 0){
		zval *content, function_name_fread, function_name_pack;
		ZVAL_STRING(&function_name, "feof", 1);
		ZVAL_STRING(&function_name_fread, "fread", 1);
		ZVAL_STRING(&function_name_pack, "pack", 1);
		params[0] = handle;
		call_user_function(EG(function_table), NULL, &function_name, res[0], 1, params TSRMLS_DC);
		buf = NULL;
		
		while(!i_zend_is_true(res[0])){
			ZVAL_LONG(&param1, 32768);
			params[0] = handle;
			params[1] = &param1;
			call_user_function(EG(function_table), NULL, &function_name, content, 2, params TSRMLS_DC);

			if (Z_STRLEN_P(content) < 15){
				/*
					$stream .= pack('C', $len + 0x20);
					$stream .= $content;
				*/
				ZVAL_STRING(&param1, "C", 1);
				ZVAL_LONG(&param2, Z_STRLEN_P(content) + 0x20);
				params[0] = &param1;
				params[1] = &param2;

				call_user_function(EG(function_table), NULL, &function_name, res[0], 2, params TSRMLS_DC);
				if (buf){
					buf = strcat(buf, Z_STRVAL_P(res[0]));
				}else{
					buf = Z_STRVAL_P(res[0]);
				}

				buf = strcat(buf, Z_STRVAL_P(content));
			}else{
				/*
					$tag = 'b';
					if(feof($handle))
						$tag = 'B';
					$stream .= $tag . pack('n', $len);
					$stream .= $content;
				*/

				params[0] = handle;
				call_user_function(EG(function_table), NULL, &function_name, res[0], 1, params TSRMLS_DC);

				ZVAL_STRING(&param1, "n", 1);
				ZVAL_LONG(&param2, Z_STRLEN_P(content));
				call_user_function(EG(function_table), NULL, &function_name_pack, res[1], 2, params TSRMLS_DC);
				if (i_zend_is_true(res[0])){
					buf = strcat(strcat("B", Z_STRVAL_P(res[1])), Z_STRVAL_P(content));
				}else{
					buf = strcat(strcat("b", Z_STRVAL_P(res[1])), Z_STRVAL_P(content));
				}
			}

			params[0] = handle;
			call_user_function(EG(function_table), NULL, &function_name, res[0], 1, params TSRMLS_DC);
		}

		RETURN_STRING(buf, 0);
	}else{
		zend_class_entry **ce_exception;
		
		zend_hash_find(CG(class_table), "exception", sizeof("exception"), (void **) &ce_exception);
		zend_throw_exception(ce_exception, sprintf("Cannot handle resource of type '%s'", Z_STRVAL_P(type))
			, 0 TSRMLS_DC);
	}
}


//Hessian2Writer functions
const zend_function_entry hessian2_writer_functions[] = {
	PHP_ME(Hessian2Writer, __construc, arginfo_hessian2_writer_construct, ZEND_ACC_PUBLIC)
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
