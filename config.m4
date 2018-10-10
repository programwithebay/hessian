 PHP_ARG_WITH(hessian, for hessian support,
 Make sure that the comment is aligned:
 [  --with-hessian             Include hessian support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(hessian, whether to enable hessian support,
dnl Make sure that the comment is aligned:
dnl [  --enable-hessian           Enable hessian support])

if test "$PHP_HESSIAN" != "no"; then
  PHP_REQUIRE_CXX()
  PHP_SUBST(HESSIAN_SHARED_LIBADD)
  PHP_ADD_LIBRARY(stdc++, 1, HESSIAN_SHARED_LIBADD)
  PHP_NEW_EXTENSION(hessian, hessian.cpp dubbo_client.cpp  php_dubbo_client.cpp dubbo_storage.c, $ext_shared)
fi
