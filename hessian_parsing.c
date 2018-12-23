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


/*
	HessianRuleResolver::resolveSymbol
*/
hessian_parsing_rule hessian_rule_resolver_resolve_symbol(char symbol, char *type_expected)
{
	byte index;
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
			if strcmp(rule.type, split[i]) == 0){
				//throw new HessianParsingException("Type $typeExpected expected");
				//@todo:error code
				zend_throw_exception(Hessian_parsing_exception_entry, sprintf("Type %s expected", type_expected), 8);
			}
		}
	}

	return rule;
}



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
