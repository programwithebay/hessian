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





ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_callback_handler_construct, 0, 0, 1)
	ZEND_ARG_INFO(0, callback) /* array */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_callback_handler_get_callback, 0, 0, 1)
	ZEND_ARG_INFO(0, obj) /* array */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_callback_handler_do_callback, 0, 0, 2)
	ZEND_ARG_INFO(0, callable) /* array */
	ZEND_ARG_INFO(0, arguments) /* array */
ZEND_END_ARG_INFO()




zend_class_entry *hessian_callback_handler_entry;



/*
	HessianCallbackHandler::__construct
*/
static PHP_METHOD(HessianCallbackHandler, __construct)
{
	zval *callback, *not_found, *cache;
	zval *self;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &callback)) {
		return;
	}
	self = getThis();
	if (!callback){
		MAKE_STD_ZVAL(callback);
		array_init_size(callback, 8);
	}
	zend_update_property(hessian_callback_handler_entry, self, ZEND_STRL("callbacks"), callback TSRMLS_DC);
	//init notFound and cache

	MAKE_STD_ZVAL(not_found);
	array_init_size(not_found, 8);


	MAKE_STD_ZVAL(cache);
	array_init_size(cache, 8);

	zend_update_property(hessian_callback_handler_entry, self, ZEND_STRL("notFound"), not_found TSRMLS_DC);
	zend_update_property(hessian_callback_handler_entry, self, ZEND_STRL("cache"), cache TSRMLS_DC);
}


/*
	HessianCallbackHandler::getCallback
*/
static PHP_METHOD(HessianCallbackHandler, getCallback)
{
	zval *obj, *callbacks, *not_found, *cache;
	zval function_name;
	zval *ref, *types, *retval;
	zval *self;
	zval **value_ptr;
	int dup;
	char *class_name;
	char check[512];
	int class_name_len;
	zval *params[1];
	HashPosition	pos;
	zval **src_entry;
	zval *z_false;
	zend_class_entry* ce_reflection;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &obj)) {
		return;
	}
	self = getThis();
	callbacks = zend_read_property(hessian_callback_handler_entry, self, ZEND_STRL("callbacks"), 0 TSRMLS_DC);
	if (Z_TYPE_P(callbacks) != IS_ARRAY){
		RETURN_FALSE;
	}

	if (zend_hash_num_elements(Z_ARRVAL_P(callbacks)) < 1){
		RETURN_FALSE;
	}

	if (Z_TYPE_P(obj) == IS_STRING){
		if (SUCCESS == zend_hash_find(HASH_OF(callbacks), Z_STRVAL_P(obj), Z_STRLEN_P(obj)-1, (void **)&value_ptr)){
			return_value =  *value_ptr;
			return;
		}else{
			RETURN_FALSE;
		}
	}

	if (Z_TYPE_P(obj) != IS_OBJECT){
		RETURN_FALSE;
	}
	dup = zend_get_object_classname(obj, &class_name, &class_name_len TSRMLS_DC);
	not_found = zend_read_property(hessian_callback_handler_entry, self, ZEND_STRL("notFound"), 0 TSRMLS_DC);
	if (SUCCESS == zend_hash_find(HASH_OF(not_found), class_name, class_name_len, (void **)&value_ptr)){
		RETURN_FALSE;
	}
	cache = zend_read_property(hessian_callback_handler_entry, self, ZEND_STRL("cache"), 0 TSRMLS_DC);
	if (SUCCESS == zend_hash_find(HASH_OF(cache), class_name, class_name_len, (void **)&value_ptr)){
		return_value =  *value_ptr;
		return;
	}

	//zend_fetch_class
	ce_reflection = zend_fetch_class("ReflectionClass", strlen("ReflectionClass")-1, 0);
	MAKE_STD_ZVAL(ref);
	Z_TYPE_P(ref) = IS_OBJECT;
	object_init_ex(ref, ce_reflection);
	Z_SET_REFCOUNT_P(ref, 1);

	//init types
	MAKE_STD_ZVAL(types);
	array_init_size(types, 8);

	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "getName", 1);
	//dont check is error
	call_user_function(NULL, &ref, &function_name, retval, 0, params TSRMLS_CC);
	//$types[] = $ref->getName();
	zend_hash_next_index_insert(HASH_OF(types),retval, sizeof(zval*), NULL);

	/*
	if($ref->getParentClass())
			$types[] = $ref->getParentClass()->getName();
		
	*/

	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "getParentClass", 1);
	call_user_function(NULL, &ref, &function_name, retval, 0, params TSRMLS_CC);
	if (Z_TYPE_P(retval) == IS_OBJECT){
		zval *tmp;
		INIT_ZVAL(function_name);
		ZVAL_STRING(&function_name, "getName", 1);
		//dont check is error
		tmp = retval;
		call_user_function(NULL, &tmp, &function_name, retval, 0, params TSRMLS_CC);
		zend_hash_next_index_insert(HASH_OF(types),retval, sizeof(zval*), NULL);
	}

	/*
	$ints = $ref->getInterfaceNames();
	if($ints)
		$types = array_merge($types, $ints);
	*/

	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "getInterfaceNames", 1);
	//dont check is error
	call_user_function(NULL, &ref, &function_name, retval, 0, params TSRMLS_CC);
	if (Z_TYPE_P(retval) == IS_ARRAY){
		php_array_merge(Z_ARRVAL_P(types), Z_ARRVAL_P(retval), 0 TSRMLS_CC);
	}

	/*
	foreach($types as $type){
			$check = '@'.$type;
			if(isset($this->callbacks[$check])){
				$callback = $this->callbacks[$check];
				$this->cache[$class] = $callback;
				return $callback;
			}
		}
		$this->notFound[$class] = true;
		return false;
	*/

	zend_hash_internal_pointer_reset_ex(HASH_OF(types), &pos);
	check[0] = '@';
	while (zend_hash_get_current_data_ex(HASH_OF(types), (void **)&src_entry, &pos) == SUCCESS) {
		memcpy(check+1, Z_STRVAL_PP(src_entry), Z_STRLEN_PP(src_entry));
		if (SUCCESS == zend_hash_find(HASH_OF(callbacks), check,  Z_STRLEN_PP(src_entry)+1, (void **)&value_ptr)){
			zend_hash_add(HASH_OF(cache), class_name, class_name_len, *value_ptr,  sizeof(zval*), NULL);
			return_value = *value_ptr;
		}
		zend_hash_move_forward_ex(HASH_OF(types), &pos);
	}

	ALLOC_ZVAL(z_false);
	//INIT_ZVAL(z_false);
	Z_TYPE_P(z_false) = IS_BOOL;
	Z_LVAL_P(z_false) = 0;
	zend_hash_add(HASH_OF(not_found), class_name, class_name_len, z_false,  sizeof(zval*), NULL);
	RETURN_FALSE;
}


/*
	HessianCallbackHandler::doCallback
*/
static PHP_METHOD(HessianCallbackHandler, doCallback)
{
	zval *self;
	zval *params, *retval_ptr = NULL;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "fa/", &fci, &fci_cache, &params) == FAILURE) {
		return;
	}

	zend_fcall_info_args(&fci, params TSRMLS_CC);
	fci.retval_ptr_ptr = &retval_ptr;

	if (zend_call_function(&fci, &fci_cache TSRMLS_CC) == SUCCESS && fci.retval_ptr_ptr && *fci.retval_ptr_ptr) {
		COPY_PZVAL_TO_ZVAL(*return_value, *fci.retval_ptr_ptr);
	}

	zend_fcall_info_args_clear(&fci, 1);
}


//HessianCallbackHandler functions
const zend_function_entry hessian_callback_handler_functions[] = {
	PHP_ME(HessianCallbackHandler, __construct,		arginfo_hessian_callback_handler_construct,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianCallbackHandler, getCallback,		arginfo_hessian_callback_handler_get_callback,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianCallbackHandler, doCallback,		arginfo_hessian_callback_handler_do_callback,		ZEND_ACC_PUBLIC)
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
