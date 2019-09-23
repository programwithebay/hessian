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


//params
ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_stream_construct, 0, 0, 2)
	ZEND_ARG_INFO(0, data) /* string */
	ZEND_ARG_INFO(0, length) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_stream_set_stream, 0, 0, 2)
	ZEND_ARG_INFO(0, data) /* string */
	ZEND_ARG_INFO(0, length) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_stream_peek, 0, 0, 2)
	ZEND_ARG_INFO(0, count) /* string */
	{"count", sizeof("count")-1,NULL, 0, 0, 0, 1, 0 },
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_stream_read, 0, 0, 1)
	ZEND_ARG_INFO(0, count) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_stream_readall, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_stream_write, 0, 0, 1)
	ZEND_ARG_INFO(0, data) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_stream_close, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_stream_flush, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_stream_get_data, 0, 0, 0)
ZEND_END_ARG_INFO()



//entry
zend_class_entry *hessian_stream_entry;


//set hessian stream
void hessian_stream_set_stream(zval *self, zval *data)
{
	zval *len, *pos;
	
	zend_update_property(NULL, self, ZEND_STRL("bytes"), data TSRMLS_DC);
	//len
	len = zend_read_property(NULL, self, ZEND_STRL("len"), 1 TSRMLS_DC);
	Z_LVAL_P(len) = Z_STRLEN_P(data);
	zend_update_property(NULL, self, ZEND_STRL("len"), len TSRMLS_DC);

	//pos
	pos = zend_read_property(NULL, self, ZEND_STRL("pos"), 1 TSRMLS_DC);
	ZVAL_LONG(pos, 0);
	//zend_update_property(NULL, self, ZEND_STRL("pos"), pos TSRMLS_DC);
}


/*
	HessianStream::__construct
*/
static PHP_METHOD(HessianStream, __construct)
{
	zval *self;
	zval *data, *length;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &data) == FAILURE) {
		RETURN_FALSE;
	}
	if (Z_TYPE_P(data) != IS_STRING){
		RETURN_FALSE;
	}
	/*
	if ($data)
            $this->setStream($data, $length);
       */
	self = getThis();
	if (i_zend_is_true(data)){
		hessian_stream_set_stream(self, data);
    }
}

/*
	HessianStream::setStream
*/
static PHP_METHOD(HessianStream, setStream)
{
	zval *self;
	zval *data;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &data) == FAILURE) {
		RETURN_FALSE;
	}
	if (Z_TYPE_P(data) != IS_STRING){
		RETURN_FALSE;
	}

	/*
	 	$this->bytes = $data;
        	$this->len = strlen($data);
        	$this->pos = 0;
       */
	self = getThis();
	hessian_stream_set_stream(self, data);
}

/*
	HessianStream::peek
*/
static PHP_METHOD(HessianStream, peek)
{
	zval *self, *arg_pos;
	long count, pos;
	zval *bytes;
	char *buf, *ret_buf;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lz", &count, &arg_pos) == FAILURE) {
		RETURN_FALSE;
	}

	/*
	 	$this->bytes = $data;
        	$this->len = strlen($data);
        	$this->pos = 0;
       */
	self = getThis();
	if (Z_TYPE_P(arg_pos) == IS_NULL){
		zval *z_pos;
		z_pos = zend_read_property(NULL, self, ZEND_STRL("pos"), 1 TSRMLS_DC);
		pos  = Z_LVAL_P(z_pos);
	}else{
		pos = Z_LVAL_P(arg_pos);
	}
	bytes = zend_read_property(NULL, self, ZEND_STRL("bytes"), 1 TSRMLS_DC);
	if (Z_TYPE_P(bytes) != IS_STRING || (Z_STRLEN_P(bytes) < 1)){
		RETURN_FALSE;
	}

	buf = Z_STRVAL_P(bytes);
	if (pos >= Z_STRLEN_P(bytes)){
		RETURN_FALSE;
	}
	if ((Z_STRLEN_P(bytes) -  pos) < count){
		count = Z_STRLEN_P(bytes) -  pos;
	}
	ret_buf = pemalloc(count+1, 0);
	memcpy(ret_buf, buf+pos, count);
	ret_buf[count] = 0;
	RETURN_STRING(ret_buf, 0);
}


/*
	HessianStream::read
*/
int hessian_stream_read(zval *self, long count, zval *return_value)
{
	zval  *z_pos;
	long pos, len;
	zval *z_bytes;
	char *buf, *ret_buf;
	


	if (count < 1){
		return;
	}
	/*
	 	$portion = substr($this->bytes, $this->pos, $count);
	        $read = strlen($portion);
	        $this->pos += $read;
	        if ($read < $count) {
	            if ($this->pos == 0)
	                throw new Exception('Empty stream received!');
	            else
	                throw new Exception('read past end of stream: ' . $this->pos);
	        }
	        return $portion;
       */

	z_bytes = zend_read_property(NULL, self, ZEND_STRL("bytes"), 1 TSRMLS_DC);
	z_pos = zend_read_property(NULL, self, ZEND_STRL("pos"), 1 TSRMLS_DC);
	pos = Z_LVAL_P(z_pos);
	
	buf = Z_STRVAL_P(z_bytes);
	
	if ((pos + count) > Z_STRLEN_P(z_bytes)){
		//zend_error(E_ERROR, "read past end of stream:");
		RETURN_FALSE;
	}

	ret_buf = pemalloc(count+1, 0);
	memcpy(ret_buf, buf+pos, count);
	ret_buf[count] = 0;

	//update pos
	pos += count;
	Z_LVAL_P(z_pos) = pos;
	zend_update_property(NULL, self, ZEND_STRL("pos"), z_pos TSRMLS_DC);
	
	RETVAL_STRING(ret_buf, 0);
	return SUCCESS;
}




/*
	HessianStream::read
*/
static PHP_METHOD(HessianStream, read)
{
	zval *self;
	long count;

	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &count) == FAILURE) {
		RETURN_FALSE;
	}

	self = getThis();
	hessian_stream_read(self, count, return_value);
}


/*
	HessianStream::readAll
*/
static PHP_METHOD(HessianStream, readAll)
{
	zval *self, *z_pos, *z_len;
	zval *z_bytes;

	self = getThis();
	z_bytes = zend_read_property(NULL, self, ZEND_STRL("bytes"), 1 TSRMLS_DC);
	z_pos = zend_read_property(NULL, self, ZEND_STRL("pos"), 1 TSRMLS_DC);
	z_len = zend_read_property(NULL, self, ZEND_STRL("len"), 1 TSRMLS_DC);
	Z_LVAL_P(z_pos) = Z_LVAL_P(z_len);
	zend_update_property(NULL, self, ZEND_STRL("pos"), z_pos TSRMLS_DC);

	RETURN_STRING(Z_STRVAL_P(z_bytes), 1);
}


/*
	HessianStream::write
*/
static PHP_METHOD(HessianStream, write)
{
	zval *self, *data;
	zval *len, *bytes;
	char *buf;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &data) == FAILURE) {
		RETURN_FALSE;
	}
	if (Z_TYPE_P(data) != IS_STRING){
		RETURN_FALSE;
	}
	self = getThis();

	/*
	 $bytes = str_split($data);
        $this->len += count($bytes);
        $this->bytes = array_merge($this->bytes, $bytes);
      */
    len = zend_read_property(NULL, self, ZEND_STRL("len"), 1 TSRMLS_DC);
	Z_LVAL_P(len) += Z_STRLEN_P(data);
	bytes = zend_read_property(NULL, self, ZEND_STRL("bytes"), 1 TSRMLS_DC);
	if (Z_STRLEN_P(bytes) > 0){
		buf = pemalloc(Z_STRLEN_P(data) + Z_STRLEN_P(bytes), 0);
		if (!buf){
			php_error_docref(NULL, E_ERROR, "Hessian::write alloc memory error");
			return;
		}
		memcpy(buf, Z_STRVAL_P(bytes), Z_STRLEN_P(bytes));
		memcpy(buf+Z_STRLEN_P(bytes), Z_STRVAL_P(data), Z_STRLEN_P(data));
		buf[Z_STRLEN_P(bytes)+Z_STRLEN_P(data)] = 0;
		Z_STRVAL_P(bytes) = buf;
		Z_STRLEN_P(bytes) = Z_STRLEN_P(bytes)+Z_STRLEN_P(data);
	}else{
		buf = pemalloc(Z_STRLEN_P(data), 0);
		memcpy(buf, Z_STRVAL_P(data), Z_STRLEN_P(data));
		buf[Z_STRLEN_P(data)] = 0;
		Z_STRVAL_P(bytes) = buf;
		Z_STRLEN_P(bytes) = Z_STRLEN_P(data);
	}
}


/*
	HessianStream::close
*/
static PHP_METHOD(HessianStream, close)
{
}


/*
	HessianStream::flush
*/
static PHP_METHOD(HessianStream, flush)
{
}

/*
	HessianStream::getData
*/
static PHP_METHOD(HessianStream, getData)
{
	zval *self, *bytes;
	
	self = getThis();

	/*
	 $bytes = str_split($data);
        $this->len += count($bytes);
        $this->bytes = array_merge($this->bytes, $bytes);
      */
    bytes = zend_read_property(NULL, self, ZEND_STRL("bytes"), 1 TSRMLS_DC);
	if ((Z_TYPE_P(bytes) == IS_STRING) && (Z_STRLEN_P(bytes) > 0) ){
		RETURN_STRING(Z_STRVAL_P(bytes), 1);
	}
	RETURN_FALSE;
}


//HessianStream functions
const zend_function_entry hessian_stream_functions[] = {
	PHP_ME(HessianStream, __construct, 	arginfo_hessian_stream_construct,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianStream, setStream, 	arginfo_hessian_stream_set_stream,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianStream, peek, 	arginfo_hessian_stream_peek,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianStream, read, 	arginfo_hessian_stream_read,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianStream, readAll, 	arginfo_hessian_stream_readall,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianStream, write, 	arginfo_hessian_stream_write,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianStream, close, 	arginfo_hessian_stream_close,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianStream, flush, 	arginfo_hessian_stream_flush,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianStream, getData, 	arginfo_hessian_stream_get_data,		ZEND_ACC_PUBLIC)
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
