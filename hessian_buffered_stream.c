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


zend_object_handlers hessian_buffered_stream_object_handlers;

struct _hessian_buffered_stream_entity{
	char *bytes;
	long buffer_size;
	long buffer_pos;
	long buffer_alloc_size;
	long length;	//how much bytes read/write
};

typedef struct _hessian_buffered_stream_entity hessian_buffered_stream_entity;

typedef struct _hessian_buffered_stream_object hessian_buffered_stream_object;
struct _hessian_buffered_stream_object {
    zend_object std;
	hessian_buffered_stream_entity entity;
};




void hessian_buffered_stream_free_storage(void *object TSRMLS_DC)
{
    hessian_buffered_stream_object *obj = (hessian_buffered_stream_object *)object;

   	zend_object_std_dtor(&obj->std TSRMLS_CC);
	if (obj->entity.bytes){
		efree(obj->entity.bytes);
	}
    efree(obj);
}

zend_object_value hessian_buffered_stream_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    hessian_buffered_stream_object *obj = (hessian_buffered_stream_object *)emalloc(sizeof(hessian_buffered_stream_object));
    memset(obj, 0, sizeof(hessian_buffered_stream_object));
   

	zend_object_std_init(&obj->std, type TSRMLS_CC);
	object_properties_init(&obj->std, type);

	obj->std.ce = type;
    retval.handle = zend_objects_store_put(obj, NULL, hessian_buffered_stream_free_storage, NULL TSRMLS_CC);
    retval.handlers = &hessian_buffered_stream_object_handlers;

    return retval;
}


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

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_buffered_stream_eof, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_buffered_stream_readall, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_buffered_stream_read, 0, 0, 1)
	ZEND_ARG_INFO(0, count) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_buffered_stream_write, 0, 0, 1)
	ZEND_ARG_INFO(0, data) /* string */
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
void hessian_ensure_buffer_size(zval **self, long size){
	zval *property, *this, *z_bytes;
	php_stream *stream;
	long len;
	long buffer_size, read_length;
	char *bytes;
	char *buf;
	long buffer_pos, buffer_alloc_size, new_buffer_alloc_size;
	long multi;
	hessian_buffered_stream_object *obj;
	

	this = *self;
	obj = (hessian_buffered_stream_object *)zend_object_store_get_object(this TSRMLS_CC);
	//buffer
	bytes = obj->entity.bytes;
	//buffer pos
	buffer_size = obj->entity.buffer_size;
	buffer_pos = obj->entity.buffer_pos;
	buffer_alloc_size = obj->entity.buffer_alloc_size;
	
	if (size < buffer_alloc_size){
		return;
	}
	//todo:need optimize
	multi =  size / buffer_size;
	if (0 == (size % buffer_size)){
		multi += 1;
	}else{
		multi += 1;
	}
	new_buffer_alloc_size = multi * buffer_size;
	if (!bytes){
		bytes = emalloc(new_buffer_alloc_size);
	}else{
		bytes = perealloc(bytes, new_buffer_alloc_size, 0);
	}
	if (!bytes){
		php_error_docref(NULL, E_ERROR, "BufferInputStream alloc memory error");
	}
	obj->entity.bytes =  bytes;
	obj->entity.buffer_alloc_size = new_buffer_alloc_size;
}


/*
	check read
*/
char* hessian_buffered_stream_read(zval **self, long new_pos){
	zval *fp, *this;
	zval *property;
	php_stream *stream;
	long len;
	long buffer_size, read_length;
	char *bytes;
	char *tmp_buf, *buf;
	long read_count;
	hessian_buffered_stream_object *obj;

	this = *self;
	obj = (hessian_buffered_stream_object *)zend_object_store_get_object(this TSRMLS_CC);
	fp = zend_read_property(hessian_buffered_stream_entry, this, ZEND_STRL("fp"), 0 TSRMLS_DC);
	php_stream_from_zval_no_verify(stream, &fp); 
	len = obj->entity.length;

	if (php_stream_eof(stream) && (new_pos > len)) {
		php_error_docref(NULL, E_WARNING, "read past end of stream: %d", new_pos);
		return;
	}

	hessian_ensure_buffer_size(self, new_pos);
	bytes = obj->entity.bytes;
	buffer_size = obj->entity.buffer_size;
	buf = bytes + obj->entity.length;
	tmp_buf = buf;
	//read  from fp
	if (new_pos > len){
		while(new_pos > len){
			read_length = php_stream_read(stream, tmp_buf, buffer_size);
			tmp_buf += read_length;
			len += read_length;
		}
	}
	
	obj->entity.length = len;
	return buf;
}


/*
	HessianBufferedStream::__construct
*/
static PHP_METHOD(HessianBufferedStream, __construct)
{
	zval *fp, *buffer_size;
	zval *self;
	hessian_buffered_stream_object *obj;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &fp, &buffer_size)) {
		return;
	}
	if(Z_TYPE_P(fp) != IS_RESOURCE){
		php_error_docref(NULL, E_WARNING, "fp is not a resource");
		return;
	}
	self = getThis();
	obj = (hessian_buffered_stream_object *)zend_object_store_get_object(self TSRMLS_CC);
	
	if (buffer_size){
		if(Z_TYPE_P(buffer_size) != IS_LONG){
			php_error_docref(NULL, E_WARNING, "buffer_size is not a long number");
			return;
		}
		obj->entity.buffer_size = Z_LVAL_P(buffer_size);
	}else{
		obj->entity.buffer_size = 1024;
	}

	zend_update_property(hessian_buffered_stream_entry, self, ZEND_STRL("fp"), fp TSRMLS_DC);
	obj->entity.buffer_alloc_size = 0;
	obj->entity.buffer_pos = 0;
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
	hessian_buffered_stream_object *obj;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &fp)) {
		return;
	}
	if(Z_TYPE_P(fp) != IS_RESOURCE){
		php_error_docref(NULL, E_WARNING, "fp is not a resource");
		return;
	}

	self = getThis();
	zend_update_property(hessian_buffered_stream_entry, self, ZEND_STRL("fp"), fp TSRMLS_DC);
	obj = (hessian_buffered_stream_object *)zend_object_store_get_object(self TSRMLS_CC);
	obj->entity.length = 0;
}


/*
	public function peek($count = 1, $pos = null){
*/
static PHP_METHOD(HessianBufferedStream, peek)
{
	zval *count, *z_pos;
	zval *self;
	long new_pos, new_len, pos;
	zend_bool check_res;
	char *str, *buf;
	zval *property;
	char *bytes;
	hessian_buffered_stream_object *obj;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &count, &z_pos)) {
		return;
	}
	if (Z_TYPE_P(count) != IS_LONG){
		Z_TYPE_P(count) = IS_LONG;
		Z_LVAL_P(count) = 1;
	}
	self = getThis();
	obj = (hessian_buffered_stream_object *)zend_object_store_get_object(self TSRMLS_CC);
	if (!z_pos || Z_TYPE_P(z_pos) == IS_NULL){
		pos = obj->entity.buffer_pos;
	}else{
		pos = Z_LVAL_P(z_pos);
	}
	new_pos = pos + Z_LVAL_P(count);
	//check length
	buf = hessian_buffered_stream_read(&self, new_pos);
	str = emalloc(Z_LVAL_P(count) + 1);
	if (!str){
		php_error_docref(NULL, E_ERROR, "BuffererInputStream::peek alloc memory error");
	}
	str = memcpy(str, buf, Z_LVAL_P(count));
	RETURN_STRING(str, 0);
}

/*
	public function peek($count = 1, $pos = null){
*/
static PHP_METHOD(HessianBufferedStream, read)
{
	zval *z_count;
	zval *self;
	long new_pos, new_len, count;
	zend_bool check_res;
	char *str, *buf;
	zval *property;
	char *bytes;
	hessian_buffered_stream_object *obj;
		
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &z_count)) {
		return;
	}
	if (!z_count){
		count = 1;
	}
	if (Z_TYPE_P(z_count) != IS_LONG){
		return;
	}
	if (Z_LVAL_P(z_count) < 1){
		return;
	}
	count = Z_LVAL_P(z_count);
	self = getThis();
	obj = (hessian_buffered_stream_object *)zend_object_store_get_object(self TSRMLS_CC);
	new_pos = obj->entity.buffer_pos + count; 
	str = emalloc(count+1);
	if (!str){
		php_error_docref(NULL, E_ERROR, "BuffererInputStream::read alloc memory error");
	}
	//read
	buf = hessian_buffered_stream_read(&self, new_pos);
	memcpy(str, buf, count);
	str[count] = 0;
	obj->entity.buffer_pos = new_pos;
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
	php_stream_from_zval_no_verify(stream, &fp);

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
	zval *self, *fp;
	char *bytes, *buf;
	char *data;
	int	data_len;
	long buffer_pos, new_size;
	long len;
	php_stream *stream;
	hessian_buffered_stream_object *obj;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &data, &data_len)) {
		return;
	}

	
	self = getThis();
	obj = (hessian_buffered_stream_object *)zend_object_store_get_object(self TSRMLS_CC);
	fp = zend_read_property(hessian_buffered_stream_entry, self, ZEND_STRL("fp"), 0 TSRMLS_DC);
	php_stream_from_zval_no_verify(stream, &fp);
	buffer_pos = obj->entity.buffer_pos;
	new_size = buffer_pos + data_len;
	hessian_ensure_buffer_size(&self, new_size);
	bytes = obj->entity.bytes;
	/*
		$this->bytes = array_merge($this->bytes, $bytes);
		$len = fwrite($this->fp, $data);
		$this->len += $len;
	*/
	buf = bytes + obj->entity.length;
	memcpy(buf, data, data_len);

	php_stream_write(stream, data, data_len);
	obj->entity.length += data_len;
}



/*
	HessianBufferedStream::readAll
*/
static PHP_METHOD(HessianBufferedStream, readAll)
{
	zval *self, *property, *fp;
	php_stream	*stream;
	zval		*zsrc;
	long		maxlen		= PHP_STREAM_COPY_ALL;
	long		len;
	char		*contents	= NULL;
	char *bytes;
	
	self = getThis();
	fp = zend_read_property(hessian_buffered_stream_entry, self, ZEND_STRL("fp"), 0 TSRMLS_DC);
	php_stream_from_zval_no_verify(stream, &fp);
	len = php_stream_copy_to_mem(stream, &contents, maxlen, 0);
	if (!contents){
		RETURN_FALSE;
	}

	/*
	if (contents) {
		if (len > INT_MAX) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "content truncated from %ld to %d bytes", len, INT_MAX);
		}
	} 
	*/

	hessian_buffered_stream_object *obj = (hessian_buffered_stream_object *)zend_object_store_get_object(self TSRMLS_CC);
	bytes = obj->entity.bytes;
	if (bytes){
		efree(bytes);
	}

	obj->entity.bytes = contents;
	obj->entity.length = len;
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
	php_stream_from_zval_no_verify(stream, &fp);
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
	
	hessian_buffered_stream_object *obj = (hessian_buffered_stream_object *)zend_object_store_get_object(self TSRMLS_CC);
	bytes = obj->entity.bytes;
	if (!bytes){
		RETURN_FALSE;
	}
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
	php_stream_from_zval_no_verify(stream, &fp);
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
