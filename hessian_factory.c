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



ZEND_BEGIN_ARG_INFO_EX(arginfo_ihessian_object_factory_get_object, 0, 0, 1)
	ZEND_ARG_INFO(0, type) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ihessian_object_factory_set_options, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, options, "HessianOptions", 0) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_object_factory_set_options, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, options, "HessianOptions", 0) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_object_factory_get_object, 0, 0, 1)
	ZEND_ARG_INFO(0, type) /* string */
ZEND_END_ARG_INFO()


zend_class_entry *ihessian_object_factory_entry;
zend_class_entry *hessian_object_factory_entry;



/*
	HessianObjectFactory::setOptions
*/
static PHP_METHOD(HessianObjectFactory, setOptions)
{
	zval *options;
	zval *self;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z",&options)) {
		return;
	}
	self = getThis();

	zend_update_property(hessian_client_entry, self, ZEND_STRL("options"), options TSRMLS_DC);
}

/*
	HessianObjectFactory::getObject
*/
static PHP_METHOD(HessianObjectFactory, getObject)
{
	zval *type;
	zval *self;
	zend_class_entry ce;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z",&type)) {
		return;
	}
	if (Z_TYPE_P(type) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "type must be an string");
		return;
	}
	self = getThis();

	/*
	if(!class_exists($type)) {
			if(isset($this->options->strictType) && $this->options->strictType)
				throw new Exception("Type $type cannot be found for object instantiation, check your type mappings");
			$obj = new stdClass();
			//$obj->__type = $type;
			return $obj;
		}
		return new $type();
	*/
	if (zend_lookup_class(Z_STRVAL_P(type), Z_STRLEN_P(type), &ce TSRMLS_CC) != SUCCESS) {
		zval  *options;
		options = zend_read_property(hessian_client_entry, self, ZEND_STRL("options"), 0 TSRMLS_DC);
		if (options){
			zval *strict_type;
			strict_type = zend_read_property(hessian_client_entry, options, ZEND_STRL("strictType"), 0 TSRMLS_DC);
			if (i_zend_is_true(strict_type){
				php_error_docref(NULL, E_ERROR, "Type %s cannot be found for object instantiation, check your type mappings", Z_STRVAL_P(type));
			}
		}
		zend_lookup_class(ZEND_STRL("stdClass"), &ce TSRMLS_CC);
		object_init_ex(return_value, &ce);
		Z_TYPE_P(return_value) = IS_OBJECT;
		return;
	}
	object_init_ex(return_value, &ce);
	Z_TYPE_P(return_value) = IS_OBJECT;
}



//IDubboStorage interface
const zend_function_entry hessian_object_factory_functions[] = {
	PHP_ABSTRACT_ME(IHessianObjectFactory, getObject, arginfo_ihessian_object_factory_get_object)
	PHP_ABSTRACT_ME(IHessianObjectFactory, setOptions, arginfo_ihessian_object_factory_set_options)
	PHP_FE_END
};



//HessianClient functions
const zend_function_entry hessian_object_factory_functions[] = {
	PHP_ME(HessianObjectFactory, setOptions, 	arginfo_hessian_object_factory_set_options,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianObjectFactory, getObject,		arginfo_hessian_object_factory_get_object,		ZEND_ACC_PUBLIC)
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
