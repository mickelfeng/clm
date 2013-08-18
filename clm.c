/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
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
#include "php_clm.h"

/* 声明全局变量 */
ZEND_DECLARE_MODULE_GLOBALS(clm)

/* {{{ clm_functions[]
 *
 * Every user visible function must have an entry in clm_functions[].
 */
const zend_function_entry clm_functions[] = {
	PHP_FE(clm_set,	NULL)
	PHP_FE(clm_get,	NULL)
	PHP_FE_END	/* Must be the last line in clm_functions[] */
};
/* }}} */

/* {{{ clm_module_entry
 */
zend_module_entry clm_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"clm",
	clm_functions,
	PHP_MINIT(clm),
	PHP_MSHUTDOWN(clm),
	PHP_RINIT(clm),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(clm),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(clm),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_CLM
ZEND_GET_MODULE(clm)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("clm.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_clm_globals, clm_globals)
    STD_PHP_INI_ENTRY("clm.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_clm_globals, clm_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_clm_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_clm_init_globals(zend_clm_globals *clm_globals)
{
	clm_globals->global_value = 0;
	clm_globals->global_string = NULL;
}
*/
/* }}} */

#define CLM_HT_DTOR clm_hash_destructor

static int clm_init_cache_ht()
{
	zend_hash_init(CLM_G(cache_ht), 0, NULL, CLM_HT_DTOR, 1);
	array_init(CLM_G(cache_ht));
}

int clm_hash_destructor(HashTable *ht){
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(clm)
{
	clm_init_cached();
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(clm)
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
PHP_RINIT_FUNCTION(clm)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(clm)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(clm)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "clm support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/**
 * clm_set
 * @param string $key
 * @param mixed $value
 */
PHP_FUNCTION(clm_set)
{
	char *key;
	int key_len, ret;
	zval *val;
	smart_str buf = {0};
	php_serialize_data_t var_hash;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &key, &key_len, &val) == FAILURE) {
		return;
	}

	ret = clm_add(CLM_G(cache_ht), key, key_len, val_tmp);
	if (ret == SUCCESS){
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

PHP_FUNCTION(clm_get)
{
	char *key;
	int key_len, ret;
	zval *val;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE) {
		return;
	}

	ret = zend_hash_find(Z_ARRVAL_P(CLM_G(cache_ht)), key, key_len, (void **)&val);

	RETURN_ZVAL(val, 1, 0);
}

typedef struct _clm_ele {
	int type;
	union {
		int lval;
		double dval;
		HashTable *ht;
		struct {
			char *val;
			int len;
		} str;
	} value;
} clm_ele;

clm_ele *clm_make_ele_from_zval(zval *zval)
{
	clm_ele *ele;

	ele = pemalloc(sizeof(clm_ele), 1);
	ele->type = Z_TYPE_P(zval);
	switch (Z_TYPE_P(zval) & IS_CONSTANT_TYPE_MASK){
		case IS_NULL:
			ele->value.lval = Z_NULL;
			break;
		case IS_LONG:
			ele->value.lval = Z_LVAL_P(zval);
			break;
		case IS_BOOL:
			ele->value.lval = Z_BVAL_P(zval);
			break;
		case IS_DOUBLE:
			ele->value.dval = Z_DVAL_P(zval);
			break;
		case IS_ARRAY:
			ele->value.ht = Z_ARRVAL_P(zval);
			break;
		case IS_STRING:
			ele->value.len = Z_STRLEN_P(zval);
			ele->value.str = pestrndup(Z_STRVAL_P(zval), Z_STRLEN_P(zval), 1);
			break;
	}
	return ele;
}

zval *clm_make_zval_from_ele(clm_ele *ele)
{
	zval *zval;
	MAKE_STD_ZVAL(zval);
	Z_TYPE_P(zval) = ele->type;

	switch(Z_TYPE_P(zval) & IS_CONSTANT_TYPE_MASK){
		case IS_NULL:
			ZVAL_NULL(zval);
			break;
		case IS_LONG:
			break;
		case IS_BOOL:
			ZVAL_LONG(zval, ele->value.lval);
			break;
		case IS_DOUBLE:
			ZVAL_DOUBLE(zval, ele->value.dval);
			break;
		case IS_ARRAY:
			zval *arr;
			MAKE_STD_ZVAL(arr);
			array_init(arr);
			/* TODO 遍历:递归 */
			break;
		case IS_STRING:
			ZVAL_STRINGL(zval, ele->value.str.val, ele->value.str.len, 1);
			break;
	}
	}
}

int clm_cache_set(HashTable *ht, const char *key, int key_len, zval *v)
{
	char *real_key;
	clm_ele *ele;

	/* 寻找当前的真实key */
	for (real_key = key + key_len - 1; real_key >= key && real_key - 1 != '.'; real_key --);

	switch (Z_TYPE_P(v) & IS_CONSTANT_TYPE_MASK){
		case IS_NULL:
			ele = clm_make_ele_from_zval(v);
			zend_hash_update(ht, real_key, key + key_len - real_key, ele, sizeof(clm_ele), NULL);
			break;
		case IS_LONG:
			ele = clm_make_ele_from_zval(v);
			zend_hash_update(ht, real_key, key + key_len - real_key, ele, sizeof(clm_ele), NULL);
			break;
		case IS_DOUBLE:
			break;
		case IS_BOOL:
			break;
		case IS_ARRAY:
			break;
		case IS_STRING:
			break;
		case IS_RESOURCE:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "clm_set don't support resource type, error key is [%s]", key);
			break;
		case IS_OBJECT:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "clm_set don't support object type, error key is [%s]", key);
			break;
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
