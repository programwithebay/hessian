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
ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_iterator_writer_write, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, list, "Iterator", 0) /* string */
	ZEND_ARG_INFO(0, writer) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_iterator_writer_is_iterator, 0, 0, 1)
	ZEND_ARG_INFO(0, object) /* string */
ZEND_END_ARG_INFO()







//entry
zend_class_entry *hessian2_iterator_writer_entry;


//list header
zval** hessian2_iterator_writer_list_header(zval *self, zval *writer, zval *type, zval *total)
{
	zval *res[2];
	zval terminate;
	zval *use_type;
	char *stream;
	zval function_name;
	zval *params[2];
	zval *z_stream;
	

	use_type = zend_read_property(NULL, self, ZEND_STRL("usetype"), 1 TSRMLS_DC);

	if (i_zend_is_true(use_type) && i_zend_is_true(type)){
		/*
			if($total !== false){ // typed fixed length
				$stream = 'V';
				$stream .= $writer->writeType($type);
				$stream .= $writer->writeInt($total);
			} else { // typed variable length
				$stream = "\x55"; 
				$stream .= $writer->writeType($type);
				$terminate = true;
			}
		*/

		if ( !((Z_TYPE_P(total) == IS_BOOL) && (0 == Z_BVAL_P(total))) ){
			zval *write_type_res, *write_int_res;
			ZVAL_STRING(&function_name, "writeType", 1);
			params[0] = type;
			call_user_function(NULL, &writer, &function_name, write_type_res, 1, params TSRMLS_DC);

			ZVAL_STRING(&function_name, "writeInt", 1);
			params[0] = total;
			call_user_function(NULL, &writer, &function_name, write_int_res, 1, params TSRMLS_DC);

			stream = pemalloc(1 + Z_STRLEN_P(write_type_res) + Z_STRLEN_P(write_int_res) + 1, 0);
			stream[0] = 'V';
			mempcy(stream+1, Z_STRVAL_P(write_type_res), Z_STRLEN_P(write_type_res));
			mempcy(stream+1+ Z_STRLEN_P(write_type_res), Z_STRVAL_P(write_int_res), Z_STRLEN_P(write_int_res));
			stream[1 + Z_STRLEN_P(write_type_res) + Z_STRLEN_P(write_int_res) + 1] = 0;
		}else{
			zval *write_type_res;
			ZVAL_STRING(&function_name, "writeType", 1);
			params[0] = type;
			call_user_function(NULL, &writer, &function_name, write_type_res, 1, params TSRMLS_DC);

			stream = pemalloc(1 + Z_STRLEN_P(write_type_res) + 1, 0);
			stream[0] = 55;
			mempcy(stream+1, Z_STRVAL_P(write_type_res), Z_STRLEN_P(write_type_res));
			stream[1 + Z_STRLEN_P(write_type_res) + 1] = 0;
			ZVAL_BOOL(&terminate, 1);
		}
	}else{
		if ( !((Z_TYPE_P(total) == IS_BOOL) && (0 == Z_BVAL_P(total))) ){
			zval *write_int_res;
	
			ZVAL_STRING(&function_name, "writeInt", 1);
			params[0] = total;
			call_user_function(NULL, &writer, &function_name, write_int_res, 1, params TSRMLS_DC);

			stream = pemalloc(1 + Z_STRLEN_P(write_int_res) + 1, 0);
			stream[0] = 58;
			mempcy(stream+1, Z_STRVAL_P(write_int_res), Z_STRLEN_P(write_int_res));
			stream[1 +  Z_STRLEN_P(write_int_res) + 1] = 0;
		}else{
			stream = pemalloc(2, 0);
			stream[0] =57;
			stream[1] = 0;
			
			ZVAL_BOOL(&terminate, 1);
		}
	}

	ALLOC_ZVAL(z_stream);
	ZVAL_STRING(z_stream, stream, 0);
	res[0] = z_stream;
	res[1] = &terminate;
}


/*
	Hessian2IteratorWriter::write
*/
static PHP_METHOD(Hessian2IteratorWriter, write)
{
	zval *list, *writer;
	zval *self;
	zval function_name;
	zval *params[2];
	zval total;
	HashPosition pos;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &list, &writer)) {
		return;
	}
	self = getThis();

	/*
		$writer->logMsg('iterator writer');
		$writer->refmap->objectlist[] = $list; 
	*/
	zval msg;
	ZVAL_STRING(&function_name, "logMsg", 1);
	ZVAL_STRING(&msg, "iterator writer", 1);
	params[0] = &msg;

	call_user_function(NULL, &writer, &function_name, NULL, 1, params TSRMLS_DC);

	zval *ref_map, *obj_list;
	ref_map = zend_read_property(NULL, writer, ZEND_STRL("refmap"), 1 TSRMLS_DC);
	obj_list = zend_read_property(NULL, ref_map, ZEND_STRL("objectlist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(obj_list) != IS_ARRAY){
		ALLOC_ZVAL(obj_list);
		array_init_size(obj_list, 2);
	}

	zend_hash_next_index_insert(Z_ARRVAL_P(obj_list), &list, sizeof(zval**), NULL);
	zend_update_property(NULL, ref_map,  ZEND_STRL("objectlist"), obj_list TSRMLS_DC);



	//$total = $this->getCount($list);
	zend_class_entry *ce_countable;
	
	ce_countable = zend_fetch_class("Countable", strlen("Countable") -1, 0 TSRMLS_DC);
	if (instanceof_function(Z_OBJCE_P(list), ce_countable TSRMLS_DC)){
		ZVAL_STRING(&function_name, "count", 1);
		params[0]= list;
		call_user_function(EG(function_table), NULL, &function_name, &total, 1, params TSRMLS_DC);
	}else{
		ZVAL_BOOL(&total, 0);
	}

	/*
		$class = get_class($list);
		$type = $writer->typemap->getRemoteType($class);
		
		$mappedType = $type ? $type : $class; // OJO con esto
	*/

	char *class;
	zval z_class;
	zend_uint class_name_len;
	zval *type_map;
	zval *type;
	zval *mapped_type;
	zval *stream;

	
	zend_get_object_classname(list, (const char**)&class, &class_name_len TSRMLS_CC);
	type_map = zend_read_property(NULL, writer, ZEND_STRL("typemap"), 1 TSRMLS_DC);
	ZVAL_STRING(&function_name, "getRemoteType", 1);
	params[0] = &z_class;
	call_user_function(NULL, &type_map, &function_name, type, 1, params TSRMLS_DC);
	if (i_zend_is_true(type)){
		mapped_type = type;
	}else{
		mapped_type = &z_class;
	}

	//$islist = HessianUtils::isListIterate($list);
	zval *is_list;
	ZVAL_STRING(&function_name, "HessianUtils::isListIterate", 1);
	params[0] = list;
	call_user_function(EG(function_table), NULL, &function_name, is_list, 1, params TSRMLS_DC);

	if (i_zend_is_true(is_list)){
		zval **list_header_res;
		
		int add_len=0;
		zval **arr_write_value;
		zval *src_entry;
		int len=0, i;
		char *buf;
		zval *terminate;
		uint offset;
		
		
		/*
		list($stream, $terminate) = $this->listHeader($writer, $mappedType, $total);
			foreach($list as $value){
				$stream .= $writer->writeValue($value);
			}
			if($terminate)
				$stream .= 'Z';
		*/

		list_header_res = hessian2_iterator_writer_list_header(self, writer, mapped_type, &total);
		arr_write_value = pemalloc(zend_hash_num_elements(Z_ARRVAL_P(list)) * sizeof(zval*), 0);

		ZVAL_STRING(&function_name, "writeValue", 1);
		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(list), &pos);
		while (zend_hash_get_current_data_ex(Z_ARRVAL_P(list), (void **)&src_entry, &pos) == SUCCESS) {
			params[0] = src_entry;
			call_user_function(NULL, &writer, &function_name, arr_write_value[i], 1,  params TSRMLS_DC);
			add_len += Z_STRLEN_P(arr_write_value[i]);
			++len;
			zend_hash_move_forward_ex(Z_ARRVAL_P(list), &pos);
		}

		//alloc memory
		stream = list_header_res[0];
		terminate = list_header_res[1];
		buf = Z_STRVAL_P(stream);
		buf = perealloc(buf, Z_STRLEN_P(stream) + add_len + 1, 0);
		if (!buf){
			php_error_docref(NULL, E_ERROR, "Hessian2IteratorWriter::write alloc memory error");
			return;
		}
		buf += Z_STRLEN_P(stream);
		offset = 0;
		for(i=0; i<len; i++){
			memcpy(buf+offset, Z_STRVAL_P(arr_write_value[i]), Z_STRLEN_P(arr_write_value[i]));
			offset +=  Z_STRLEN_P(arr_write_value[i]);
		}
		buf[offset] = 'Z';
		buf[offset + 1] = 0;

		ZVAL_STRING(stream, buf, 0);
	}else{
		/*
			if($this->usetype && $mappedType){
				$stream = 'M';
				$stream .= $writer->writeType($mappedType);
			} else {
				$stream = 'H';
			}
			
			foreach($list as $key => $value){
				$stream .= $writer->writeValue($key);
				$stream .= $writer->writeValue($value);
			}
			$stream .= 'Z';
		*/

		zval *use_type, *write_type_res;
		uint buf_len;
		char *buf;
		
		use_type = zend_read_property(NULL, writer, ZEND_STRL("usetype"), 1 TSRMLS_DC);
		if (i_zend_is_true(use_type) && (mapped_type)){
			zval *write_type_res;
			ZVAL_STRING(&function_name, "writeType", 1);
			params[0] = mapped_type;
			call_user_function(NULL, &writer, &function_name, write_type_res, 1, params TSRMLS_DC);
			
			buf_len = 1 + Z_STRLEN_P(write_type_res);
			buf = pemalloc(buf_len + 1, 0);
			buf[0] = 'M';
			memcpy(buf+1, Z_STRVAL_P(write_type_res), Z_STRLEN_P(write_type_res));
			buf[buf_len] = 0;
			
		}else{
			buf = pemalloc(2, 0);
			buf[0] = 'H';
			buf[1] = 0;
			buf_len = 1;
		}

		char *str_index;
		ulong num_index;
		zval *src_entry;
		zval **arr_write_res;
		int i;
		zval z_key;
		uint write_len=0, offset=0;


		arr_write_res = pemalloc(2 * sizeof(zval *) * zend_hash_num_elements(Z_ARRVAL_P(list)), 0);
		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(list), &pos);
		ZVAL_STRING(&function_name, "writeValue", 1);
		while (zend_hash_get_current_data_ex(Z_ARRVAL_P(list), (void **)&src_entry, &pos) == SUCCESS) {
			zend_hash_get_current_key(Z_ARRVAL_P(list), &str_index, &num_index, 0);
			ZVAL_STRING(&z_key, str_index, 1);
			call_user_function(NULL, &writer, &function_name, arr_write_res[i], 1, params TSRMLS_DC);
			write_len += Z_STRLEN_P(arr_write_res[i]);
			++i;
			ZVAL_STRING(&z_key, Z_STRVAL_P(src_entry), 1);
			call_user_function(NULL, &writer, &function_name, arr_write_res[i], 1, params TSRMLS_DC);
			write_len += Z_STRLEN_P(arr_write_res[i]);
			++i;
			
			zend_hash_move_forward_ex(Z_ARRVAL_P(list), &pos);
		}


		buf = perealloc(buf, buf_len+write_len + 1, 0);
		buf += buf_len;
		for(i=0; i<2*zend_hash_num_elements(Z_ARRVAL_P(list)); i++){
			memcpy(buf+offset, Z_STRVAL_P(arr_write_res[i]), Z_STRLEN_P(arr_write_res[i]));
			offset += Z_STRLEN_P(arr_write_res[i]);
		}
		buf[offset++] = 'Z';
		buf[offset] = 0;
	}

	object_init_ex(return_value, hessian_stream_result_entry);
	ZVAL_STRING(&function_name, "__construct", 1);
	params[0] = stream;
	call_user_function(NULL, &return_value, &function_name, NULL, 1, params TSRMLS_DC);
}



/*
	Hessian2IteratorWriter::isIterator
*/
static PHP_METHOD(Hessian2IteratorWriter, isIterator)
{
	zval *object;
	zval *self;
	zend_class_entry *ce_itertor;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &object)) {
		return;
	}
	self = getThis();
	if (Z_TYPE_P(object) != IS_OBJECT){
		RETURN_FALSE;
	}

	ce_itertor = zend_fetch_class("Iterator", strlen("Iterator")-1, 0);
	if (instanceof_function(Z_OBJCE_P(object), ce_itertor TSRMLS_DC)){
		RETURN_TRUE;
	}else{
		RETURN_FALSE;
	}
}




//Hessian2IteratorWriter functions
const zend_function_entry hessian2_iterator_writer_functions[] = {
	PHP_ME(Hessian2IteratorWriter, isIterator, arginfo_hessian2_iterator_writer_is_iterator, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2IteratorWriter, write, arginfo_hessian2_iterator_writer_write, ZEND_ACC_PUBLIC)
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
