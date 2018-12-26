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

ZEND_BEGIN_ARG_INFO_EX(arginfo_ihessian_custom_writer_write, 0, 0, 1)
	ZEND_ARG_INFO(0, writer) /* string */
	ZEND_ARG_INFO(0, data) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ihessian_interceptor_before_request, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, context, "HessianCallingContext", 0) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ihessian_interceptor_after_request, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, context, "HessianCallingContext", 0) /* string */
ZEND_END_ARG_INFO()



zend_class_entry *ihessian_object_factory_entry;
zend_class_entry *ihessian_custom_writer_entry;
zend_class_entry *ihessian_interceptor_entry;
zend_class_entry *hessian_calling_context_entry;




//IDubboStorage interface
const zend_function_entry ihessian_object_factory_functions[] = {
	PHP_ABSTRACT_ME(IHessianObjectFactory, getObject, arginfo_ihessian_object_factory_get_object)
	PHP_ABSTRACT_ME(IHessianObjectFactory, setOptions, arginfo_ihessian_object_factory_set_options)
	PHP_FE_END
};


//IHessianCustomWriter interface
const zend_function_entry ihessian_custom_writer_functions[] = {
	PHP_ABSTRACT_ME(IHessianCustomWriter, write, arginfo_ihessian_custom_writer_write)
	PHP_FE_END
};



//HessianCallingContext functions
const zend_function_entry hessian_calling_context_functions[] = {
	PHP_FE_END	/* Must be the last line in hessian_functions[] */
};



//IHessianInterceptor interface
const zend_function_entry ihessian_interceptor_functions[] = {
	PHP_ABSTRACT_ME(IHessianInterceptor, beforeRequest, arginfo_ihessian_interceptor_before_request)
	PHP_ABSTRACT_ME(IHessianInterceptor, afterRequest, arginfo_ihessian_interceptor_after_request)
	PHP_FE_END
};


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
