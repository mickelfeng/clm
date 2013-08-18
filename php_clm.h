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

#ifndef PHP_CLM_H
#define PHP_CLM_H

extern zend_module_entry clm_module_entry;
#define phpext_clm_ptr &clm_module_entry

#ifdef PHP_WIN32
#	define PHP_CLM_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_CLM_API __attribute__ ((visibility("default")))
#else
#	define PHP_CLM_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(clm);
PHP_MSHUTDOWN_FUNCTION(clm);
PHP_RINIT_FUNCTION(clm);
PHP_RSHUTDOWN_FUNCTION(clm);
PHP_MINFO_FUNCTION(clm);

PHP_FUNCTION(clm_set);
PHP_FUNCTION(clm_get);

/* 定义全局变量 */
ZEND_BEGIN_MODULE_GLOBALS(clm)
	HashTable *cache_ht;
ZEND_END_MODULE_GLOBALS(clm)

/* In every utility function you add that needs to use variables 
   in php_clm_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as CLM_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define CLM_G(v) TSRMG(clm_globals_id, zend_clm_globals *, v)
#else
#define CLM_G(v) (clm_globals.v)
#endif

#endif	/* PHP_CLM_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
