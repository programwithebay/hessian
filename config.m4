 PHP_ARG_WITH(hessian, for hessian support,
 Make sure that the comment is aligned:
 [  --with-hessian             Include hessian support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(hessian, whether to enable hessian support,
dnl Make sure that the comment is aligned:
dnl [  --enable-hessian           Enable hessian support])

if test "$PHP_HESSIAN" != "no"; then
  dnl PHP_REQUIRE_CXX()
  PHP_SUBST(HESSIAN_SHARED_LIBADD)
  dnl PHP_ADD_LIBRARY(stdc++, 1, HESSIAN_SHARED_LIBADD)
  PHP_NEW_EXTENSION(hessian, hessian.c  php_dubbo_client.c dubbo_storage.c dubbo_service.c hessian_buffered_stream.c hessian_callback_handler.c hessian_client.c hessian_factory.c hessian_parsing.c hessian_interface.c hessian_options.c hessian_reference_map.c hessian_service.c hessian_stream.c hessian_transport.c hessian_type_map.c hessian_utils.c hessian2_iterator_writer.c hessian2_parser.c hessian2_service_parser.c hessian2_service_writer.c, $ext_shared)
fi
