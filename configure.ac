NC_MAJOR=0
NC_MINOR=4
NC_SUB=0
NC_PRE=""

AC_INIT([netclass], [0.4.0])

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_SRCDIR([src/netclass.cpp])

AC_SUBST(NC_MAJOR)
AC_SUBST(NC_MINOR)
AC_SUBST(NC_SUB)

AC_DEFINE_UNQUOTED(NC_MAJOR,$NC_MAJOR)
AC_DEFINE_UNQUOTED(NC_MINOR,$NC_MINOR)
AC_DEFINE_UNQUOTED(NC_SUB,$NC_SUB)

AM_INIT_AUTOMAKE

dnl netclass 0.3.0 deprecates the ncconfig.h in place of using the
dnl compiler flag options at compile time
dnl AM_CONFIG_HEADER(ncconfig.h)

dnl AC_DISABLE_SHARED
AM_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)
AC_STDC_HEADERS
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_CXX

dnl netclass 0.3.0 deprecates the use of ranlib in favor of libtool
dnl AC_PROG_RANLIB


dnl
dnl Checks for typedefs, structures, and compiler characteristics.
dnl
AC_C_CONST
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_SIZE_T

AC_CHECK_SIZEOF(unsigned short, 2)
AC_CHECK_SIZEOF(unsigned int, 4)
AC_CHECK_SIZEOF(unsigned long, 4)


dnl
dnl Common cflags for all platforms
dnl
COMMON_CFLAGS="-D_REENTRANT"

dnl
dnl Check whether or not to use debugging
dnl
AC_ARG_ENABLE(debug,
        [  --enable-debug          Enable debug support in NetClass [default=no]],
        [ac_cv_enable_debug=$enableval], [ac_cv_enable_debug=no])
AC_MSG_CHECKING([whether to use debugging])
if test $ac_cv_enable_debug = yes; then
        AC_MSG_RESULT(yes)
        AC_DEFINE(NCDEBUG)
        CPPFLAGS="$COMMON_CFLAGS $CPPFLAGS -ggdb"
else
        AC_MSG_RESULT(no)
        CPPFLAGS="$COMMON_CFLAGS $CPPFLAGS"
fi

dnl
dnl Check whether or not to use threads
dnl
AC_ARG_ENABLE(threads,
        [  --enable-threads          Enable thread support in NetClass [default=yes]],
        [ac_cv_enable_threads=$enableval], [ac_cv_enable_threads=yes])
AC_MSG_CHECKING([whether to use threads])
if test $ac_cv_enable_threads = yes; then
        AC_MSG_RESULT(yes)
        AC_DEFINE(HAVE_THREADS)
else
        AC_MSG_RESULT(no)
fi


dnl
dnl pthread support
dnl
if test $ac_cv_enable_threads = yes; then
AC_CHECK_LIB(pthread, pthread_create,
             THREAD_LIBS="-lpthread",
             CC='cc -pthread'
             THREAD_LIBS="-L/usr/local/lib -pthread"
             THREAD_CFLAGS="-I/usr/local/include -D_THREAD_SAFE"
             CFLAGS="$CFLAGS -L/usr/local/lib $THREAD_CFLAGS"
             CPPFLAGS="$CPPFLAGS -I/usr/local/include -L/usr/local/lib"
             AC_DEFINE(HAVE_THREADS)
             AC_MSG_RESULT("                               ")
             AC_MSG_RESULT("                               ")
             AC_MSG_RESULT("WARNING: Assuming host is a BSD variant")
             AC_MSG_RESULT("WARNING: Using -L/usr/local/lib -pthread")
             AC_MSG_RESULT("                               ")
             AC_MSG_RESULT("                               ")
             AC_MSG_RESULT("REMINDER: Use gmake to compile!")
             AC_MSG_RESULT("                               ")
             AC_MSG_RESULT("                               ")
)
fi

CFLAGS="$COMMON_FLAGS $CFLAGS"
CPPFLAGS="$COMMON_FLAGS $CPPFLAGS"

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(THREAD_CFLAGS)
AC_SUBST(THREAD_LIBS)

dnl
dnl Checks for library functions.
dnl
AC_CHECK_FUNCS(gethostbyname socket inet_aton inet_addr \
               inet_pton poll select strerror)

AC_OUTPUT([
           Makefile
           src/Makefile
          ])
