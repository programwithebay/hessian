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
  | Author: Wez Furlong <wez@php.net>                                    |
  |         Marcus Boerger <helly@php.net>                               |
  |         Sterling Hughes <sterling@php.net>                           |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

/* Stuff private to the PDO extension and not for consumption by PDO drivers
 * */
#ifndef _PHP_HESSIAN_INIT_H_
#define _PHP_HESSIAN_INIT_H_ 1


extern zend_object_handlers dubbo_client_object_handlers;
extern zend_object_handlers hessian_buffered_stream_object_handlers;

struct _hessian_parsing_rule{
	char *type;
	char *func;
	char *desc;
}
typedef struct _hessian_parsing_rule hessian_parsing_rule;


struct dubbo_client_object {
    zend_object std;
    //DubboClient *client;
};


void get_service_by_name(zval *name, zval *storage, zval* ret);

//dubbo client
void dubbo_client_free_storage(void *object TSRMLS_DC);

zend_object_value dubbo_client_create_handler(zend_class_entry *type TSRMLS_DC);
PHP_FUNCTION(calltest);




//HessianBufferedStream
void hessian_buffered_stream_free_storage(void *object TSRMLS_DC);

zend_object_value hessian_buffered_stream_create_handler(zend_class_entry *type TSRMLS_DC);



//class entry
extern zend_class_entry *dubbo_client_class_entry;
extern zend_class_entry *dubbo_storage_factory_class_entry;
extern zend_class_entry *dubbo_file_storage_class_entry;
extern zend_class_entry *dubbo_storage_abstract_class_entry;
extern zend_class_entry *idubbo_storage_interface_entry;
extern zend_class_entry *dubbo_service_class_entry;
extern zend_class_entry *hessian_buffered_stream_entry;
extern zend_class_entry *hessian_callback_handler_entry;
extern zend_class_entry *hessian_client_entry;
extern zend_class_entry *hessian_object_factory_entry;
extern zend_class_entry *hessian_client_entry;
extern zend_class_entry *ihessian_object_factory_entry;
extern zend_class_entry *ihessian_custom_writer_entry;
extern zend_class_entry *ihessian_interceptor_entry;
extern zend_class_entry *hessian_calling_context_entry;
extern zend_class_entry *ihessian_ignore_code_entry;
extern zend_class_entry *hessian_class_def_entry;
extern zend_class_entry *hessian_call_entry;
extern zend_class_entry *hessian_ref_entry;
extern zend_class_entry *hessian_stream_result_entry;


//class function
extern const zend_function_entry hessian_functions[];
extern const zend_function_entry dubbo_storage_factory_functions[];
extern const zend_function_entry idubbo_storage_interface_functions[];
extern const zend_function_entry dubbo_storage_abstract_functions[];
extern const zend_function_entry dubbo_file_storage_functions[];
extern const zend_function_entry dubbo_service_functions[];
extern const zend_function_entry hessian_buffered_stream_functions[];
extern const zend_function_entry hessian_callback_handler_functions[];
extern const zend_function_entry hessian_client_functions[];
extern const zend_function_entry hessian_object_factory_functions[];
extern const zend_function_entry hessian_client_functions[];
extern const zend_function_entry ihessian_object_factory_functions[];
extern const zend_function_entry  ihessian_custom_writer_functions[];
extern const zend_function_entry  hessian_calling_context_functions[];
extern const zend_function_entry  ihessian_interceptor_functions[];
extern const zend_function_entry ihessian_ignore_code_functions[];
extern const zend_function_entry hessian_class_def_functions[];
extern const zend_function_entry hessian_class_functions[];
extern const zend_function_entry hessian_ref_functions[];
extern const zend_function_entry hessian_stream_result_functions[];

#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
