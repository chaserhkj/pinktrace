IF_RUBY_FILES= \
	       trace.c \
	       event.c \
	       bitness.c \
	       syscall.c \
	       string.c \
	       strarray.c \
	       socket.c \
	       init.c
IF_RUBY_TESTS= \
	       TEST_trace.rb \
	       TEST_bitness.rb \
	       TEST_syscall_freebsd.rb \
	       TEST_string_freebsd.rb \
	       TEST_strarray_freebsd.rb \
	       TEST_socket_address_freebsd.rb \
	       TEST_event.rb \
	       TEST_syscall_linux.rb \
	       TEST_string_linux.rb \
	       TEST_strarray_linux.rb \
	       TEST_socket_linux.rb \
	       TEST_socket_address_linux.rb
EXTRA_DIST= $(IF_RUBY_FILES) $(IF_RUBY_TESTS)
CLEANFILES= TEST_UNIX_SOCKET

dist_noinst_HEADERS= globals.h
dist_noinst_SCRIPTS= ruby-test.sh

if ENABLE_RUBY
AM_CFLAGS= \
	   -I. -I@RUBY_HDRDIR@ -I@RUBY_HDRARCHDIR@ \
	   -I$(top_builddir)/include \
	   -I$(top_srcdir)/include \
	   @PINKTRACE_CFLAGS@ @PINKTRACE_RUBY_CFLAGS@

rubylibdir= @RUBY_INSTALL_DIR@
rubylib_LTLIBRARIES= PinkTrace.la

PinkTrace_la_SOURCES= $(IF_RUBY_FILES)
PinkTrace_la_LDFLAGS= -module -avoid-version -shared
PinkTrace_la_LIBADD= $(top_builddir)/src/libpinktrace_@PINKTRACE_PC_SLOT@.la

TESTS_ENVIRONMENT= RUBY="$(RUBY)" $(srcdir)/ruby-test.sh
TESTS= \
       TEST_trace.rb \
       TEST_bitness.rb

if FREEBSD
TESTS+= \
	TEST_syscall_freebsd.rb \
	TEST_string_freebsd.rb \
	TEST_strarray_freebsd.rb \
	TEST_socket_address_freebsd.rb
endif # FREEBSD

if LINUX
TESTS+= \
	TEST_event.rb \
	TEST_syscall_linux.rb \
	TEST_string_linux.rb \
	TEST_strarray_linux.rb \
	TEST_socket_linux.rb \
	TEST_socket_address_linux.rb
endif # LINUX

# Remove the .la file - PinkTrace.la is never linked against (it's a module)
# and Ruby doesn't use libltdl. Note that the library gets installed by
# install-data, so that's where we need to hook.
# XXX: This breaks distcheck!
#install-data-hook:
#	rm -f $(DESTDIR)$(rubylibdir)/PinkTrace.la
endif # ENABLE_RUBY
