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


ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_fault_construct, 0, 0, 2)
	ZEND_ARG_INFO(0, message) /* string */
	ZEND_ARG_INFO(0, code) /* string */
	ZEND_ARG_INFO(0, detail) /* string */
ZEND_END_ARG_INFO()



//entry
zend_class_entry *hessian_parsing_exception_entry;
zend_class_entry *hessian_fault_entry;
zend_class_entry *hessian_exception_entry;



/*
	HessianFault::HessianFault
*/
static PHP_METHOD(HessianFault, __construct)
{
	zval *self;
	zval *code, *message, *detail;
	
	self = getThis();

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &message, &code, &detail)) {
		return;
	}

	zend_update_property(NULL, self, ZEND_STRL("message"), message TSRMLS_CC);
	zend_update_property(NULL, self, ZEND_STRL("code"), code TSRMLS_CC);
	zend_update_property(NULL, self, ZEND_STRL("detail"), detail TSRMLS_CC);
}


//HessianParsingException functions
const zend_function_entry hessian_parsing_exception_functions[] = {
	PHP_FE_END	/* Must be the last line in hessian_functions[] */
};

//HessianFault functions
const zend_function_entry hessian_fault_functions[] = {
	PHP_ME(HessianFault, __construct, 	arginfo_hessian_fault_construct,		ZEND_ACC_PUBLIC)
	PHP_FE_END	/* Must be the last line in hessian_functions[] */
};

//HessianException functions
const zend_function_entry hessian_exception_functions[] = {
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
