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
ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_write_call, 0, 0, 2)
	ZEND_ARG_INFO(0, method) /* string */
	ZEND_ARG_INFO(0, params) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_write_fault, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, ex, "Exception", 0) /* string */
	ZEND_ARG_INFO(0, detail) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_write_reply, 0, 0, 1)
	ZEND_ARG_INFO(0, value) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian2_writer_write_version, 0, 0, 0)
ZEND_END_ARG_INFO()



//entry
zend_class_entry *hessian2_service_writer_entry;


/*
	Hessian2ServiceWriter::writeVersion
*/
void hessian2_service_writer_write_version(zval *return_value)
{
	char buf[3];

	//return "H\x02\x00";
	buf[0] = 'H';
	buf[1] = 2;
	buf[2] = 0;

	RETURN_STRINGL(buf,  3, 1);
}




/*
	Hessian2ServiceWriter::writeCall
*/
void hessian2_service_writer_write_call(zval *self, zval *method, zval *params, zval *return_value)
{
	zval write_string_res, write_int_res;
	zval *stream;
	zval function_name, param1;
	zval *call_params[2];
	zend_uchar *buf;
	ulong buf_size;
	int i, params_count;
	zval retval;


	/*
	char msg_buf[100];
	//todo:call
	sprintf(msg_buf, "call %s", Z_STRVAL_P(method));
	ZVAL_STRING(&param1, msg_buf, 1);
	call_params[0] = &param1;
	ZVAL_STRING(&function_name, "logMsg", 1);
	call_user_function(NULL, &self, &function_name, &retval, 1, call_params TSRMLS_DC);
	*/
	

	//$stream = $this->writeVersion();
	ALLOC_ZVAL(stream);
	hessian2_service_writer_write_version(stream);

	if (Z_TYPE_P(params) != IS_ARRAY){
		params_count = 0;
	}else{
		params_count = zend_hash_num_elements(Z_ARRVAL_P(params));
	}
	buf_size = Z_STRLEN_P(stream) + 1;

	
	//$stream .= 'C';
	//$stream .= $this->writeString($method);
	hessian2_writer_write_string(self, method, &write_string_res);


	//$stream .= $this->writeInt(count($params));
	hessian2_writer_write_int(self, params_count, &write_int_res);

	buf_size += Z_STRLEN(write_string_res);
	buf_size += Z_STRLEN(write_int_res);

	
	zval *params_ptr;

	if (params_count > 0){
		params_ptr = (zval *)pemalloc(params_count * sizeof(zval), 0);
		if (!params_ptr){
			php_error_docref(NULL, E_ERROR, "Hessian2ServiceWriter::writeCall alloc memory error");
			return;
		}

		/*
			foreach($params as $param){
				$stream .= $this->writeValue($param);
			}
		*/
		i=0;
		HashPosition pos;
		zval **src_entry;

		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(params), &pos);
		while (zend_hash_get_current_data_ex(Z_ARRVAL_P(params), (void **)&src_entry, &pos) == SUCCESS) {
			hessian2_writer_write_value(self,  *src_entry, &params_ptr[i]);
			
			//call_user_function(NULL, &self, &function_name, params_ptr[i], 1, call_params TSRMLS_DC);
			buf_size += Z_STRLEN(params_ptr[i]);
			i++;
			zend_hash_move_forward_ex(Z_ARRVAL_P(params), &pos);
		}
	}
	
	buf = pemalloc(buf_size, 0);
	if (!buf){
		php_error_docref(NULL, E_ERROR, "Hessian2ServiceWriter::writeCall alloc memory error");
		return;
	}

	char *p;
	p = buf;
	memcpy(p, Z_STRVAL_P(stream), Z_STRLEN_P(stream));
	p += Z_STRLEN_P(stream);
	zval_dtor(stream);
	FREE_ZVAL(stream);
	*p = 'C';
	++p;

	
	memcpy(p, Z_STRVAL(write_string_res), Z_STRLEN(write_string_res));
	p += Z_STRLEN(write_string_res);
	memcpy(p, Z_STRVAL(write_int_res), Z_STRLEN(write_int_res));
	p += Z_STRLEN(write_int_res);
	zval_dtor(&write_string_res);

	//params
	for(i=0; i<params_count; i++){
		memcpy(p, Z_STRVAL(params_ptr[i]), Z_STRLEN(params_ptr[i]));
		p += Z_STRLEN(params_ptr[i]);
		//zval_dtor(&params_ptr[i]);
	}
	if (params_count > 0){
		pefree(params_ptr, 0);
	}
	
	RETURN_STRINGL(buf,  buf_size, 0);
}



/*
	Hessian2ServiceWriter::writeCall
*/
static PHP_METHOD(Hessian2ServiceWriter, writeCall)
{
	zval *self;
	zval *method, *params;

	self = getThis();
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "za", &method, &params)) {
		return;
	}
	if (Z_TYPE_P(method) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "method must be an string");
		return;
	}

	hessian2_service_writer_write_call(self,  method, params, return_value);
}



/*
	Hessian2ServiceWriter::writeFault
*/
static PHP_METHOD(Hessian2ServiceWriter, writeFault)
{
	zval *self;
	zval *stream, arr, *ex, *detail;
	zval function_name, param1;
	zval *params[2];
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &ex, &detail)) {
		return;
	}

	ZVAL_STRING(&param1, "fault", 1);
	params[0] = &param1;
	ZVAL_STRING(&function_name, "logMsg", 1);
	call_user_function(NULL, &self, &function_name, NULL, 1, params TSRMLS_DC);

	/*
		$stream = $this->writeVersion();
		$stream .= 'F';
		$arr['message'] = $ex->getMessage();
		$arr['code'] = $ex->getCode();
		$arr['file'] = $ex->getFile();
		$arr['trace'] = $ex->getTraceAsString();
		$arr['detail'] = $detail;
		
	*/
	zval *message, *code, *file, *trace;

	ZVAL_STRING(&function_name, "writeVersion", 1);
	call_user_function(NULL, &self, &function_name, stream, 0, params TSRMLS_DC);

	array_init_size(&arr, 5);
	
	ZVAL_STRING(&function_name, "getMessage", 1);
	call_user_function(NULL, &ex, &function_name, message,  0, params TSRMLS_DC);
	zend_hash_add(Z_ARRVAL(arr), "message", 7, &message, sizeof(zval *), NULL);

	
	ZVAL_STRING(&function_name, "getCode", 1);
	call_user_function(NULL, &ex, &function_name, code,  0, params TSRMLS_DC);
	zend_hash_add(Z_ARRVAL(arr), "code", 4, &code, sizeof(zval *), NULL);

	ZVAL_STRING(&function_name, "getFile", 1);
	call_user_function(NULL, &ex, &function_name, file,  0, params TSRMLS_DC);
	zend_hash_add(Z_ARRVAL(arr), "file", 4, &file, sizeof(zval *), NULL);

	ZVAL_STRING(&function_name, "getTraceAsString", 1);
	call_user_function(NULL, &ex, &function_name, trace,  0, params TSRMLS_DC);
	zend_hash_add(Z_ARRVAL(arr), "trace", 5, &trace, sizeof(zval *), NULL);

	zend_hash_add(Z_ARRVAL(arr), "detail", 6, &detail, sizeof(zval *), NULL);

	//$stream .= $this->writeMap($arr);

	zval *write_map_res;
	params[0] = &arr;
	ZVAL_STRING(&function_name, "writeMap", 1);
	call_user_function(NULL, &self, &function_name, write_map_res,  1, params TSRMLS_DC);

	char *buf, *p;
	buf = pemalloc(Z_STRLEN_P(stream) + 1 + Z_STRLEN_P(write_map_res), 1);
	if (!buf){
		php_error_docref(NULL, E_ERROR, "Hessian2ServiceWriter::writeFault alloc memory error");
		return;
	}

	p = buf;
	memcpy(p, Z_STRVAL_P(stream), Z_STRLEN_P(stream));
	*p = 'F';
	++p;
	memcpy(p, Z_STRVAL_P(write_map_res), Z_STRLEN_P(write_map_res));

	RETURN_STRING(buf, 0);
}


/*
	Hessian2ServiceWriter::writeReply
*/
static PHP_METHOD(Hessian2ServiceWriter, writeReply)
{
	zval *self, *value, stream;
	zval function_name;
	zval param1;
	zval *params[2];
	int buf_len;
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value)) {
		return;
	}


	/*
	ZVAL_STRING(&param1, "reply", 1);
	params[0] = &param1;
	ZVAL_STRING(&function_name, "logMsg", 1);
	call_user_function(NULL, &self, &function_name, NULL, 1, params TSRMLS_DC);
	*/
	
	/*
		$stream = $this->writeVersion();
		$stream .= 'R';
		$stream .= $this->writeValue($value);
		return $stream;
	*/
	hessian2_service_writer_write_version(&stream);


	//write value
	zval write_value_res;
	params[0] = value;
	hessian2_writer_write_value(self, value, &write_value_res);


	char *buf, *p;
	buf_len = Z_STRLEN(stream) + 1 + Z_STRLEN(write_value_res);
	buf = pemalloc(buf_len, 0);
	if (!buf){
		php_error_docref(NULL, E_ERROR, "Hessian2ServiceWriter::writeReply alloc memory error");
		return;
	}

	p = buf;
	memcpy(p, Z_STRVAL(stream), Z_STRLEN(stream));
	*p = 'R';
	++p;
	memcpy(p, Z_STRVAL(write_value_res), Z_STRLEN(write_value_res));

	RETURN_STRINGL(buf, buf_len, 0);

}


/*
	Hessian2ServiceWriter::writeVersion
*/
static PHP_METHOD(Hessian2ServiceWriter, writeVersion)
{
	hessian2_service_writer_write_version(return_value);
}


//Hessian2ServiceWriter functions
const zend_function_entry hessian2_service_writer_functions[] = {
	PHP_ME(Hessian2ServiceWriter, writeCall, arginfo_hessian2_writer_write_call, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2ServiceWriter, writeFault, arginfo_hessian2_writer_write_fault, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2ServiceWriter, writeReply, arginfo_hessian2_writer_write_reply, ZEND_ACC_PUBLIC)
	PHP_ME(Hessian2ServiceWriter, writeVersion, arginfo_hessian2_writer_write_version, ZEND_ACC_PUBLIC)
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
