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

#ifndef _DUBBO_CLIENT_H_
#define _DUBBO_CLIENT_H_

extern "C" {
	#include "php.h"
	#include "php_ini.h"
	#include "Zend/zend_types.h"
	#include "Zend/zend_hash.h"
}


class DubboClient{
	protected:
		char *version = "1.0.0";
		long connectTimeout = 2;
		long executeTimeout = 5;
		char retries = 2;
		char *dubbo = "2.5.3.1-SNAPSHOT";
		char *loadbalance = "random";
		char *methods;
		char *interface;
		char *owner = "php";
		char *protocol = "http";
		char *side = "consumer";
		char *timestamp;
		HashTable *serviceConfig;
		zend_fcall_info fci;
		zend_fcall_info_cache fci_cache;
		
	public:
		DubboClient();
		void setConnectTimeout(long timeout){connectTimeout = timeout;}
		void setFci(zend_fcall_info arg_fci, zend_fcall_info_cache arg_fci_cache){fci = arg_fci; fci_cache = arg_fci_cache;}
		zend_fcall_info getFci(){return fci;}
		zend_fcall_info_cache getFciCache(){return fci_cache;}
		zval* callService(char *serviceName, char *methodName, zval *params);
};

#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
