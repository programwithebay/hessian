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



ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_buffered_stream_construct, 0, 0, 2)
	ZEND_ARG_INFO(0, fp) /* string */
	ZEND_ARG_INFO(0, bufferSize) /* array */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_buffered_stream_setstream, 0, 0, 1)
	ZEND_ARG_INFO(0, fp) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_buffered_stream_peek, 0, 0, 2)
	ZEND_ARG_INFO(0, count) /* string */
	ZEND_ARG_INFO(0, pos) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_buffered_stream_peek, 0, 0, 1)
	ZEND_ARG_INFO(0, count) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_buffered_stream_eof, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_buffered_stream_peek, 0, 0, 1)
	ZEND_ARG_INFO(0, data) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_buffered_stream_readall, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_buffered_stream_flush, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_buffered_stream_get_data, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_buffered_stream_get_close, 0, 0, 0)
ZEND_END_ARG_INFO()



zend_class_entry *hessian_buffered_stream_entry;


/*
*/
void hessian_buffered_stream_ensure_buffer_size(){
}


//ensure memory enough
//long size:new total buffer size
void hessian_ensure_buffer_size(zval *this, long size){
	zval *property;
	php_stream *stream;
	long len;
	long buffer_size, read_length;
	char *bytes;
	char *buf;
	long buffer_pos, buffer_alloc_size, new_buffer_alloc_size;
	
	

	
	//buffer
	property = zend_read_property(hessian_buffered_stream_entry, this, ZEND_STRL("bytes"), 0 TSRMLS_DC);
	bytes = (char*)property;
	//buffer pos
	property = zend_read_property(hessian_buffered_stream_entry, this, ZEND_STRL("bufferSize"), 0 TSRMLS_DC);
	buffer_size = Z_LVAL_P(property);
	property = zend_read_property(hessian_buffered_stream_entry, this, ZEND_STRL("bufferAllocSize"), 0 TSRMLS_DC);
	buffer_alloc_size = (long)property;
	property = zend_read_property(hessian_buffered_stream_entry, this, ZEND_STRL("bufferPos"), 0 TSRMLS_DC);
	buffer_pos = Z_STRVAL_P(property);
	
	if (size <= buffer_alloc_size){
		return;
	}
	
	long multi;
	multi =  size / buffer_size + 1;
	new_buffer_alloc_size = multi * buffer_size;
	bytes = perealloc(bytes, new_buffer_alloc_size, 0);
	if (!bytes){
		php_error_docref(NULL, E_ERROR, "BufferInputStream alloc memory error");
	}
	zend_update_property(hessian_buffered_stream_entry, this, ZEND_STRL("bytes"), (zval*)bytes TSRMLS_DC);
	zend_update_property(hessian_buffered_stream_entry, this, ZEND_STRL("bufferAllocSize"), new_buffer_alloc_size TSRMLS_DC);
		
}

/*
	check read
*/
zend_bool hessian_buffered_stream_check_read(zval *this, long new_pos){
	zval *fp, *z_len;
	zval *property;
	php_stream *stream;
	long len;
	long buffer_size, read_length;
	char *bytes;
	char *buf;
	long buffer_pos, buffer_alloc_size, new_buffer_alloc_size;
	
	
	fp = zend_read_property(hessian_buffered_stream_entry, this, ZEND_STRL("fp"), 0 TSRMLS_DC);
	z_len = zend_read_property(hessian_buffered_stream_entry, this, ZEND_STRL("len"), 0 TSRMLS_DC);
	len = Z_LVAL_P(z_len);
	PHP_STREAM_TO_ZVAL(stream, &fp);

	if (php_stream_eof(stream) && (new_pos > len)) {
		php_error_docref(NULL, E_WARNING, "read past end of stream: %d", new_pos);
		return;
	}
	property = zend_read_property(hessian_buffered_stream_entry, this, ZEND_STRL("bufferSize"), 0 TSRMLS_DC);
	buffer_size = Z_LVAL_P(property);
	property = zend_read_property(hessian_buffered_stream_entry, this, ZEND_STRL("bytes"), 0 TSRMLS_DC);
	bytes = (char*)property;
	property = zend_read_property(hessian_buffered_stream_entry, this, ZEND_STRL("bufferAllocSize"), 0 TSRMLS_DC);
	buffer_alloc_size = (long)property;
	property = zend_read_property(hessian_buffered_stream_entry, this, ZEND_STRL("bufferPos"), 0 TSRMLS_DC);
	buffer_pos = Z_STRVAL_P(property);

	hessian_ensure_buffer_size(this, new_pos);

	buf = bytes + buffer_pos;
	read_length = php_stream_read(stream, buf, buffer_size);
	len += read_length;

	zend_update_property(hessian_buffered_stream_entry, this, ZEND_STRL("len"), len TSRMLS_DC);
}


/*
	HessianBufferedStream::__construct
*/
static PHP_METHOD(HessianBufferedStream, __construct)
{
	zval *fp, *buffer_size;
	zval *self;
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &fp, &buffer_size)) {
		return;
	}
	if(Z_TYPE_P(fp) != IS_RESOURCE){
		php_error_docref(NULL, E_WARNING, "fp is not a resource");
		return;
	}
	if(Z_TYPE_P(buffer_size) != IS_LONG){
		php_error_docref(NULL, E_WARNING, "buffer_size is not a long number");
		return;
	}

	self = getThis();
	zend_update_property(hessian_buffered_stream_entry, self, ZEND_STRL("fp"), fp TSRMLS_DC);
	zend_update_property(hessian_buffered_stream_entry, self, ZEND_STRL("bufferSize"), buffer_size TSRMLS_DC);
}

/*
	HessianBufferedStream::setStream
*/
static PHP_METHOD(HessianBufferedStream, setStream)
{
	zval *fp;
	zval *self;
	zval *len;
	long new_pos;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &fp)) {
		return;
	}
	if(Z_TYPE_P(fp) != IS_RESOURCE){
		php_error_docref(NULL, E_WARNING, "fp is not a resource");
		return;
	}

	self = getThis();
	zend_update_property(hessian_buffered_stream_entry, self, ZEND_STRL("fp"), fp TSRMLS_DC);
	len = zend_read_property(hessian_buffered_stream_entry, self, ZEND_STRL("len"), 0 TSRMLS_DC);
	Z_LVAL_P(len) = 0;
	zend_update_property(hessian_buffered_stream_entry, self, ZEND_STRL("len"), len TSRMLS_DC);
}


/*
	public function peek($count = 1, $pos = null){
*/
static PHP_METHOD(HessianBufferedStream, peek)
{
	zval *count, *pos;
	zval *self;
	long new_pos;
	zend_bool check_res;
	char *str;
	zval *property;
	char *bytes;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &count, &pos)) {
		return;
	}
	if (Z_TYPE_P(count) != IS_LONG){
		Z_TYPE_P(count) = IS_LONG;
		Z_LVAL_P(count) = 1;
	}
	self = getThis();
	if (Z_TYPE_P(pos) == IS_NULL){
		pos = zend_read_property(hessian_buffered_stream_entry, self, ZEND_STRL("pos"), 0 TSRMLS_DC);
	}
	new_pos = Z_LVAL_P(pos) + Z_LVAL_P(count);


	check_res = hessian_buffered_stream_check_read(self, new_pos);
	if (!check_res){
		php_error_docref(NULL, E_WARNING, "read past end of stream:");
	}
	property = zend_read_property(hessian_buffered_stream_entry, this, ZEND_STRL("bytes"), 0 TSRMLS_DC);
	bytes = Z_STRVAL_P(property);
	str = emalloc(Z_LVAL_P(count) + 1);
	if (!str){
		php_error_docref(NULL, E_ERROR, "BuffererInputStream::peek alloc memory error");
	}
	str = memcpy(str, bytes, Z_LVAL_P(count));
	RETURN_STRING(str, 0);
}

/*
	public function peek($count = 1, $pos = null){
*/
static PHP_METHOD(HessianBufferedStream, read)
{
	zval *count, *pos;
	zval *self;
	long new_pos, copy_count;
	zend_bool check_res;
	char *str, *buf;
	zval *property;
	char *bytes;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &count, &pos)) {
		return;
	}
	if (Z_TYPE_P(count) != IS_LONG){
		return;
	}
	if (Z_LVAL_P(count) < 1){
		return;
	}
	self = getThis();
	pos = zend_read_property(hessian_buffered_stream_entry, self, ZEND_STRL("pos"), 0 TSRMLS_DC);
	new_pos = Z_LVAL_P(pos) + Z_LVAL_P(count); 
	hessian_buffered_stream_check_read(self, new_pos);
	property = zend_read_property(hessian_buffered_stream_entry, self, ZEND_STRL("bytes"), 0 TSRMLS_DC);
	buf = (char*)property;
	str = emalloc(str, count+1);
	if (!str){
		php_error_docref(NULL, E_ERROR, "BuffererInputStream::read alloc memory error");
	}
	//chek read length
	copy_count = memcpy(str, buf+pos, count);
	str[count] = 0;
	Z_LVAL_P(pos) = Z_LVAL_P(pos) + copy_count;
	RETURN_STRING(str, 0);
}


/*
	HessianBufferedStream::EOF
*/
static PHP_METHOD(HessianBufferedStream, EOF)
{
	zval *self, *fp;
	php_stream *stream;

	self = getThis();
	fp = zend_read_property(hessian_buffered_stream_entry, self, ZEND_STRL("fp"), 0 TSRMLS_DC);
	PHP_STREAM_TO_ZVAL(stream, &fp);

	if (php_stream_eof(stream)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}


/*
	HessianBufferedStream::write
*/
static PHP_METHOD(HessianBufferedStream, write)
{
	zval *self, *property, *fp;
	char *bytes, *buf;
	char *data;
	int	data_len;
	long buffer_alloc_size, buffer_pos, new_size;
	long len;
	php_stream *stream;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &data, &data_len)) {
		return;
	}

	
	self = getThis();
	property = zend_read_property(hessian_buffered_stream_entry, self, ZEND_STRL("bytes"), 0 TSRMLS_DC);
	fp = zend_read_property(hessian_buffered_stream_entry, self, ZEND_STRL("fp"), 0 TSRMLS_DC);
	PHP_STREAM_TO_ZVAL(stream, &fp);
	bytes = (char*)property;

	property = zend_read_property(hessian_buffered_stream_entry, this, ZEND_STRL("bufferAllocSize"), 0 TSRMLS_DC);
	buffer_alloc_size = (long)property;
	property = zend_read_property(hessian_buffered_stream_entry, this, ZEND_STRL("bufferPos"), 0 TSRMLS_DC);
	buffer_pos = Z_STRVAL_P(property);
	new_size = buffer_pos + data_len;
	hessian_ensure_buffer_size(self, new_size);
	/*
		$this->bytes = array_merge($this->bytes, $bytes);
		$len = fwrite($this->fp, $data);
		$this->len += $len;
	*/

	buf = bytes + buffer_pos;
	memcpy(buf, data, data_len);

	php_stream_write(stream, data, data_len);
	property = zend_read_property(hessian_buffered_stream_entry, this, ZEND_STRL("len"), 0 TSRMLS_DC);
	len = Z_LVAL_P(property) + data_len;
	Z_LVAL_P(property) = len;
	zend_update_property(hessian_buffered_stream_entry, this, ZEND_STRL("len"), property TSRMLS_DC);
}



/*
	HessianBufferedStream::readAll
*/
static PHP_METHOD(HessianBufferedStream, readAll)
{
	zval *self, *property, *fp;
	php_stream	*stream;
	zval		*zsrc;
	long		maxlen		= PHP_STREAM_COPY_ALL
	long		len;
	char		*contents	= NULL;
	char *bytes;
	
	self = getThis();
	/*
	$data = stream_get_contents($this->fp);
	$this->bytes = str_split($data);
	$this->len = count($this->bytes);
	return $data;		
	*/

	


	len = php_stream_copy_to_mem(stream, &contents, maxlen, 0);

	/*
	if (contents) {
		if (len > INT_MAX) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "content truncated from %ld to %d bytes", len, INT_MAX);
		}
	} 
	*/

	property = zend_read_property(hessian_buffered_stream_entry, self, ZEND_STRL("bytes"), 0 TSRMLS_DC);
	if(property){
		bytes = (char*)property;
		if (bytes){
			efree(bytes);
		}
	}

	zend_update_property(hessian_buffered_stream_entry, this, ZEND_STRL("bytes"), (zval*)contents TSRMLS_DC);
	property = zend_read_property(hessian_buffered_stream_entry, self, ZEND_STRL("len"), 0 TSRMLS_DC);
	Z_LVAL_P(property) = len;
	zend_update_property(hessian_buffered_stream_entry, this, ZEND_STRL("len"), property TSRMLS_DC);
	RETURN_STRING(contents, 1);
}


/*
	HessianBufferedStream::flush
*/
static PHP_METHOD(HessianBufferedStream, flush)
{
	zval *self, *fp;
	php_stream *stream;

	self = getThis();
	fp = zend_read_property(hessian_buffered_stream_entry, self, ZEND_STRL("fp"), 0 TSRMLS_DC);
	PHP_STREAM_TO_ZVAL(stream, &fp);
	php_stream_passthru(stream);
	php_stream_flush(stream);
}


/*
	HessianBufferedStream::getData
*/
static PHP_METHOD(HessianBufferedStream, getData)
{
	zval *self, *property;
	char *bytes;
	
	self = getThis();
	property = zend_read_property(hessian_buffered_stream_entry, self, ZEND_STRL("bytes"), 0 TSRMLS_DC);
	bytes = (char*)property;
	RETURN_STRING(bytes, 1);
}


/*
	HessianBufferedStream::close
*/
static PHP_METHOD(HessianBufferedStream, close)
{
	zval *self, *fp;
	php_stream *stream;

	self = getThis();
	fp = zend_read_property(hessian_buffered_stream_entry, self, ZEND_STRL("fp"), 0 TSRMLS_DC);
	PHP_STREAM_TO_ZVAL(stream, &fp);
	php_stream_close(stream);
}




//HessianBufferedStream functions
const zend_function_entry hessian_buffered_stream_functions[] = {
	PHP_ME(HessianBufferedStream, __construct,		arginfo_hessian_buffered_stream_construct,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianBufferedStream, setStream,		arginfo_hessian_buffered_stream_setstream,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianBufferedStream, peek,		arginfo_hessian_buffered_stream_peek			,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianBufferedStream, read,		arginfo_hessian_buffered_stream_read			,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianBufferedStream, EOF,		arginfo_hessian_buffered_stream_eof			,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianBufferedStream, write,		arginfo_hessian_buffered_stream_write			,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianBufferedStream, readAll,		arginfo_hessian_buffered_stream_readall			,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianBufferedStream, flush,		arginfo_hessian_buffered_stream_flush			,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianBufferedStream, getData,		arginfo_hessian_buffered_stream_get_data			,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianBufferedStream, close,		arginfo_hessian_buffered_stream_get_close			,		ZEND_ACC_PUBLIC)
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
