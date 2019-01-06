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
#include "php_hessian_init.h"


//params
ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_type_map_construct, 0, 0, 1)
	ZEND_ARG_INFO(0, map) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_type_map_map_type, 0, 0, 2)
	ZEND_ARG_INFO(0, local) /* string */
	ZEND_ARG_INFO(0, remote) /* string */
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_type_map_get_local_type, 0, 0, 1)
	ZEND_ARG_INFO(0, remoteType) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_type_map_get_remote_type, 0, 0, 1)
	ZEND_ARG_INFO(0, localType) /* string */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hessian_type_map_merge, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, map, "HessianTypeMap", O) /* string */
ZEND_END_ARG_INFO()



//entry
zend_class_entry *hessian_type_map_entry;

//check is rule
zend_bool hessian_type_map_is_rule(zval *string){
	char *buf;
	int i,len;
	
	if (Z_TYPE_P(string) != IS_STRING){
		return 0;
	}
	buf = Z_STRVAL_P(string);
	len = Z_STRLEN_P(string);
	for(i=0; i<len; i++){
		if (buf[i] == '*'){
			return 1;
		}
	}
	return 0;
}

//rule to regexp
char* hessian_type_map_rule_to_regexp(zval *string){
	zval function_name;
	zval *params[2];
	zval param1, param2;
	zval *rule;

	ZVAL_STRING(function_name, "str_replace");
	params[0] = string;
	call_user_function(EG(function_table), NULL, &function_name, rule, 1, params TSRMLS_DC);
	//$rule = str_replace('.', '\.', $string);
	//return '/' . str_replace('*', self::REG_ALL, $rule) . '/';

	ZVAL_STRING(&param1, "*");
	ZVAL_STRING(&param2, "([_0-9a-z\-]*)");
	params[0] = &param1;
	params[1] = &param2;
	params[2] = rule;
	call_user_function(EG(function_table), NULL, &function_name, rule, 1, params TSRMLS_DC);

	char *buf;

	buf = pemalloc(Z_STRLEN_P(rule) + 2);
	if (!buf){
		php_error_docref(NULL, E_ERROR, "hessian_type_map_rule_to_regexp alloc memory error");
		return;
	}
	buf[0] = '/';
	memcpy(buf+1, Z_STRVAL_P(rule), Z_STRLEN_P(rule));
	buf[1+Z_STRVAL_P(rule)] = '/';
	buf[1+Z_STRVAL_P(rule)+1] = 0;
	return buf;
}

//map type
void hessian_type_map_map_type(zval* self, zval *local, zval *remote){
	zend_bool rule_local, rule_remote;
	
	rule_local = hessian_type_map_is_rule(local);
	rule_remote = hessian_type_map_is_rule(remote);
	if ( rule_local && rule_remote){
		//throw new Exception('Typemap : Cannot use wildcards in both local and remote types');
		zend_class_entry ce_exception;
		ce_exception = zend_fetch_class("Exception", strlen("Exception")-1, 0);
		zend_throw_exception(ce_exception, "Typemap : Cannot use wildcards in both local and remote types", 0);
		return;
	}

	/*
		if($ruleLocal){
			$rule = self::ruleToRegexp($local);
			$this->localRules[$rule] = $remote;
		} else
		if($ruleRemote){
			$rule = self::ruleToRegexp($remote);
			$this->remoteRules[$rule] = $local;
		} else
			$this->types[$remote] = $local;
	*/
	if (rule_local){
		char *rule;
		zval *local_rules;
		rule = hessian_type_map_rule_to_regexp(local);
		if (rule){
			local_rules = zend_read_property(NULL, self, ZEND_STRL("localRules"), 1 TSRMLS_DC);
			if (Z_TYPE_P(local_rules) = IS_ARRAY){
				array_init_size(local_rules, 2);
			}
			zend_hash_add(Z_ARRVAL_P(local_rules), rule, strlen(rule)-1, &remote, sizeof(zval**), NULL);
		}else{
		}
	}else{
		if (rule_remote){
			char *rule;
			zval *remote_rules;
			rule = hessian_type_map_rule_to_regexp(remote);
			if (rule){
				remote_rules = zend_read_property(NULL, self, ZEND_STRL("remoteRules"), 1 TSRMLS_DC);
				if (Z_TYPE_P(remote_rules) = IS_ARRAY){
					array_init_size(remote_rules, 2);
				}
				zend_hash_add(Z_ARRVAL_P(remote_rules), rule, strlen(rule)-1, &local, sizeof(zval**), NULL);
			}
		}else{
			char *rule;
			zval *types;

			types = zend_read_property(NULL, self, ZEND_STRL("types"), 1 TSRMLS_DC);
			if (Z_TYPE_P(types) = IS_ARRAY){
				array_init_size(types, 2);
			}
			zend_hash_add(Z_ARRVAL_P(types), Z_STRVAL_P(remote), Z_STRLEN_P(remote), &local, sizeof(zval**), NULL);
		}
	}
}

/*
	HessianTypeMap::__construct
*/
static PHP_METHOD(HessianTypeMap, __construct)
{
	zval *map;
	zval *self;
	HashPosition *pos;
	zval **src_entry;
	char *string_key;
	uint string_key_len;
	ulong num_key;
	zval local;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &map)) {
		return;
	}
	self = getThis();
	if (Z_TYPE_P(map) != IS_ARRAY){
		php_error_docref(NULL, E_WARNING, "map must be an array");
		return;
	}
	/*
		foreach($map as $local => $remote){
			$this->mapType($local, $remote);		
		}
	*/
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(map), &pos);
	
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(map), (void **)&src_entry, &pos) == SUCCESS) {
		zend_hash_get_current_key_ex(Z_ARRVAL_P(map), &string_key, &string_key_len, &num_key, 0, &pos);
		ZVAL_STRING(local, string_key, 0);
		hessian_type_map_map_type(self, &local, *src_entry);
		zend_hash_move_forward_ex(Z_ARRVAL_P(map), &pos);
	}
}


/*
	HessianTypeMap::mapType
*/
static PHP_METHOD(HessianTypeMap, mapType)
{
	zval *local, *remote;
	zval *self;
	zend_bool rule_local, rule_remote;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &local, &remote)) {
		return;
	}
	self = getThis();
	hessian_type_map_map_type(self, local, remote);
}


/*
	HessianTypeMap::getLocalType
*/
static PHP_METHOD(HessianTypeMap, getLocalType)
{
	zval *remote_type;
	zval *self, *types;
	zend_class_entry *ce_remote_type;
	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z",  &remote_type)) {
		return;
	}
	if (Z_TYPE_P(remote_type) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "remoteType must be an string");
		return;
	}
	self = getThis();

	/*
		if(class_exists($remoteType))
			return $remoteType;
	*/
	ce_remote_type = zend_fetch_class(Z_STRVAL_P(remote_type), Z_STRLEN_P(remote_type), 0);
	if (ce_remote_type){
		RETURN_ZVAL(remote_type, 0);
	}


	/*
		if(isset($this->types[$remoteType])){
			$local = $this->types[$remoteType];
			return $local != 'array' ? $local : false; 
		}
		foreach($this->remoteRules as $rule => $local){
			if(preg_match($rule, $remoteType)){
				$this->types[$remoteType] = $local;
				return $local != 'array' ? $local : false; 
			}
		}
		return $remoteType;
	*/

	types = zend_read_property(NULL, self, ZEND_STRL("types"), 1 TSRMLS_DC);
	if (Z_TYPE_P(types) == IS_ARRAY){
		zval *local;
		if (SUCCESS == zend_hash_find(Z_ARRVAL_P(types), Z_STRVAL_P(remote_type), Z_STRLEN_P(remote_type), &local)){
			if(strcmp(Z_STRVAL_P(local), "array") == 0){
				RETURN_ZVAL(local, 1);
			}else{
				RETURN_FALSE;
			}
		}
	}
}



/*
	HessianTypeMap::getRemoteType
*/
static PHP_METHOD(HessianTypeMap, getRemoteType)
{
	zval *local_type, *remote, *types;
	zval *local_rules;
	zval *self;
	HashPosition *pos;
	zval *entry;

	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z",  &local_type)) {
		return;
	}
	if (Z_TYPE_P(local_type) != IS_STRING){
		php_error_docref(NULL, E_WARNING, "remoteType must be an string");
		return;
	}
	self = getThis();

	/*
		$remote = array_search($localType, $this->types); 
		if($remote !== false)
			return $remote;
	*/
	types = zend_read_property(NULL, self, ZEND_STRL("types"), 1 TSRMLS_DC);
	if (Z_TYPE_P(types) == IS_ARRAY){
		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(types), &pos);
		zval *compare_result;
		while (zend_hash_get_current_data_ex(Z_ARRVAL_P(types), (void **)&entry, &pos) == SUCCESS) {
			if (compare_function(compare_result, entry, local_type TSRMLS_CC) == FAILURE) {
				RETURN_ZVAL(entry, 1);
			}
			zend_hash_move_forward_ex(Z_ARRVAL_P(types), &pos);
		}
	}

	/*
		foreach($this->localRules as $rule => $remote){
			if(preg_match($rule, $localType)){
				return $remote;
			}
		}
		//return false;
		return $localType;
	*/
	char *str_key;
	uint *num_index;
	zval function_name, rule;
	zval *params[2];
	zval *preg_res;
	
	local_rules = zend_read_property(NULL, self, ZEND_STRL("localRules"), 1 TSRMLS_DC);
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(local_rules), &pos);
	ZVAL_STRING(function_name, "preg_match");
	while (zend_hash_get_current_data_ex(Z_ARRVAL_P(local_rules), (void **)&entry, &pos) == SUCCESS) {
		zend_hash_get_current_key(Z_ARRVAL_P(local_rules), &str_key, &num_index, 0);
		ZVAL_STRING(rule, str_key);
		params[0] = rule;
		params[1] = local_type;
		call_user_function(EG(function_table), NULL, &function_name, preg_res, 2, params TSRMLS_DC);
		if (i_zend_is_true(preg_res)){
			RETURN_ZVAL(remote, 1);
		}
		zend_hash_move_forward_ex(Z_ARRVAL_P(local_rules), &pos);
	}

	RETURN_ZVAL(local_type, 1);
}



/*
	HessianTypeMap::merge
*/
static PHP_METHOD(HessianTypeMap, merge)
{
	zval *map;
	zval *self;
	zval *types, *local_rules, *remote_rules;
	zval *map_types, *map_local_rules, *map_remote_rules;

	
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z",  &map)) {
		return;
	}
	self = getThis();

	/*
		$this->types = array_merge($this->types, $map->types);
		$this->localRules = array_merge($this->localRules, $map->localRules);
		$this->remoteRules = array_merge($this->remoteRules, $map->remoteRules);
	*/
	types = zend_read_property(NULL, self, ZEND_STRL("types"), 1 TSRMLS_DC);
	local_rules = zend_read_property(NULL, self, ZEND_STRL("localRules"), 1 TSRMLS_DC);
	remote_rules = zend_read_property(NULL, self, ZEND_STRL("remoteRules"), 1 TSRMLS_DC);

	map_types = zend_read_property(NULL, map, ZEND_STRL("types"), 1 TSRMLS_DC);
	map_local_rules = zend_read_property(NULL, map, ZEND_STRL("localRules"), 1 TSRMLS_DC);
	map_remote_rules = zend_read_property(NULL, map, ZEND_STRL("remoteRules"), 1 TSRMLS_DC);

	php_array_merge(types, map_types, 0 TSRMLS_DC);
	php_array_merge(local_rules, map_local_rules, 0 TSRMLS_DC);
	php_array_merge(remote_rules, map_remote_rules, 0 TSRMLS_DC);

	zend_update_property(NULL, self, ZEND_STRL("types"), types TSRMLS_DC);
	zend_update_property(NULL, self, ZEND_STRL("localRules"), local_rules TSRMLS_DC);
	zend_update_property(NULL, self, ZEND_STRL("remoteRules"), remote_rules TSRMLS_DC);
}


//HessianTypeMap functions
const zend_function_entry hessian_type_map_functions[] = {
	PHP_ME(HessianTypeMap, __construct, arginfo_hessian_type_map_construct, ZEND_ACC_PUBLIC)
	PHP_ME(HessianTypeMap, mapType, arginfo_hessian_type_map_map_type, ZEND_ACC_PUBLIC)
	PHP_ME(HessianTypeMap, getLocalType, arginfo_hessian_type_map_get_local_type, ZEND_ACC_PUBLIC)
	PHP_ME(HessianTypeMap, getRemoteType, arginfo_hessian_type_map_get_remote_type, ZEND_ACC_PUBLIC)
	PHP_ME(HessianTypeMap, merge, arginfo_hessian_type_map_merge, ZEND_ACC_PUBLIC)
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
