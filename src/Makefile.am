CPPCFLAGS = @CPPFLAGS@ @THREAD_CFLAGS@

AM_CPPFLAGS = -I/usr/include -I./ -I /usr/local/include -I/usr/src/sys

lib_LTLIBRARIES = libnetclass.la

libnetclass_la_SOURCES = ncsocket.cpp   \
                         ncthread.cpp   \
                         ncmutex.cpp    \
                         ncsocklist.cpp \
                         netclass.cpp

include_HEADERS = ncarch.h \
                  ncconsts.h \
                  ncdebug.h \
                  ncenums.h \
                  ncmutex.h \
                  ncsocket.h \
                  ncsocklist.h \
                  ncthread.h \
                  netclass.h \
                  udpdata.h

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS) $(SHELL) ./config.status --recheck

libnetclass_la_LIBADD = -lstdc++ @THREAD_LIBS@


