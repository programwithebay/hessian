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
#include "php_hessian_int.h"
#include "hessian_common.h"
#include "php_hessian.h"


//params
ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_call_construct, 0, 0, 2)
	ZEND_ARG_INFO(0, method) /* string */
	ZEND_ARG_INFO(0, arguments) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_ref_isref, 0, 0, 1)
	ZEND_ARG_INFO(0, val) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_ref_get_index, 0, 0, 1)
	ZEND_ARG_INFO(0, list) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_ref_construct, 0, 0, 1)
	ZEND_ARG_INFO(0, list) /* string */
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_stream_result_construct, 0, 0, 1)
	ZEND_ARG_INFO(0, stream) /* string */
ZEND_END_ARG_INFO()



//entry
zend_class_entry *ihessian_ignore_code_entry;
zend_class_entry *hessian_class_def_entry;
zend_class_entry *hessian_call_entry;
zend_class_entry *hessian_ref_entry;
zend_class_entry *hessian_stream_result_entry;

ZEND_DECLARE_MODULE_GLOBALS(hessian)


/*
	HessianRuleResolver::resolveSymbol
*/
hessian_parsing_rule hessian_rule_resolver_resolve_symbol(char symbol, char *type_expected)
{
	char index;
	hessian_parsing_rule rule;
	
	
	index = HESSIAN_G(hessian2_symbols)[symbol];
	rule = HESSIAN_G(hessian2_rules)[index];
	if (type_expected){
		char* split[20];
		char *p_head, *p_end;
		int i, len;

		i = 0;
		p_head = type_expected;
		while(*p_head == ' ') ++p_head;
		if (!p_head)	return rule;
		while(p_head){
			split[i++] = p_head;
			while(*p_head && *p_head != ',') p_head++;
			if (*p_head && *p_head == ',') *p_head = 0;
			++p_head;
		}

		len = i;
		for(i=0; i<len; i++){
			if (strcmp(rule.type, split[i]) == 0){
				//throw new HessianParsingException("Type $typeExpected expected");
				//@todo:error code
				zend_throw_exception(hessian_parsing_exception_entry, sprintf("Type %s expected", type_expected), 8);
			}
		}
	}

	return rule;
}


/*
	HessianCall::__construct
*/
static PHP_METHOD(HessianCall, __construct)
{
	zval *self, *method, *arguments;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &method,&arguments) == FAILURE) {
		RETURN_FALSE;
	}

	self= getThis();
	zend_update_property(hessian_call_entry, self, ZEND_STRL("method"),  method TSRMLS_DC);
	zend_update_property(hessian_call_entry, self, ZEND_STRL("arguments"), arguments TSRMLS_DC);
}



/*
	HessianRef::isRef
*/
static PHP_METHOD(HessianRef, isRef)
{
	zval *val;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &val) == FAILURE) {
		RETURN_FALSE;
	}

	RETURN_BOOL(instanceof_function(Z_OBJCE_P(val), hessian_ref_entry TSRMLS_DC));
}



/*
	HessianRef::getIndex
*/
static PHP_METHOD(HessianRef, getIndex)
{
	zval *list;
	zval function_name;
	zval *params[1];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &list) == FAILURE) {
		RETURN_FALSE;
	}

	object_init_ex(return_value, hessian_ref_entry);
	INIT_ZVAL(function_name);
	ZVAL_STRING(&function_name, "__construct", 1);
	params[0] = list;
	call_user_function(NULL, &return_value, &function_name, NULL, 1, params TSRMLS_CC);
}


/*
	HessianRef::__construct
*/
static PHP_METHOD(HessianRef, __construct)
{
	zval *list, *self;
	long cnt;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &list) == FAILURE) {
		RETURN_FALSE;
	}

	self = getThis();
	if (Z_TYPE_P(list) == IS_ARRAY){
		zval *index;
		ALLOC_ZVAL(index);
		INIT_ZVAL(*index);
		cnt = zend_hash_num_elements(Z_ARRVAL_P(list));
		ZVAL_LONG(&index, cnt - 1);
		zend_update_property(hessian_ref_entry, self, ZEND_STRL("index"), index TSRMLS_DC);
	}else{
		zend_update_property(hessian_ref_entry, self, ZEND_STRL("index"), list TSRMLS_DC);
	}
}



/*
	HessianStreamResult::__construct
*/
static PHP_METHOD(HessianStreamResult, __construct)
{
	zval *stream, *self;


	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &stream) == FAILURE) {
		RETURN_FALSE;
	}

	self = getThis();
	zend_update_property(hessian_ref_entry, self, ZEND_STRL("stream"), stream TSRMLS_DC);
}




//IHessianInterceptor interface
const zend_function_entry ihessian_ignore_code_functions[] = {
	PHP_FE_END
};

//HessianCallingContext functions
const zend_function_entry hessian_class_def_functions[] = {
	PHP_FE_END	/* Must be the last line in hessian_functions[] */
};

//HessianCall functions
const zend_function_entry hessian_call_functions[] = {
	PHP_ME(HessianCall, __construct, 	arginfo_hessian_call_construct,		ZEND_ACC_PUBLIC)
	PHP_FE_END	/* Must be the last line in hessian_functions[] */
};

//HessianRef functions
const zend_function_entry hessian_ref_functions[] = {
	PHP_ME(HessianRef, isRef, 	arginfo_hessian_ref_isref,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianRef, getIndex, 	arginfo_hessian_ref_get_index,		ZEND_ACC_PUBLIC)
	PHP_ME(HessianRef, __construct, 	arginfo_hessian_ref_construct,		ZEND_ACC_PUBLIC)
	PHP_FE_END	/* Must be the last line in hessian_functions[] */
};


//HessianStreamResult functions
const zend_function_entry hessian_stream_result_functions[] = {
	PHP_ME(HessianStreamResult, __construct, 	arginfo_hessian_stream_result_construct,		ZEND_ACC_PUBLIC)
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
