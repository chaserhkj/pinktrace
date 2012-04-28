SUBDIRS= .

CLEANFILES=
AM_CFLAGS= \
	   -I$(top_srcdir)/include \
	   @PINKTRACE_CFLAGS@

easy_SRCS= \
	   pink-easy-attach.c \
	   pink-easy-call.c \
	   pink-easy-context.c \
	   pink-easy-exec.c \
	   pink-easy-error.c \
	   pink-easy-internal.c \
	   pink-easy-loop.c \
	   pink-easy-process.c
EXTRA_DIST= $(easy_SRCS)

if WANT_EASY
lib_LTLIBRARIES= libpinktrace_easy_@PINKTRACE_PC_SLOT@.la
libpinktrace_easy_@PINKTRACE_PC_SLOT@_la_SOURCES= $(easy_SRCS)
libpinktrace_easy_@PINKTRACE_PC_SLOT@_la_LDFLAGS= \
						  -export-symbols-regex '^pink_' \
						  -version-info @VERSION_LIB_CURRENT@:@VERSION_LIB_REVISION@:0
libpinktrace_easy_@PINKTRACE_PC_SLOT@_la_LIBADD= $(top_builddir)/src/libpinktrace_@PINKTRACE_PC_SLOT@.la
endif # WANT_EASY
