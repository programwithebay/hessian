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
//#include "Zend/zend_alloc.c"


#define DECLARE_HESSIAN_RULE(index, type, func, desc) \
	HESSIAN_G(hessian2_rules)[index].type =  type; \
	HESSIAN_G(hessian2_rules)[index].func = func; \
	HESSIAN_G(hessian2_rules)[index].desc = desc; 

ZEND_DECLARE_MODULE_GLOBALS(hessian)


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
	zend_declare_property_null(dubbo_storage_abstract_class_entry, "config", sizeof("config")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);

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
	zend_declare_property_null(dubbo_file_storage_class_entry, ZEND_STRL(BASE_PATH),  ZEND_ACC_PUBLIC TSRMLS_CC);

	//register DubboService Class
	INIT_CLASS_ENTRY(ce_dubbo_service, "DubboService", dubbo_service_functions);
	dubbo_service_class_entry = zend_register_internal_class(&ce_dubbo_service TSRMLS_CC);
	zend_declare_property_null(dubbo_service_class_entry,ZEND_STRL("name"),  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(dubbo_service_class_entry,ZEND_STRL("providers"),  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_long(dubbo_service_class_entry,ZEND_STRL("curProviderIndex"),  0, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_bool(dubbo_service_class_entry,ZEND_STRL("initProvider"),  0, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(dubbo_service_class_entry,ZEND_STRL("consumers"),  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(dubbo_service_class_entry,ZEND_STRL("storage"),  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(dubbo_service_class_entry,ZEND_STRL("options"),  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(dubbo_service_class_entry,ZEND_STRL("dtoMap"),  ZEND_ACC_PUBLIC TSRMLS_CC);


	//register DubboClient Class
	INIT_CLASS_ENTRY(ce_dubbo_client, "DubboClient", hessian_functions);
	dubbo_client_class_entry = zend_register_internal_class(&ce_dubbo_client TSRMLS_CC);


	dubbo_client_class_entry->create_object = dubbo_client_create_handler;
    memcpy(&dubbo_client_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    dubbo_client_object_handlers.clone_obj = NULL;
	
	zend_declare_property_null(dubbo_client_class_entry, ZEND_STRL("storage"),  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(dubbo_client_class_entry, ZEND_STRL("logCallback"), ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_string(dubbo_client_class_entry, ZEND_STRL("version"), "1.0.0", ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_string(dubbo_client_class_entry, ZEND_STRL("group"), "cn", ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_long(dubbo_client_class_entry, ZEND_STRL("connectTimeout"), 2, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_long(dubbo_client_class_entry, ZEND_STRL("executeTimeout"), 5, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_long(dubbo_client_class_entry, ZEND_STRL("retries"), 2, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_string(dubbo_client_class_entry, ZEND_STRL("dubbo"), "2.5.3.1-SNAPSHOT", ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_string(dubbo_client_class_entry, ZEND_STRL("loadbalance"), "random", ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(dubbo_client_class_entry, ZEND_STRL("methods"), ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(dubbo_client_class_entry, ZEND_STRL("interface"), ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_string(dubbo_client_class_entry, ZEND_STRL("owner"), "php", ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_string(dubbo_client_class_entry, ZEND_STRL("protocol"), "http", ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_string(dubbo_client_class_entry, ZEND_STRL("side"), "consumer", ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(dubbo_client_class_entry, ZEND_STRL("timestamp"), ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(dubbo_client_class_entry, ZEND_STRL("serviceConfig"), ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(dubbo_client_class_entry, ZEND_STRL("dtoMapConfig"), ZEND_ACC_PUBLIC TSRMLS_CC);
}


/*
	register hessian ext class
*/
void register_hessian_ext_class(){
	zend_class_entry ce_hessian_buffered_stream, ce_hessian_callback_handler_stream, ce_hessian_client;
	zend_class_entry ce_ihessian_object_factory, ce_hessian_object_factory;
	zend_class_entry ce_ihessian_custom_writer, ce_hessian_calling_context, ce_hessian_interceptor;
	

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

	//

	
	//Hessian Interfaces
	INIT_CLASS_ENTRY(ce_ihessian_object_factory, "IHessianObjectFactory", ihessian_object_factory_functions);
	ihessian_object_factory_entry = zend_register_internal_interface(&ce_ihessian_object_factory TSRMLS_CC);
	INIT_CLASS_ENTRY(ce_ihessian_custom_writer, "IHessianCustomWriter", ihessian_custom_writer_functions);
	ihessian_custom_writer_entry= zend_register_internal_interface(&ce_ihessian_custom_writer TSRMLS_CC);
	INIT_CLASS_ENTRY(ce_hessian_calling_context, "HessianCallingContext", hessian_calling_context_functions);
	hessian_calling_context_entry  = zend_register_internal_class(&ce_hessian_calling_context TSRMLS_CC);
	zend_declare_property_null(hessian_calling_context_entry, "writer", sizeof("writer")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_calling_context_entry, "parser", sizeof("parser")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_calling_context_entry, "transport", sizeof("transport")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_calling_context_entry, "typemap", sizeof("typemap")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_calling_context_entry, "options", sizeof("options")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_calling_context_entry, "stream", sizeof("stream")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_bool(hessian_calling_context_entry, "isClient", sizeof("isClient")-1, 1,ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_calling_context_entry, "call", sizeof("call")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_calling_context_entry, "url", sizeof("url")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_calling_context_entry, "payload", sizeof("payload")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_calling_context_entry, "error", sizeof("error")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);


	//HessianOptions
	zend_class_entry ce_hessian_options;
	INIT_CLASS_ENTRY(ce_hessian_options, "HessianOptions", hessian_options_functions);
	hessian_options_entry = zend_register_internal_class(&ce_hessian_options TSRMLS_CC);
	zend_declare_property_long(hessian_options_entry, "version", sizeof("version")-1,  2, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_string(hessian_options_entry, "transport", sizeof("transport")-1, "CURL", ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_options_entry, "transportOptions", sizeof("transportOptions")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_bool(hessian_options_entry, "detectVersion", sizeof("detectVersion")-1, 0, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_options_entry, "objectFactory", sizeof("objectFactory")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_options_entry, "typeMap", sizeof("typeMap")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_bool(hessian_options_entry, "strictTypes", sizeof("strictTypes")-1,  0, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_options_entry, "headers", sizeof("headers")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_options_entry, "interceptors", sizeof("interceptors")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_options_entry, "timeZone", sizeof("timeZone")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_bool(hessian_options_entry, "saveRaw", sizeof("saveRaw")-1, 0, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_string(hessian_options_entry, "serviceName", sizeof("serviceName")-1, "", ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_bool(hessian_options_entry, "displayInfo", sizeof("displayInfo")-1, 0, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_bool(hessian_options_entry, "ignoreOutput", sizeof("ignoreOutput")-1, 0, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_options_entry, "parseFilters", sizeof("parseFilters")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_options_entry, "writeFilters", sizeof("writeFilters")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_options_entry, "before", sizeof("before")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_options_entry, "after", sizeof("after")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);

	
	INIT_CLASS_ENTRY(ce_hessian_interceptor, "IHessianInterceptor", ihessian_interceptor_functions);
	ihessian_interceptor_entry = zend_register_internal_interface(&ce_hessian_interceptor TSRMLS_CC);

	
	//register HessianObjectFactory Class
	INIT_CLASS_ENTRY(ce_hessian_object_factory, "HessianObjectFactory", hessian_object_factory_functions);
	//dubbo_storage_abstract_class_entry= zend_register_internal_class_ex(&ce_dubbo_storage_abstract, zend_standard_class_def, NULL TSRMLS_CC);
	hessian_object_factory_entry= zend_register_internal_class(&ce_hessian_object_factory TSRMLS_CC);
	zend_class_implements(hessian_object_factory_entry , 1, ihessian_object_factory_entry TSRMLS_CC);
	zend_declare_property_null(hessian_object_factory_entry, "options", sizeof("options")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);

	zend_class_entry ce_hessian_datetime_adapter, ce_hessian_factory;
	INIT_CLASS_ENTRY(ce_hessian_datetime_adapter, "HessianDatetimeAdapter", hessian_datetime_adapter_functions);
	hessian_datetime_adapter_entry = zend_register_internal_class(&ce_hessian_datetime_adapter TSRMLS_CC);
	
	INIT_CLASS_ENTRY(ce_hessian_factory, "HessianFactory", hessian_factory_functions);
	hessian_factory_entry = zend_register_internal_class(&ce_hessian_factory TSRMLS_CC);


	//HessianIgnoreCode && HessianClassDef
	zend_class_entry ce_ihessian_ignore_code, ce_hessian_class_def;
	INIT_CLASS_ENTRY(ce_ihessian_ignore_code, "HessianIgnoreCode", ihessian_ignore_code_functions);
	ihessian_ignore_code_entry = zend_register_internal_interface(&ce_ihessian_ignore_code TSRMLS_CC);

	INIT_CLASS_ENTRY(ce_hessian_class_def, "HessianClassDef", hessian_class_def_functions);
	hessian_class_def_entry = zend_register_internal_class(&ce_hessian_class_def TSRMLS_CC);
	zend_class_implements(hessian_class_def_entry , 1, ihessian_ignore_code_entry TSRMLS_CC);
	zend_declare_property_null(hessian_class_def_entry, "type", sizeof("type")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_class_def_entry, "remoteType", sizeof("remoteType")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_class_def_entry, "props", sizeof("props")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);


	//HessianService
	zend_class_entry ce_hessian_service;
	INIT_CLASS_ENTRY(ce_hessian_service, "HessianService", hessian_service_functions);
	hessian_service_entry = zend_register_internal_class(&ce_hessian_service TSRMLS_CC);
	zend_declare_property_bool(hessian_service_entry, "fault", sizeof("fault")-1, 0, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_service_entry, "options", sizeof("options")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_service_entry, "service", sizeof("service")-1,	ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_service_entry, "reflected", sizeof("reflected")-1,	ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_service_entry, "typemap", sizeof("typemap")-1,	ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_service_entry, "factory", sizeof("factory")-1,	ZEND_ACC_PUBLIC TSRMLS_CC);

	
	zend_class_entry ce_hessian_call, ce_hessian_ref;
	INIT_CLASS_ENTRY(ce_hessian_call, "HessianCall", hessian_call_functions);
	hessian_call_entry = zend_register_internal_class(&ce_hessian_call TSRMLS_CC);
	zend_declare_property_null(hessian_call_entry, "method", sizeof("method")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_call_entry, "arguments", sizeof("arguments")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	
	INIT_CLASS_ENTRY(ce_hessian_ref, "HessianRef", hessian_ref_functions);
	hessian_ref_entry = zend_register_internal_class(&ce_hessian_ref TSRMLS_CC);
	zend_declare_property_long(hessian_call_entry, "index", sizeof("index")-1,  0, ZEND_ACC_PUBLIC TSRMLS_CC);


	zend_class_entry ce_hessian_stream_result;
	INIT_CLASS_ENTRY(ce_hessian_stream_result, "HessianStreamResult", hessian_stream_result_functions);
	hessian_stream_result_entry= zend_register_internal_class(&ce_hessian_stream_result TSRMLS_CC);

	//HessianReferenceMap
	zend_class_entry ce_hessian_reference_map;
	INIT_CLASS_ENTRY(ce_hessian_reference_map, "HessianReferenceMap", hessian_reference_map_functions);
	hessian_reference_map_entry = zend_register_internal_class(&ce_hessian_reference_map TSRMLS_CC);
	zend_declare_property_null(hessian_call_entry, "reflist", sizeof("reflist")-1,  ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_call_entry, "typelist", sizeof("typelist")-1,	ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_call_entry, "classlist", sizeof("classlist")-1,	ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_call_entry, "objectlist", sizeof("objectlist")-1,	ZEND_ACC_PUBLIC TSRMLS_CC);

	
	//HessinaStream
	zend_class_entry ce_hessian_stream;
	INIT_CLASS_ENTRY(ce_hessian_stream, "HessianStream", hessian_stream_functions);
	hessian_stream_entry= zend_register_internal_class(&ce_hessian_stream TSRMLS_CC);
	zend_declare_property_long(hessian_stream_entry, "pos", sizeof("pos")-1,  0, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_long(hessian_stream_entry, "len", sizeof("len")-1,  0, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_stream_entry, "bytes", sizeof("bytes")-1, ZEND_ACC_PUBLIC TSRMLS_CC);

	//IHessianTransport && HessianCURLTransport
	zend_class_entry ce_ihessian_transport, ce_hessian_curl_transport;
	INIT_CLASS_ENTRY(ce_ihessian_transport, "IHessianTransport", ihessian_transport_functions);
	ihessian_transport_entry = zend_register_internal_interface(&ce_ihessian_transport TSRMLS_CC);


	INIT_CLASS_ENTRY(ce_hessian_curl_transport, "HessianCURLTransport", hessian_curl_transport_functions);
	hessian_curl_transport_entry = zend_register_internal_class(&ce_hessian_curl_transport TSRMLS_CC);
	zend_class_implements(hessian_curl_transport_entry , 1, ihessian_transport_entry TSRMLS_CC);
	zend_declare_property_null(hessian_curl_transport_entry, "metadata", sizeof("metadata")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_curl_transport_entry, "rawData", sizeof("rawData")-1, ZEND_ACC_PUBLIC TSRMLS_CC);

	//HessianTypeMap and hessian utils
	zend_class_entry ce_ihessian_type_map, ce_hessian_utils;
	INIT_CLASS_ENTRY(ce_ihessian_type_map, "HessianTypeMap", hessian_type_map_functions);
	hessian_type_map_entry =  zend_register_internal_class(&ce_ihessian_type_map TSRMLS_CC);
	zend_declare_property_null(hessian_type_map_entry, "types", sizeof("types")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_type_map_entry, "localRules", sizeof("localRules")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_type_map_entry, "remoteRules", sizeof("remoteRules")-1, ZEND_ACC_PUBLIC TSRMLS_CC);

	INIT_CLASS_ENTRY(ce_hessian_utils, "HessianUtils", hessian_utils_functions);
	hessian_utils_entry =  zend_register_internal_class(&ce_hessian_utils TSRMLS_CC);
	zend_declare_property_null(hessian_utils_entry, "littleEndian", sizeof("littleEndian")-1, ZEND_ACC_PUBLIC TSRMLS_CC);

	zend_declare_class_constant_long(hessian_utils_entry, "pow32", sizeof("pow32")-1, 4294967296L TSRMLS_CC);

	//Hessian2Writer
	zend_class_entry ce_hessian2_writer;
	INIT_CLASS_ENTRY(ce_hessian2_writer, "Hessian2Writer", hessian2_writer_functions);
	hessian2_writer_entry =  zend_register_internal_class(&ce_hessian2_writer TSRMLS_CC);
	zend_declare_property_null(hessian2_writer_entry, "refmap", sizeof("refmap")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian2_writer_entry, "typemap", sizeof("typemap")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian2_writer_entry, "logMsg", sizeof("logMsg")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian2_writer_entry, "options", sizeof("options")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian2_writer_entry, "filterContainer", sizeof("filterContainer")-1, ZEND_ACC_PUBLIC TSRMLS_CC);

	
	//Hessian2IteratorWriter
	zend_class_entry ce_hessian2_iterator_writer, ce_hessian2_service_parser, ce_hessian2_parser;
	INIT_CLASS_ENTRY(ce_hessian2_iterator_writer, "Hessian2IteratorWriter", hessian2_iterator_writer_functions);
	hessian2_iterator_writer_entry =  zend_register_internal_class(&ce_hessian2_iterator_writer TSRMLS_CC);
	zend_declare_property_null(hessian2_iterator_writer_entry, "usetype", sizeof("usetype")-1, ZEND_ACC_PUBLIC TSRMLS_CC);

	//Hessian2Parser
	INIT_CLASS_ENTRY(ce_hessian2_parser, "Hessian2Parser", hessian2_parser_functions);
	hessian2_parser_entry =  zend_register_internal_class(&ce_hessian2_parser TSRMLS_CC);
	zend_declare_property_null(hessian2_iterator_writer_entry, "resolver", sizeof("resolver")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian2_iterator_writer_entry, "stream", sizeof("stream")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian2_iterator_writer_entry, "refmap", sizeof("refmap")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian2_iterator_writer_entry, "typemap", sizeof("typemap")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian2_iterator_writer_entry, "log", sizeof("log")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian2_iterator_writer_entry, "objectFactory", sizeof("objectFactory")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian2_iterator_writer_entry, "options", sizeof("options")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian2_iterator_writer_entry, "filterContainer", sizeof("filterContainer")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	
	//Hessian2ServiceParser
	INIT_CLASS_ENTRY(ce_hessian2_service_parser, "Hessian2ServiceParser", hessian2_service_parser_functions);
	hessian2_service_parser_entry =  zend_register_internal_class_ex(&ce_hessian2_service_parser, hessian2_parser_entry, NULL TSRMLS_CC);

	//Hessian2ServiceWriter
	zend_class_entry ce_hessian2_service_writer;
	INIT_CLASS_ENTRY(ce_hessian2_service_writer, "Hessian2ServiceWriter", hessian2_service_writer_functions);
	hessian2_service_writer_entry =  zend_register_internal_class_ex(&ce_hessian2_service_writer, hessian2_writer_entry, NULL TSRMLS_CC);

	
	//HessianParsingException
	zend_class_entry ce_hessian_parsing_exception, ce_hessian_fault, ce_hessian_exception;
	zend_class_entry *ce_exception, **p_ce_exception;

	//ce_exception = zend_fetch_class("Exception", sizeof("Exception")-1 , 0 TSRMLS_CC);
	zend_hash_find(CG(class_table), "exception", sizeof("exception"), (void **) &p_ce_exception);
	ce_exception = *p_ce_exception;

	
	INIT_CLASS_ENTRY(ce_hessian_parsing_exception, "HessianParsingException", hessian_parsing_exception_functions);
	hessian_parsing_exception_entry =  zend_register_internal_class_ex(&ce_hessian_parsing_exception, ce_exception, NULL TSRMLS_CC);
	zend_declare_property_null(hessian_parsing_exception_entry, "position", sizeof("position")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(hessian_parsing_exception_entry, "details", sizeof("details")-1, ZEND_ACC_PUBLIC TSRMLS_CC);

	INIT_CLASS_ENTRY(ce_hessian_fault, "HessianFault", hessian_fault_functions);
	hessian_fault_entry =  zend_register_internal_class_ex(&ce_hessian_fault, ce_exception, NULL TSRMLS_CC);
	zend_declare_property_null(hessian_parsing_exception_entry, "detail", sizeof("detail")-1, ZEND_ACC_PUBLIC TSRMLS_CC);

	INIT_CLASS_ENTRY(ce_hessian_exception, "HessianException", hessian_exception_functions);
	hessian_exception_entry =  zend_register_internal_class_ex(&ce_hessian_exception, ce_exception, NULL TSRMLS_CC);	
}


//declare hessian2_rules
void inline hessian_declare_global_hessian_rules(int index, char *type, char *func, char *desc)
{
	HESSIAN_G(hessian2_rules)[index].type = type;
	HESSIAN_G(hessian2_rules)[index].func = func;
	HESSIAN_G(hessian2_rules)[index].desc = desc;
}

//init hessian globals
void init_hessian_globals(){

	//HESSIAN_G(hessian2_rules)[0].type= = {.type="binary", .funct="binary0", .desc="binary data length 0-16"};
	
	//who not?
	//DECLARE_HESSIAN_RULE(0, "binary", "binary0", "binary data length 0-16")

	hessian_declare_global_hessian_rules(0, "binary", "binary0", "binary data length 0-16");
	hessian_declare_global_hessian_rules(1, "binary", "binary1", "binary data length 0-1023");
	hessian_declare_global_hessian_rules(2, "binary", "binaryLong", "8-bit binary data non-final chunk ('A')");
	hessian_declare_global_hessian_rules(3, "binary", "binaryLong", "8-bit binary data final chunk ('B')");
	hessian_declare_global_hessian_rules(4, "boolean", "bool", "boolean false ('F')");
	hessian_declare_global_hessian_rules(5, "boolean", "bool", "boolean true ('T')");
	hessian_declare_global_hessian_rules(6, "date", "date", "64-bit UTC millisecond date");
	hessian_declare_global_hessian_rules(7, "date", "compactDate", "32-bit UTC minute date");
	hessian_declare_global_hessian_rules(8, "double", "double64", "64-bit IEEE encoded double ('D')");
	hessian_declare_global_hessian_rules(9, "double", "double1", "double 0.0");
	hessian_declare_global_hessian_rules(10, "double", "double1", "double 1.0");
	hessian_declare_global_hessian_rules(11, "double", "double1", "double represented as byte (-128.0 to 127.0)");
	hessian_declare_global_hessian_rules(12, "double", "double2", "double represented as short (-32768.0 to 327676.0)");
	hessian_declare_global_hessian_rules(13, "double", "double4", "double represented as float");
	hessian_declare_global_hessian_rules(14, "integer", "parseInt", "32-bit signed integer ('I')");
	hessian_declare_global_hessian_rules(15, "integer", "compactInt1", "one-octet compact int (-x10 to x3f, x90 is 0)");
	hessian_declare_global_hessian_rules(16,"integer", "compactInt2", "two-octet compact int (-x800 to x7ff)");
	hessian_declare_global_hessian_rules(17, "integer", "compactInt3", "three-octet compact int (-x40000 to x3ffff)");
	hessian_declare_global_hessian_rules(18,"list", "vlenList", "variable-length list/vector ('U')");
	hessian_declare_global_hessian_rules(19,"list", "flenList", "fixed-length list/vector ('V')");
	hessian_declare_global_hessian_rules(20,"list", "vlenUntypedList", "variable-length untyped list/vector ('W')");
	hessian_declare_global_hessian_rules(21,"list", "flenUntypedList", "fixed-length untyped list/vector ('X')");
	hessian_declare_global_hessian_rules(22,"list", "directListTyped", "fixed list with direct length");
	hessian_declare_global_hessian_rules(23,"list", "directListUntyped", "fixed untyped list with direct length");
	hessian_declare_global_hessian_rules(24,"long", "long3", "three-octet compact long (-x40000 to x3ffff)");
	hessian_declare_global_hessian_rules(25,"long", "long64", "64-bit signed long integer ('L')");
	hessian_declare_global_hessian_rules(26,"long", "long32", "long encoded as 32-bit int ('Y')");
	hessian_declare_global_hessian_rules(27,"long", "long1", "one-octet compact long (-x8 to xf, xe0 is 0)");
	hessian_declare_global_hessian_rules(28,"long", "long2", "two-octet compact long (-x800 to x7ff, xf8 is 0)");
	hessian_declare_global_hessian_rules(29,"map", "untypedMap", "untyped map ('H')");
	hessian_declare_global_hessian_rules(30,"map", "typedMap", "map with type ('M')");
	hessian_declare_global_hessian_rules(31,"null", "parseNull", "null ('N')");
	hessian_declare_global_hessian_rules(32,"object", "typeDefinition", "object type definition ('C')");
	hessian_declare_global_hessian_rules(33,"object", "objectInstance", "object instance ('O')");
	hessian_declare_global_hessian_rules(34,"object", "objectDirect", "object with direct type");
	hessian_declare_global_hessian_rules(35,"reference", "reference", "reference to map/list/object - integer ('Q')");
	hessian_declare_global_hessian_rules(36,"reserved", "reserved", "reserved (expansion/escape)");
	hessian_declare_global_hessian_rules(37,"reserved", "reserved", "reserved");
	hessian_declare_global_hessian_rules(38,"reserved", "reserved", "reserved");
	hessian_declare_global_hessian_rules(39,"reserved", "reserved", "reserved");
	hessian_declare_global_hessian_rules(40,"string", "string0", "utf-8 string length 0-32");
	hessian_declare_global_hessian_rules(41,"string", "string1", "utf-8 string length 0-1023");
	hessian_declare_global_hessian_rules(42,"string", "stringLong", "utf-8 string non-final chunk ('R')");
	hessian_declare_global_hessian_rules(43,"string", "stringLong", "utf-8 string final chunk ('S')");
	hessian_declare_global_hessian_rules(44,"terminator", "terminator", "list/map terminator ('Z')");

	//symboles
	HESSIAN_G(hessian2_symbols)[32] = 0;
	HESSIAN_G(hessian2_symbols)[33] = 0;
	HESSIAN_G(hessian2_symbols)[34] = 0;
	HESSIAN_G(hessian2_symbols)[35] = 0;
	HESSIAN_G(hessian2_symbols)[36] = 0;
	HESSIAN_G(hessian2_symbols)[37] = 0;
	HESSIAN_G(hessian2_symbols)[38] = 0;
	HESSIAN_G(hessian2_symbols)[39] = 0;
	HESSIAN_G(hessian2_symbols)[40] = 0;
	HESSIAN_G(hessian2_symbols)[41] = 0;
	HESSIAN_G(hessian2_symbols)[42] = 0;
	HESSIAN_G(hessian2_symbols)[43] = 0;
	HESSIAN_G(hessian2_symbols)[44] = 0;
	HESSIAN_G(hessian2_symbols)[45] = 0;
	HESSIAN_G(hessian2_symbols)[46] = 0;
	HESSIAN_G(hessian2_symbols)[47] = 0;
	HESSIAN_G(hessian2_symbols)[52] = 1;
	HESSIAN_G(hessian2_symbols)[53] = 1;
	HESSIAN_G(hessian2_symbols)[54] = 1;
	HESSIAN_G(hessian2_symbols)[55] = 1;
	HESSIAN_G(hessian2_symbols)[65] = 2;
	HESSIAN_G(hessian2_symbols)[66] = 3;
	HESSIAN_G(hessian2_symbols)[70] = 4;
	HESSIAN_G(hessian2_symbols)[84] = 5;
	HESSIAN_G(hessian2_symbols)[74] = 6;
	HESSIAN_G(hessian2_symbols)[75] = 7;
	HESSIAN_G(hessian2_symbols)[68] = 8;
	HESSIAN_G(hessian2_symbols)[91] = 9;
	HESSIAN_G(hessian2_symbols)[92] = 10;
	HESSIAN_G(hessian2_symbols)[93] = 11;
	HESSIAN_G(hessian2_symbols)[94] = 12;
	HESSIAN_G(hessian2_symbols)[95] = 13;
	HESSIAN_G(hessian2_symbols)[73] = 14;
	HESSIAN_G(hessian2_symbols)[128] = 15;
	HESSIAN_G(hessian2_symbols)[129] = 15;
	HESSIAN_G(hessian2_symbols)[130] = 15;
	HESSIAN_G(hessian2_symbols)[131] = 15;
	HESSIAN_G(hessian2_symbols)[132] = 15;
	HESSIAN_G(hessian2_symbols)[133] = 15;
	HESSIAN_G(hessian2_symbols)[134] = 15;
	HESSIAN_G(hessian2_symbols)[135] = 15;
	HESSIAN_G(hessian2_symbols)[136] = 15;
	HESSIAN_G(hessian2_symbols)[137] = 15;
	HESSIAN_G(hessian2_symbols)[138] = 15;
	HESSIAN_G(hessian2_symbols)[139] = 15;
	HESSIAN_G(hessian2_symbols)[140] = 15;
	HESSIAN_G(hessian2_symbols)[141] = 15;
	HESSIAN_G(hessian2_symbols)[142] = 15;
	HESSIAN_G(hessian2_symbols)[143] = 15;
	HESSIAN_G(hessian2_symbols)[144] = 15;
	HESSIAN_G(hessian2_symbols)[145] = 15;
	HESSIAN_G(hessian2_symbols)[146] = 15;
	HESSIAN_G(hessian2_symbols)[147] = 15;
	HESSIAN_G(hessian2_symbols)[148] = 15;
	HESSIAN_G(hessian2_symbols)[149] = 15;
	HESSIAN_G(hessian2_symbols)[150] = 15;
	HESSIAN_G(hessian2_symbols)[151] = 15;
	HESSIAN_G(hessian2_symbols)[152] = 15;
	HESSIAN_G(hessian2_symbols)[153] = 15;
	HESSIAN_G(hessian2_symbols)[154] = 15;
	HESSIAN_G(hessian2_symbols)[155] = 15;
	HESSIAN_G(hessian2_symbols)[156] = 15;
	HESSIAN_G(hessian2_symbols)[157] = 15;
	HESSIAN_G(hessian2_symbols)[158] = 15;
	HESSIAN_G(hessian2_symbols)[159] = 15;
	HESSIAN_G(hessian2_symbols)[160] = 15;
	HESSIAN_G(hessian2_symbols)[161] = 15;
	HESSIAN_G(hessian2_symbols)[162] = 15;
	HESSIAN_G(hessian2_symbols)[163] = 15;
	HESSIAN_G(hessian2_symbols)[164] = 15;
	HESSIAN_G(hessian2_symbols)[165] = 15;
	HESSIAN_G(hessian2_symbols)[166] = 15;
	HESSIAN_G(hessian2_symbols)[167] = 15;
	HESSIAN_G(hessian2_symbols)[168] = 15;
	HESSIAN_G(hessian2_symbols)[169] = 15;
	HESSIAN_G(hessian2_symbols)[170] = 15;
	HESSIAN_G(hessian2_symbols)[171] = 15;
	HESSIAN_G(hessian2_symbols)[172] = 15;
	HESSIAN_G(hessian2_symbols)[173] = 15;
	HESSIAN_G(hessian2_symbols)[174] = 15;
	HESSIAN_G(hessian2_symbols)[175] = 15;
	HESSIAN_G(hessian2_symbols)[176] = 15;
	HESSIAN_G(hessian2_symbols)[177] = 15;
	HESSIAN_G(hessian2_symbols)[178] = 15;
	HESSIAN_G(hessian2_symbols)[179] = 15;
	HESSIAN_G(hessian2_symbols)[180] = 15;
	HESSIAN_G(hessian2_symbols)[181] = 15;
	HESSIAN_G(hessian2_symbols)[182] = 15;
	HESSIAN_G(hessian2_symbols)[183] = 15;
	HESSIAN_G(hessian2_symbols)[184] = 15;
	HESSIAN_G(hessian2_symbols)[185] = 15;
	HESSIAN_G(hessian2_symbols)[186] = 15;
	HESSIAN_G(hessian2_symbols)[187] = 15;
	HESSIAN_G(hessian2_symbols)[188] = 15;
	HESSIAN_G(hessian2_symbols)[189] = 15;
	HESSIAN_G(hessian2_symbols)[190] = 15;
	HESSIAN_G(hessian2_symbols)[191] = 15;
	HESSIAN_G(hessian2_symbols)[192] = 16;
	HESSIAN_G(hessian2_symbols)[193] = 16;
	HESSIAN_G(hessian2_symbols)[194] = 16;
	HESSIAN_G(hessian2_symbols)[195] = 16;
	HESSIAN_G(hessian2_symbols)[196] = 16;
	HESSIAN_G(hessian2_symbols)[197] = 16;
	HESSIAN_G(hessian2_symbols)[198] = 16;
	HESSIAN_G(hessian2_symbols)[199] = 16;
	HESSIAN_G(hessian2_symbols)[200] = 16;
	HESSIAN_G(hessian2_symbols)[201] = 16;
	HESSIAN_G(hessian2_symbols)[202] = 16;
	HESSIAN_G(hessian2_symbols)[203] = 16;
	HESSIAN_G(hessian2_symbols)[204] = 16;
	HESSIAN_G(hessian2_symbols)[205] = 16;
	HESSIAN_G(hessian2_symbols)[206] = 16;
	HESSIAN_G(hessian2_symbols)[207] = 16;
	HESSIAN_G(hessian2_symbols)[208] = 17;
	HESSIAN_G(hessian2_symbols)[209] = 17;
	HESSIAN_G(hessian2_symbols)[210] = 17;
	HESSIAN_G(hessian2_symbols)[211] = 17;
	HESSIAN_G(hessian2_symbols)[212] = 17;
	HESSIAN_G(hessian2_symbols)[213] = 17;
	HESSIAN_G(hessian2_symbols)[214] = 17;
	HESSIAN_G(hessian2_symbols)[215] = 17;
	HESSIAN_G(hessian2_symbols)[85] = 18;
	HESSIAN_G(hessian2_symbols)[86] = 19;
	HESSIAN_G(hessian2_symbols)[87] = 20;
	HESSIAN_G(hessian2_symbols)[88] = 21;
	HESSIAN_G(hessian2_symbols)[112] = 22;
	HESSIAN_G(hessian2_symbols)[113] = 22;
	HESSIAN_G(hessian2_symbols)[114] = 22;
	HESSIAN_G(hessian2_symbols)[115] = 22;
	HESSIAN_G(hessian2_symbols)[116] = 22;
	HESSIAN_G(hessian2_symbols)[117] = 22;
	HESSIAN_G(hessian2_symbols)[118] = 22;
	HESSIAN_G(hessian2_symbols)[119] = 22;
	HESSIAN_G(hessian2_symbols)[120] = 23;
	HESSIAN_G(hessian2_symbols)[121] = 23;
	HESSIAN_G(hessian2_symbols)[122] = 23;
	HESSIAN_G(hessian2_symbols)[123] = 23;
	HESSIAN_G(hessian2_symbols)[124] = 23;
	HESSIAN_G(hessian2_symbols)[125] = 23;
	HESSIAN_G(hessian2_symbols)[126] = 23;
	HESSIAN_G(hessian2_symbols)[127] = 23;
	HESSIAN_G(hessian2_symbols)[56] = 24;
	HESSIAN_G(hessian2_symbols)[57] = 24;
	HESSIAN_G(hessian2_symbols)[58] = 24;
	HESSIAN_G(hessian2_symbols)[59] = 24;
	HESSIAN_G(hessian2_symbols)[60] = 24;
	HESSIAN_G(hessian2_symbols)[61] = 24;
	HESSIAN_G(hessian2_symbols)[62] = 24;
	HESSIAN_G(hessian2_symbols)[63] = 24;
	HESSIAN_G(hessian2_symbols)[76] = 25;
	HESSIAN_G(hessian2_symbols)[89] = 26;
	HESSIAN_G(hessian2_symbols)[216] = 27;
	HESSIAN_G(hessian2_symbols)[217] = 27;
	HESSIAN_G(hessian2_symbols)[218] = 27;
	HESSIAN_G(hessian2_symbols)[219] = 27;
	HESSIAN_G(hessian2_symbols)[220] = 27;
	HESSIAN_G(hessian2_symbols)[221] = 27;
	HESSIAN_G(hessian2_symbols)[222] = 27;
	HESSIAN_G(hessian2_symbols)[223] = 27;
	HESSIAN_G(hessian2_symbols)[224] = 27;
	HESSIAN_G(hessian2_symbols)[225] = 27;
	HESSIAN_G(hessian2_symbols)[226] = 27;
	HESSIAN_G(hessian2_symbols)[227] = 27;
	HESSIAN_G(hessian2_symbols)[228] = 27;
	HESSIAN_G(hessian2_symbols)[229] = 27;
	HESSIAN_G(hessian2_symbols)[230] = 27;
	HESSIAN_G(hessian2_symbols)[231] = 27;
	HESSIAN_G(hessian2_symbols)[232] = 27;
	HESSIAN_G(hessian2_symbols)[233] = 27;
	HESSIAN_G(hessian2_symbols)[234] = 27;
	HESSIAN_G(hessian2_symbols)[235] = 27;
	HESSIAN_G(hessian2_symbols)[236] = 27;
	HESSIAN_G(hessian2_symbols)[237] = 27;
	HESSIAN_G(hessian2_symbols)[238] = 27;
	HESSIAN_G(hessian2_symbols)[239] = 27;
	HESSIAN_G(hessian2_symbols)[240] = 28;
	HESSIAN_G(hessian2_symbols)[241] = 28;
	HESSIAN_G(hessian2_symbols)[242] = 28;
	HESSIAN_G(hessian2_symbols)[243] = 28;
	HESSIAN_G(hessian2_symbols)[244] = 28;
	HESSIAN_G(hessian2_symbols)[245] = 28;
	HESSIAN_G(hessian2_symbols)[246] = 28;
	HESSIAN_G(hessian2_symbols)[247] = 28;
	HESSIAN_G(hessian2_symbols)[248] = 28;
	HESSIAN_G(hessian2_symbols)[249] = 28;
	HESSIAN_G(hessian2_symbols)[250] = 28;
	HESSIAN_G(hessian2_symbols)[251] = 28;
	HESSIAN_G(hessian2_symbols)[252] = 28;
	HESSIAN_G(hessian2_symbols)[253] = 28;
	HESSIAN_G(hessian2_symbols)[254] = 28;
	HESSIAN_G(hessian2_symbols)[255] = 28;
	HESSIAN_G(hessian2_symbols)[72] = 29;
	HESSIAN_G(hessian2_symbols)[77] = 30;
	HESSIAN_G(hessian2_symbols)[78] = 31;
	HESSIAN_G(hessian2_symbols)[67] = 32;
	HESSIAN_G(hessian2_symbols)[79] = 33;
	HESSIAN_G(hessian2_symbols)[96] = 34;
	HESSIAN_G(hessian2_symbols)[97] = 34;
	HESSIAN_G(hessian2_symbols)[98] = 34;
	HESSIAN_G(hessian2_symbols)[99] = 34;
	HESSIAN_G(hessian2_symbols)[100] = 34;
	HESSIAN_G(hessian2_symbols)[101] = 34;
	HESSIAN_G(hessian2_symbols)[102] = 34;
	HESSIAN_G(hessian2_symbols)[103] = 34;
	HESSIAN_G(hessian2_symbols)[104] = 34;
	HESSIAN_G(hessian2_symbols)[105] = 34;
	HESSIAN_G(hessian2_symbols)[106] = 34;
	HESSIAN_G(hessian2_symbols)[107] = 34;
	HESSIAN_G(hessian2_symbols)[108] = 34;
	HESSIAN_G(hessian2_symbols)[109] = 34;
	HESSIAN_G(hessian2_symbols)[110] = 34;
	HESSIAN_G(hessian2_symbols)[111] = 34;
	HESSIAN_G(hessian2_symbols)[81] = 35;
	HESSIAN_G(hessian2_symbols)[64] = 36;
	HESSIAN_G(hessian2_symbols)[69] = 37;
	HESSIAN_G(hessian2_symbols)[71] = 38;
	HESSIAN_G(hessian2_symbols)[80] = 39;
	HESSIAN_G(hessian2_symbols)[0] = 40;
	HESSIAN_G(hessian2_symbols)[1] = 40;
	HESSIAN_G(hessian2_symbols)[2] = 40;
	HESSIAN_G(hessian2_symbols)[3] = 40;
	HESSIAN_G(hessian2_symbols)[4] = 40;
	HESSIAN_G(hessian2_symbols)[5] = 40;
	HESSIAN_G(hessian2_symbols)[6] = 40;
	HESSIAN_G(hessian2_symbols)[7] = 40;
	HESSIAN_G(hessian2_symbols)[8] = 40;
	HESSIAN_G(hessian2_symbols)[9] = 40;
	HESSIAN_G(hessian2_symbols)[10] = 40;
	HESSIAN_G(hessian2_symbols)[11] = 40;
	HESSIAN_G(hessian2_symbols)[12] = 40;
	HESSIAN_G(hessian2_symbols)[13] = 40;
	HESSIAN_G(hessian2_symbols)[14] = 40;
	HESSIAN_G(hessian2_symbols)[15] = 40;
	HESSIAN_G(hessian2_symbols)[16] = 40;
	HESSIAN_G(hessian2_symbols)[17] = 40;
	HESSIAN_G(hessian2_symbols)[18] = 40;
	HESSIAN_G(hessian2_symbols)[19] = 40;
	HESSIAN_G(hessian2_symbols)[20] = 40;
	HESSIAN_G(hessian2_symbols)[21] = 40;
	HESSIAN_G(hessian2_symbols)[22] = 40;
	HESSIAN_G(hessian2_symbols)[23] = 40;
	HESSIAN_G(hessian2_symbols)[24] = 40;
	HESSIAN_G(hessian2_symbols)[25] = 40;
	HESSIAN_G(hessian2_symbols)[26] = 40;
	HESSIAN_G(hessian2_symbols)[27] = 40;
	HESSIAN_G(hessian2_symbols)[28] = 40;
	HESSIAN_G(hessian2_symbols)[29] = 40;
	HESSIAN_G(hessian2_symbols)[30] = 40;
	HESSIAN_G(hessian2_symbols)[31] = 40;
	HESSIAN_G(hessian2_symbols)[48] = 41;
	HESSIAN_G(hessian2_symbols)[49] = 41;
	HESSIAN_G(hessian2_symbols)[50] = 41;
	HESSIAN_G(hessian2_symbols)[51] = 41;
	HESSIAN_G(hessian2_symbols)[82] = 42;
	HESSIAN_G(hessian2_symbols)[83] = 43;
	HESSIAN_G(hessian2_symbols)[90] = 44;
}

/*
void test_macro()
{
	unsigned int value;

	value = ZEND_MM_ALIGNED_HEADER_SIZE;
	printf("ZEND_MM_ALIGNED_HEADER_SIZE=%d  \n", value);

	value = ZEND_MM_ALIGNED_FREE_HEADER_SIZE;
	printf("ZEND_MM_ALIGNED_FREE_HEADER_SIZE=%d \n", value);

	value = ZEND_MM_ALIGNMENT;
	printf("ZEND_MM_ALIGNMENT=%d \n", value);

	value = ZEND_MM_MIN_ALLOC_BLOCK_SIZE;
	printf("ZEND_MM_MIN_ALLOC_BLOCK_SIZE=%d  \n", value);

	value = END_MAGIC_SIZE;
	printf("END_MAGIC_SIZE=%d  \n", value);

	value = ZEND_MM_ALIGNED_MIN_HEADER_SIZE;
	printf("ZEND_MM_ALIGNED_MIN_HEADER_SIZE=%d  \n", value);

	value = ZEND_MM_ALIGNED_SEGMENT_SIZE;
	printf("ZEND_MM_ALIGNED_SEGMENT_SIZE=%d \n", value);

	value = ZEND_MM_MIN_SIZE;
	printf("ZEND_MM_MIN_SIZE=%d \n", value);

	value = ZEND_MM_ALIGNMENT_LOG2;
	printf("ZEND_MM_ALIGNMENT_LOG2=%d \n", value);


	value = ZEND_MM_MAX_SMALL_SIZE;
	printf("ZEND_MM_MAX_SMALL_SIZE=%d \n", value);
	
	exit(0);
}
*/


/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(hessian)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	register_hessian_base_class();

	register_hessian_ext_class();

	init_hessian_globals();
	
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
