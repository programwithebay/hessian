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


ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_options_from_array, 0, 0, 1)
	ZEND_ARG_INFO(0, array) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_options_resolve_options, 0, 0, 1)
	ZEND_ARG_INFO(0, object) /* string */
ZEND_END_ARG_INFO()


zend_class_entry *hessian_options_entry;

//HessianOptions property
static const char* hessian_options_property[18] = {
	"version",
	"transport",
	"transportOptions",
	"detectVersion",
	"objectFactory",
	"typeMap",
	"strictTypes",
	"headers",
	"interceptors",
	"timeZone",
	"saveRaw",
	"serviceName",
	"displayInfo",
	"ignoreOutput",
	"parseFilters",
	"writeFilters",
	"before",
	"after"
};

//from array
void hessian_options_from_array(zval *self, zval * array){
	int i;
	zval *array_value;
	
	for(i=0; i<18;i++){
		if (SUCCESS == zend_hash_find(Z_ARRVAL_P(array), hessian_options_property[i]
			, strlen(hessian_options_property[i]) - 1, (void **)&array_value)){
			zend_update_property(hessian_options_entry, self, hessian_options_property[i]
				, strlen(hessian_options_property[i])-1, array_value TSRMLS_DC);
		}
	}
}

/**
	HessianOptions::fromArray
**/

static PHP_METHOD(HessianOptions, fromArray)
{
	zval *array, *vars;
	zval *self;
	zval **value_ptr;
	int i;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &array) == FAILURE) {
		return;
	}
	if (Z_TYPE_P(array) != IS_ARRAY){
		php_error_docref(NULL, E_WARNING, "array must be an array");
		return;
	}
	self = getThis();
	/*
	$vars = get_class_vars('HessianOptions');
	foreach($arr as $key=>$value){
		if(array_key_exists($key, $vars)){
			$this->$key = $value;
		}
	}
	*/

	hessian_options_from_array(self, array);
	
}



/**
	HessianOptions::fromArray
**/

static PHP_METHOD(HessianOptions, resolveOptions)
{
	zval *object, *options, *self;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &object) == FAILURE) {
		return;
	}

	ALLOC_ZVAL(options);
	object_init_ex(options, hessian_options_entry);
	if (Z_TYPE_P(object) == IS_NULL){
		RETURN_ZVAL(options, 0, NULL);
	}
	/*
	$options = new HessianOptions();
	if($object == null)
		return $options;
	if($object instanceof HessianOptions)
		return $object;
	elseif(is_array($object))
		$options->fromArray($object);
	elseif(is_object($object)){
		$arr = (array)$object;
		$options->fromArray($arr);
	}
	return $options;
	*/

	if (instanceof_function(Z_OBJCE_P(object), hessian_options_entry)){
		RETURN_ZVAL(object, 0, NULL);
	}else if (Z_TYPE_P(object) == IS_ARRAY){
		hessian_options_from_array(options, object);
	}else if (Z_TYPE_P(object) == IS_OBJECT){
		convert_to_array(object);
		hessian_options_from_array(options, object);
	}

	RETURN_ZVAL(options, 0, NULL);
}


//HessianOptions  functions
const zend_function_entry hessian_options_functions[] = {
	PHP_ME(HessianOptions, fromArray,		arginfo_hessian_options_from_array,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianOptions, resolveOptions,		arginfo_hessian_options_resolve_options,		ZEND_ACC_PUBLIC)
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
