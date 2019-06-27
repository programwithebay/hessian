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
ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_reference_inc_reference, 0, 0, 1)
	ZEND_ARG_INFO(0, obj) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_reference_get_class_index, 0, 0, 1)
	ZEND_ARG_INFO(0, class) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_reference_add_class_def, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, "classdef", "HessianClassDef", 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_reference_get_reference, 0, 0, 1)
	ZEND_ARG_INFO(0, object) /* string */
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_reference_get_type_index, 0, 0, 1)
	ZEND_ARG_INFO(0, type) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_reference_reset, 0, 0, 0)
ZEND_END_ARG_INFO()




//entry
zend_class_entry *hessian_reference_map_entry;



/*
	HessianReferenceMap::__construct
*/
static PHP_METHOD(HessianReferenceMap, incReference)
{
	zval *obj, *self;
	zval *hessian_ref, *object_list, object_count, *ref_list;
	zval function_name;
	zval *params[2];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &obj) == FAILURE) {
		RETURN_FALSE;
	}

	self = getThis();
	/*
	$this->reflist[] =  new HessianRef(count($this->objectlist));
		if($obj != null)
			$this->objectlist[] = $obj;
	*/

	ALLOC_ZVAL(hessian_ref);
	object_init_ex(hessian_ref, hessian_ref_entry);
	object_list = zend_read_property(hessian_reference_map_entry, self, ZEND_STRL("objectlist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(object_list) == IS_ARRAY){
		ZVAL_LONG(&object_count, zend_hash_num_elements(Z_ARRVAL_P(object_list)));
	}else{
		ZVAL_LONG(&object_count, 0);
	}
	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "__construct", 1);
	params[0] = &object_count;
	call_user_function(NULL, &hessian_ref, &function_name, NULL, 1, params TSRMLS_CC);

	
	ref_list = zend_read_property(hessian_reference_map_entry, self, ZEND_STRL("reflist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(ref_list) != IS_ARRAY){
		ALLOC_ZVAL(ref_list);
		array_init_size(ref_list, 2);
	}
	zend_hash_next_index_insert(Z_ARRVAL_P(ref_list), hessian_ref, sizeof(zval*), NULL);

	if (Z_TYPE_P(obj) != IS_NULL){
		if (Z_TYPE_P(object_list) != IS_ARRAY){
			ALLOC_ZVAL(object_list);
			array_init_size(object_list, 1);
		}
		zend_hash_next_index_insert(Z_ARRVAL_P(object_list), obj, sizeof(zval*), NULL);
		zend_update_property(hessian_ref_entry, self, ZEND_STRL("objectlist"), object_list TSRMLS_DC);
	}
}


/*
	HessianReferenceMap::getClassIndex
*/
static PHP_METHOD(HessianReferenceMap, getClassIndex)
{
	zval *self;
	char *class_name;
	int class_len;
	zval *class_list;
	HashPosition pos;
	zval **src_entry;
	char *var_name;
	uint var_len;
	ulong var_index;
	int key_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &class_name, &class_len) == FAILURE) {
		RETURN_FALSE;
	}

	self = getThis();
	/*
		foreach($this->classlist as $index => $classdef){
			if($classdef->type == $class)
				return $index;
		}
		return false;
	*/

	class_list = zend_read_property(hessian_reference_map_entry, self, ZEND_STRL("classlist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(class_list) !=  IS_ARRAY){
		RETURN_FALSE;
	}
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(class_list), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(class_list), (void **)&src_entry, &pos) == SUCCESS) {
		zval *type;
		key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(class_list), &var_name, &var_len, &var_index, 0, &pos);
		if (Z_TYPE_P(*src_entry) != IS_OBJECT){
			zend_hash_move_forward_ex(Z_ARRVAL_P(class_list), &pos);
			continue;
		}
		type = zend_read_property(NULL, *src_entry, ZEND_STRL("type"), 1 TSRMLS_DC);
		if (type && Z_TYPE_P(type) == IS_STRING){
			if (strcmp(class_name, Z_STRVAL_P(type)) == 0){
				int index;
				index = atoi(var_name);
				RETURN_LONG(index);
			}
		}
		zend_hash_move_forward_ex(Z_ARRVAL_P(class_list), &pos);
	}

	RETURN_FALSE;
}


/*
	HessianReferenceMap::getClassIndex
*/
static PHP_METHOD(HessianReferenceMap, addClassDef)
{
	zval *self, *class_def, *prop_class_list;


	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &class_def) == FAILURE) {
		RETURN_FALSE;
	}

	self = getThis();
	/*
		$this->classlist[] = $classdef;
		return count($this->classlist) - 1;
	*/

	prop_class_list = zend_read_property(hessian_reference_map_entry, self, ZEND_STRL("classlist"), 1 TSRMLS_DC);
	if (Z_TYPE_P(prop_class_list) != IS_ARRAY){
		ALLOC_ZVAL(prop_class_list);
		array_init_size(prop_class_list, 1);
	}
	zend_hash_next_index_insert(Z_ARRVAL_P(prop_class_list), class_def, sizeof(zval*), NULL);

	RETURN_LONG(zend_hash_num_elements(Z_ARRVAL_P(prop_class_list)));
}


/*
	HessianReferenceMap::getReference
*/
static PHP_METHOD(HessianReferenceMap, getReference)
{
	zval *self, *object, *object_list;
	HashPosition pos;
	zval **entry;
	zval result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &object) == FAILURE) {
		RETURN_FALSE;
	}

	self = getThis();
	/*
	return array_search($object, $this->objectlist, true);
	*/
	object_list = zend_read_property(hessian_reference_map_entry, self, ZEND_STRL("objectlist"), 1 TSRMLS_DC);
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(object_list), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(object_list), (void **)&entry, &pos) == SUCCESS) {
		if (SUCCESS == compare_function(&result, *entry, object TSRMLS_CC)){
			if (0 == Z_LVAL(result)){
				RETURN_TRUE;
			}else{
				RETURN_FALSE;
			}
		}
		zend_hash_move_forward_ex(Z_ARRVAL_P(object_list), &pos);
	}
	RETURN_FALSE;
}


/*
	HessianReferenceMap::getTypeIndex
*/
static PHP_METHOD(HessianReferenceMap, getTypeIndex)
{
	zval *self, *type, *type_list;
	HashPosition pos;
	zval *entry;
	zval *result;


	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &type) == FAILURE) {
		RETURN_FALSE;
	}

	self = getThis();
	/*
		return array_search($type, $this->typelist, true);
	*/
	type_list = zend_read_property(hessian_reference_map_entry, self, ZEND_STRL("typelist"), 1 TSRMLS_DC);
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(type_list), &pos);
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(type_list), (void **)&entry, &pos) == SUCCESS) {
		if (SUCCESS == compare_function(result, entry, type TSRMLS_CC)){
			RETURN_TRUE;
		}
		zend_hash_move_forward_ex(Z_ARRVAL_P(type_list), &pos);
	}
	RETURN_FALSE;
}


/*
	HessianReferenceMap::reset
*/
static PHP_METHOD(HessianReferenceMap, reset)
{
	zval *self;
	zval *object_list, *ref_list, *type_list, *class_list;


	self = getThis();
	/*
		$this->objectlist = 
			$this->reflist = 
			$this->typelist = 
			$this->classlist = array();
	*/
	object_list = zend_read_property(hessian_reference_map_entry, self, ZEND_STRL("objectlist"), 1 TSRMLS_DC);
	if (object_list){
		FREE_ZVAL(object_list);
	}
	ALLOC_ZVAL(object_list);
	array_init_size(object_list, 0);

	
	ref_list = zend_read_property(hessian_reference_map_entry, self, ZEND_STRL("reflist"), 1 TSRMLS_DC);
	if (ref_list){
		FREE_ZVAL(ref_list);
	}
	ALLOC_ZVAL(ref_list);
	array_init_size(ref_list, 0);
	
	type_list = zend_read_property(hessian_reference_map_entry, self, ZEND_STRL("typelist"), 1 TSRMLS_DC);
	if (type_list){
		FREE_ZVAL(type_list);
	}
	ALLOC_ZVAL(type_list);
	array_init_size(type_list, 0);
	
	class_list = zend_read_property(hessian_reference_map_entry, self, ZEND_STRL("classlist"), 1 TSRMLS_DC);
	if (class_list){
		FREE_ZVAL(class_list);
	}
	ALLOC_ZVAL(class_list);
	array_init_size(class_list, 0);
}




//HessianReferenceMap functions
const zend_function_entry hessian_reference_map_functions[] = {
	PHP_ME(HessianReferenceMap, incReference, 	arginfo_hessian_reference_inc_reference,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianReferenceMap, getClassIndex, 	arginfo_hessian_reference_get_class_index,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianReferenceMap, addClassDef, 	arginfo_hessian_reference_add_class_def,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianReferenceMap, getReference, 	arginfo_hessian_reference_get_reference,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianReferenceMap, getTypeIndex, 	arginfo_hessian_reference_get_type_index,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianReferenceMap, reset, 	arginfo_hessian_reference_reset,		ZEND_ACC_PUBLIC)
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
