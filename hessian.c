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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_hessian.h"
#include "hessian_common.h"
#include "php_hessian_int.h"


/* If you declare any globals in php_hessian.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(hessian)
*/

/* True global resources - no need for thread safety here */
static int le_hessian;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("hessian.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_hessian_globals, hessian_globals)
    STD_PHP_INI_ENTRY("hessian.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_hessian_globals, hessian_globals)
PHP_INI_END()
*/
/* }}} */




/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_hessian_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_hessian_init_globals(zend_hessian_globals *hessian_globals)
{
	hessian_globals->global_value = 0;
	hessian_globals->global_string = NULL;
}
*/
/* }}} */



/*
	register base function 
*/
void register_hessian_base_class(){
	zend_class_entry ce_dubbo_client, ce_idubbo_storage, ce_dubbo_storage_abstract, ce_dubbo_file_storage;
	zend_class_entry ce_dubbo_storage_factory;
	zend_class_entry ce_dubbo_service;
	

	//register interface
	INIT_CLASS_ENTRY(ce_idubbo_storage, "IDubboStorage", idubbo_storage_interface_functions);
	idubbo_storage_interface_entry = zend_register_internal_interface(&ce_idubbo_storage TSRMLS_CC);
	

	//register DubboStorageAbstract Class
	INIT_CLASS_ENTRY(ce_dubbo_storage_abstract, "DubboStorageAbstract", dubbo_storage_abstract_functions);
	//dubbo_storage_abstract_class_entry= zend_register_internal_class_ex(&ce_dubbo_storage_abstract, zend_standard_class_def, NULL TSRMLS_CC);
	dubbo_storage_abstract_class_entry= zend_register_internal_class(&ce_dubbo_storage_abstract TSRMLS_CC);
	zend_class_implements(dubbo_storage_abstract_class_entry TSRMLS_CC, 1, idubbo_storage_interface_entry);
	dubbo_storage_abstract_class_entry->ce_flags |= ZEND_ACC_IMPLICIT_ABSTRACT_CLASS;
	zend_declare_property_null(dubbo_storage_abstract_class_entry, "config", sizeof("config")-1,  ZEND_ACC_PROTECTED TSRMLS_CC);

	//register DubboStorageFactory class
	INIT_CLASS_ENTRY(ce_dubbo_storage_factory, "DubboStorageFactory", dubbo_storage_factory_functions);
	//dubbo_storage_abstract_class_entry= zend_register_internal_class_ex(&ce_dubbo_storage_abstract, zend_standard_class_def, NULL TSRMLS_CC);
	dubbo_storage_factory_class_entry= zend_register_internal_class(&ce_dubbo_storage_factory TSRMLS_CC);
	

	//register DubboFileStorage Class
	INIT_CLASS_ENTRY(ce_dubbo_file_storage, "DubboFileStorage", dubbo_file_storage_functions);
	dubbo_file_storage_class_entry = zend_register_internal_class_ex(&ce_dubbo_file_storage, dubbo_storage_abstract_class_entry, NULL TSRMLS_CC);
	//not a abstract class
	if ( (dubbo_file_storage_class_entry->ce_flags & ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)){
		dubbo_file_storage_class_entry->ce_flags -= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
	}
	if ( (dubbo_file_storage_class_entry->ce_flags & ZEND_ACC_IMPLICIT_ABSTRACT_CLASS)){
		dubbo_file_storage_class_entry->ce_flags -= ZEND_ACC_IMPLICIT_ABSTRACT_CLASS;
	}
	zend_declare_property_null(dubbo_file_storage_class_entry, ZEND_STRL(BASE_PATH),  ZEND_ACC_PROTECTED TSRMLS_CC);

	//register DubboService Class
	INIT_CLASS_ENTRY(ce_dubbo_service, "DubboService", dubbo_service_functions);
	dubbo_service_class_entry = zend_register_internal_class(&ce_dubbo_service TSRMLS_CC);
	zend_declare_property_null(dubbo_service_class_entry,ZEND_STRL("name"),  ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(dubbo_service_class_entry,ZEND_STRL("providers"),  ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(dubbo_service_class_entry,ZEND_STRL("curProviderIndex"),  ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(dubbo_service_class_entry,ZEND_STRL("initProvider"),  ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(dubbo_service_class_entry,ZEND_STRL("consumers"),  ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(dubbo_service_class_entry,ZEND_STRL("storage"),  ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(dubbo_service_class_entry,ZEND_STRL("options"),  ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(dubbo_service_class_entry,ZEND_STRL("dtoMap"),  ZEND_ACC_PROTECTED TSRMLS_CC);


	//register DubboClient Class
	INIT_CLASS_ENTRY(ce_dubbo_client, "DubboClient", hessian_functions);
	dubbo_client_class_entry = zend_register_internal_class(&ce_dubbo_client TSRMLS_CC);

	dubbo_client_class_entry->create_object = dubbo_client_create_handler;
    memcpy(&dubbo_client_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    dubbo_client_object_handlers.clone_obj = NULL;
	
	
	zend_declare_property_null(dubbo_client_class_entry, ZEND_STRL("storage"),  ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(dubbo_client_class_entry, ZEND_STRL("logCallback"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_string(dubbo_client_class_entry, ZEND_STRL("version"), "1.0.0", ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_string(dubbo_client_class_entry, ZEND_STRL("group"), "cn", ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_long(dubbo_client_class_entry, ZEND_STRL("connectTimeout"), 2, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_long(dubbo_client_class_entry, ZEND_STRL("executeTimeout"), 5, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_long(dubbo_client_class_entry, ZEND_STRL("retries"), 2, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_string(dubbo_client_class_entry, ZEND_STRL("dubbo"), "2.5.3.1-SNAPSHOT", ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_string(dubbo_client_class_entry, ZEND_STRL("loadbalance"), "random", ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(dubbo_client_class_entry, ZEND_STRL("methods"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(dubbo_client_class_entry, ZEND_STRL("interface"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_string(dubbo_client_class_entry, ZEND_STRL("owner"), "php", ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_string(dubbo_client_class_entry, ZEND_STRL("protocol"), "http", ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_string(dubbo_client_class_entry, ZEND_STRL("side"), "consumer", ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(dubbo_client_class_entry, ZEND_STRL("timestamp"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(dubbo_client_class_entry, ZEND_STRL("serviceConfig"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(dubbo_client_class_entry, ZEND_STRL("dtoMapConfig"), ZEND_ACC_PROTECTED TSRMLS_CC);
}


/*
	register hessian ext class
*/
void register_hessian_ext_class(){
	zend_class_entry ce_hessian_buffered_stream, ce_hessian_callback_handler_stream, ce_hessian_client;
	zend_class_entry ce_ihessian_object_factory, ce_hessian_object_factory;
	

	//HessianBufferedStream
	INIT_CLASS_ENTRY(ce_hessian_buffered_stream, "HessianBufferedStream", hessian_buffered_stream_functions);
	hessian_buffered_stream_entry = zend_register_internal_class(&ce_hessian_buffered_stream TSRMLS_CC);
	hessian_buffered_stream_entry->create_object = hessian_buffered_stream_create_handler;
    memcpy(&hessian_buffered_stream_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    hessian_buffered_stream_object_handlers.clone_obj = NULL;
	
	zend_declare_property_null(hessian_buffered_stream_entry, ZEND_STRL("fp"), ZEND_ACC_PUBLIC TSRMLS_CC);

	//HessianCallbackHandler
	INIT_CLASS_ENTRY(ce_hessian_callback_handler_stream, "HessianCallbackHandler", hessian_callback_handler_functions);
	hessian_callback_handler_entry = zend_register_internal_class(&ce_hessian_callback_handler_stream TSRMLS_CC);
	zend_declare_property_null(hessian_callback_handler_entry, ZEND_STRL("callbacks"), ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_callback_handler_entry, ZEND_STRL("notFound"), ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_callback_handler_entry, ZEND_STRL("cache"), ZEND_ACC_PUBLIC TSRMLS_CC);

	//HessianClient
	INIT_CLASS_ENTRY(ce_hessian_client, "HessianClient", hessian_client_functions);
	hessian_client_entry = zend_register_internal_class(&ce_hessian_client TSRMLS_CC);
	zend_declare_property_null(hessian_client_entry, ZEND_STRL("url"), ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_client_entry, ZEND_STRL("options"), ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_client_entry, ZEND_STRL("typemap"), ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_client_entry, ZEND_STRL("factory"), ZEND_ACC_PUBLIC TSRMLS_CC);


	INIT_CLASS_ENTRY(ce_ihessian_object_factory, "IHessianObjectFactory", ihessian_object_factory_functions);
	ihessian_object_factory_entry = zend_register_internal_interface(&ce_ihessian_object_factory TSRMLS_CC);

	
	//register DubboStorageAbstract Class
	INIT_CLASS_ENTRY(ce_hessian_object_factory, "HessianObjectFactory", hessian_object_factory_functions);
	//dubbo_storage_abstract_class_entry= zend_register_internal_class_ex(&ce_dubbo_storage_abstract, zend_standard_class_def, NULL TSRMLS_CC);
	hessian_object_factory_entry= zend_register_internal_class(&ce_hessian_object_factory TSRMLS_CC);
	zend_class_implements(hessian_object_factory_entry , 1, ihessian_object_factory_entry TSRMLS_CC);
	zend_declare_property_null(dubbo_storage_abstract_class_entry, "options", sizeof("options")-1,  ZEND_ACC_PROTECTED TSRMLS_CC);
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(hessian)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/

	register_hessian_base_class();

	register_hessian_ext_class();
	
	return SUCCESS;
}


/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(hessian)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(hessian)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(hessian)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(hessian)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "hessian support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ hessian_functions[]
 *
 * Every user visible function must have an entry in hessian_functions[].
 */



/* }}} */

/* {{{ hessian_module_entry
 */
zend_module_entry hessian_module_entry = {
	STANDARD_MODULE_HEADER,
	"hessian",
	hessian_functions,
	PHP_MINIT(hessian),
	PHP_MSHUTDOWN(hessian),
	PHP_RINIT(hessian),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(hessian),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(hessian),
	PHP_HESSIAN_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_HESSIAN
ZEND_GET_MODULE(hessian)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
