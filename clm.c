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

static void clm_zval_dtor(void *pDest);
static zval *clm_zval_persistent(zval *val);
static zval *clm_zval_localize(zval *pval, zval *val);

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

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(clm)
{
	CLM_G(cache_ht) = (HashTable *)pemalloc(sizeof(HashTable), 1);
	zend_hash_init(CLM_G(cache_ht), 0, NULL, clm_zval_dtor, 1);
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
	zend_hash_destroy(CLM_G(cache_ht));
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


/** {{{ proto clm_set(string $key, mixed $value)
 * local cache set
 */
PHP_FUNCTION(clm_set)
{
	char *key, *pkey;
	int key_len, ret;
	zval *val, *pval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &key, &key_len, &val) == FAILURE) {
		return;
	}

	pval = clm_zval_persistent(val TSRMLS_CC);
	if (pval == NULL){
		RETURN_FALSE;
	}
	pkey = pestrndup(key, key_len, 1);
	if (pkey == NULL){
		RETURN_FALSE;
	}

	if (FAILURE == zend_hash_update(CLM_G(cache_ht), pkey, key_len, pval, sizeof(zval), NULL)){
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

PHP_FUNCTION(clm_get)
{
	char *key;
	int key_len, ret;
	zval *pval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE) {
		return;
	}

	if (FAILURE == zend_hash_find(CLM_G(cache_ht), key, key_len, (void **)&pval)){
		RETURN_FALSE;
	}
	if (NULL == clm_zval_localize(pval, return_value)){
		RETURN_FALSE;
	}
}

/** {{{ destructor for persistent zval of clm local cache
 */
static void clm_zval_dtor(void *pDest) {
	zval *val;
	val = (zval *)pDest;
	switch (Z_TYPE_P(val) & IS_CONSTANT_TYPE_MASK){
		case IS_STRING:
			CHECK_ZVAL_STRING(val);
			pefree(Z_STRVAL_P(val), 1);
			break;
		case IS_ARRAY:
			zend_hash_destroy(Z_ARRVAL_P(val));
			pefree(Z_ARRVAL_P(val), 1);
			break;
	}
}
/* }}} */

/** {{{ persistent zval for clm local cache
 */
static zval *clm_zval_persistent(zval *val TSRMLS_DC)
{
	zval *pval;

	pval = (zval *)pemalloc(sizeof(zval), 1);
	INIT_PZVAL(pval);
	
	switch (Z_TYPE_P(val) & IS_CONSTANT_TYPE_MASK){
		case IS_BOOL:
			ZVAL_BOOL(pval, Z_BVAL_P(val));
			break;
		case IS_LONG:
			ZVAL_LONG(pval, Z_LVAL_P(val));
			break;
		case IS_DOUBLE:
			ZVAL_DOUBLE(pval, Z_DVAL_P(val));
			break;
		case IS_STRING:
			CHECK_ZVAL_STRING(val);
			Z_TYPE_P(pval) = IS_STRING;
			Z_STRLEN_P(pval) = Z_STRLEN_P(val);
			Z_STRVAL_P(pval) = pestrndup(Z_STRVAL_P(val), Z_STRLEN_P(val), 1);
			break;
		case IS_ARRAY: {
			char *str_index;
			ulong num_index;
			uint str_index_len;
			zval **ele_val, *ele_pval;

			Z_TYPE_P(pval) = IS_ARRAY;
			Z_ARRVAL_P(pval) = (HashTable *)pemalloc(sizeof(HashTable), 1);
			if (Z_ARRVAL_P(pval) == NULL){
				return NULL;
			}
			zend_hash_init(Z_ARRVAL_P(pval), zend_hash_num_elements(Z_ARRVAL_P(val)), NULL, clm_zval_dtor, 1);

			for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(val));
					!zend_hash_has_more_elements(Z_ARRVAL_P(val));
					zend_hash_move_forward(Z_ARRVAL_P(val))){
				if (FAILURE == zend_hash_get_current_data(Z_ARRVAL_P(val), (void **)&ele_val)){
					continue;
				}
				ele_pval = clm_zval_persistent(*ele_val TSRMLS_CC);
				if (ele_pval == NULL){
					continue;
				}
				if (HASH_KEY_IS_LONG == zend_hash_get_current_key_ex(Z_ARRVAL_P(val), &str_index, &str_index_len, &num_index, 0, NULL)){
					zend_hash_index_update(Z_ARRVAL_P(pval), num_index, (void **)&ele_pval, sizeof(zval), NULL);
				} else {
					zend_hash_update(Z_ARRVAL_P(pval), str_index, str_index_len, (void **)&ele_pval, sizeof(zval), NULL);
				}
			}
			break;
		}
		case IS_NULL:
		case IS_OBJECT:
		case IS_RESOURCE:
			// ignore object and resource
			ZVAL_NULL(pval);
			break;
	}
	return pval;
}
/* }}} */

/** {{{ make a persistent zval localize
 */
static zval *clm_zval_localize(zval *pval, zval *val TSRMLS_DC)
{
	if (val == NULL){
		MAKE_STD_ZVAL(val);
	}
	
	switch (Z_TYPE_P(pval) & IS_CONSTANT_TYPE_MASK){
		case IS_BOOL:
			ZVAL_BOOL(val, Z_BVAL_P(pval));
			break;
		case IS_LONG:
			ZVAL_LONG(val, Z_LVAL_P(pval));
			break;
		case IS_DOUBLE:
			ZVAL_DOUBLE(val, Z_DVAL_P(pval));
			break;
		case IS_STRING:
			CHECK_ZVAL_STRING(pval);
			Z_TYPE_P(val) = IS_STRING;
			Z_STRLEN_P(val) = Z_STRLEN_P(pval);
			Z_STRVAL_P(val) = estrndup(Z_STRVAL_P(pval), Z_STRLEN_P(val));
			break;
		case IS_ARRAY: {
			char *str_index;
			ulong num_index;
			uint str_index_len;
			zval *ele_val, **ele_pval;

			array_init(val);

			for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(pval));
					!zend_hash_has_more_elements(Z_ARRVAL_P(pval));
					zend_hash_move_forward(Z_ARRVAL_P(pval))){
				if (FAILURE == zend_hash_get_current_data(Z_ARRVAL_P(pval), (void **)&ele_pval)){
					continue;
				}
				ele_val = clm_zval_localize(*ele_pval, NULL TSRMLS_CC);
				if (ele_val == NULL){
					continue;
				}
				if (HASH_KEY_IS_LONG == zend_hash_get_current_key_ex(Z_ARRVAL_P(pval), &str_index, &str_index_len, &num_index, 0, NULL)){
					zend_hash_index_update(Z_ARRVAL_P(val), num_index, (void **)&ele_val, sizeof(zval), NULL);
				} else {
					zend_hash_update(Z_ARRVAL_P(val), str_index, str_index_len, (void **)&ele_val, sizeof(zval), NULL);
				}
			}
			break;
		}
		case IS_NULL:
			ZVAL_NULL(val);
			break;
	}
	return val;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
