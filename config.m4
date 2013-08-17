dnl $Id$
dnl config.m4 for extension clm

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(clm, for clm support,
dnl Make sure that the comment is aligned:
dnl [  --with-clm             Include clm support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(clm, whether to enable clm support,
dnl Make sure that the comment is aligned:
dnl [  --enable-clm           Enable clm support])

if test "$PHP_CLM" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-clm -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/clm.h"  # you most likely want to change this
  dnl if test -r $PHP_CLM/$SEARCH_FOR; then # path given as parameter
  dnl   CLM_DIR=$PHP_CLM
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for clm files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       CLM_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$CLM_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the clm distribution])
  dnl fi

  dnl # --with-clm -> add include path
  dnl PHP_ADD_INCLUDE($CLM_DIR/include)

  dnl # --with-clm -> check for lib and symbol presence
  dnl LIBNAME=clm # you may want to change this
  dnl LIBSYMBOL=clm # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $CLM_DIR/lib, CLM_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_CLMLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong clm lib version or lib not found])
  dnl ],[
  dnl   -L$CLM_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(CLM_SHARED_LIBADD)

  PHP_NEW_EXTENSION(clm, clm.c, $ext_shared)
fi
