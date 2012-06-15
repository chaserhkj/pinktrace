SUBDIRS= linux . easy

CLEANFILES=
EXTRA_DIST=

OS	= linux
# ARCH is `arm', `i386', 'x86_64', etc.
ARCH	= @arch@

AM_CFLAGS= @PINKTRACE_CFLAGS@ \
	   -I$(top_builddir)/include -I$(top_srcdir)/include \
	   -I$(srcdir)/$(OS)/$(ARCH) -I$(srcdir)/$(OS)

lib_LTLIBRARIES = libpinktrace_@PINKTRACE_PC_SLOT@.la

libpinktrace_@PINKTRACE_PC_SLOT@_la_SOURCES= trace.c \
					     abi.c \
					     event.c \
					     syscall.c \
					     read.c \
					     write.c \
					     socket.c
libpinktrace_@PINKTRACE_PC_SLOT@_la_LDFLAGS= \
					     -version-info @VERSION_LIB_CURRENT@:@VERSION_LIB_REVISION@:0 \
					     -export-symbols-regex '^pink_'

IF_CHECK_SRCS= \
	       trace-TEST.c \
	       read-TEST.c \
	       write-TEST.c \
	       socket-TEST.c \
	       pinktrace-check.c

noinst_HEADERS= pinktrace-check.h
EXTRA_DIST+= $(IF_CHECK_SRCS)

if HAVE_CHECK
CHECK_CFLAGS= \
	      -I$(top_builddir)/include \
	      -I$(top_srcdir)/include \
	      -L$(top_builddir)/src/.libs \
	      $(check_CFLAGS) \
	      @PINKTRACE_CFLAGS@
CHECK_LIBS= \
	    $(top_builddir)/src/libpinktrace_@PINKTRACE_PC_SLOT@.la \
	    $(check_LIBS)

check_PROGRAMS= pinktrace-check

pinktrace_check_SOURCES= $(IF_CHECK_SRCS)
pinktrace_check_CFLAGS= $(CHECK_CFLAGS)
pinktrace_check_LDADD= $(CHECK_LIBS)

TESTS= $(check_PROGRAMS)

if INSTALL_CHECKS
bin_PROGRAMS= pinktrace-check
endif # INSTALL_CHECKS

endif # HAVE_CHECK
SPARSE=sparse
SPARSE_CPPFLAGS= $(DEFAULT_INCLUDES) \
		 -I$(shell $(CC) -print-file-name=include) \
		 -I$(shell $(CC) -print-file-name=include-fixed)
# Fix this flag for your architecture!
SPARSE_CPPFLAGS+= -D__x86_64__=1
#SPARSE_CPPFLAGS+= -D__i386__=1
#SPARSE_CPPFLAGS+= -D__ia64__=1
#SPARSE_CPPFLAGS+= -D__powerpc__=1
#SPARSE_CPPFLAGS+= -D__powerpc64__=1
#SPARSE_CPPFLAGS+= -D__arm__=1

.PHONY: sparse-check
sparse-check:
	for src in $(libpinktrace_@PINKTRACE_PC_SLOT@_la_SOURCES); \
	do \
		$(SPARSE) $(DEFS) $(SPARSE_CPPFLAGS) $(AM_CFLAGS) $$src || exit 1; \
	done
