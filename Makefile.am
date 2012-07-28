SUBDIRS= src doc pkg-config
ACLOCAL_AMFLAGS= -I m4
AUTOMAKE_OPTIONS= foreign dist-bzip2 no-dist-gzip std-options

CLEANFILES= *~
MAINTAINERCLEANFILES= \
		      Makefile.in \
		      configure \
		      aclocal.m4 \
		      config.h \
		      config.h.in \
		      INSTALL
EXTRA_DIST= \
	    autogen.sh \
	    COPYRIGHT \
	    misc/generated-file.txt \
	    misc/site-commit-template.txt

DISTCHECK_CONFIGURE_FLAGS= \
			   --enable-ipv6 \
			   --enable-doxygen

doc_DATA= \
	  README.markdown \
	  NEWS.markdown \
	  TODO.markdown
EXTRA_DIST+= $(doc_DATA)

pinktrace_includedir=$(includedir)/pinktrace-$(PINKTRACE_PC_SLOT)/pinktrace/
pinktrace_include_HEADERS= \
			  include/pinktrace/about.h \
			  include/pinktrace/system.h \
			  include/pinktrace/abi.h \
			  include/pinktrace/compat.h \
			  include/pinktrace/compiler.h \
			  include/pinktrace/event.h \
			  include/pinktrace/read.h \
			  include/pinktrace/regs.h \
			  include/pinktrace/socket.h \
			  include/pinktrace/syscall.h \
			  include/pinktrace/trace.h \
			  include/pinktrace/write.h \
			  include/pinktrace/pink.h

EXTRA_DIST+= \
	     include/pinktrace/about.h.in \
	     include/pinktrace/system.h.in
MAINTAINERCLEANFILES+= \
		       include/pinktrace/about.h \
		       include/pinktrace/system.h
noinst_HEADERS= \
		include/pinktrace/internal.h

pinktrace_easy_DIST= \
		     include/pinktrace/easy/attach.h \
		     include/pinktrace/easy/call.h \
		     include/pinktrace/easy/callback.h \
		     include/pinktrace/easy/context.h \
		     include/pinktrace/easy/error.h \
		     include/pinktrace/easy/exec.h \
		     include/pinktrace/easy/func.h \
		     include/pinktrace/easy/init.h \
		     include/pinktrace/easy/loop.h \
		     include/pinktrace/easy/process.h \
		     include/pinktrace/easy/pink.h
EXTRA_DIST+= \
	     $(pinktrace_easy_DIST) \
	     include/pinktrace/easy/internal.h
if WANT_EASY
pinktrace_easy_includedir=$(includedir)/pinktrace-$(PINKTRACE_PC_SLOT)/pinktrace/easy
pinktrace_easy_include_HEADERS= $(pinktrace_easy_DIST)
endif # WANT_EASY

.PHONY: doxygen
doxygen: all
	$(MAKE) -C doc $@

.PHONY: site
site: doxygen
	$(MAKE) -C doc $@
	$(MAKE) -C examples $@

.PHONY: site
site-check: site
	$(MAKE) -C doc $@

.PHONY: checksum
checksum: dist
	sha1sum $(PACKAGE)-$(VERSION).tar.bz2 > $(PACKAGE)-$(VERSION).tar.bz2.sha1sum

.PHONY: sign
sign: dist
	gpg --detach-sign --armor $(PACKAGE)-$(VERSION).tar.bz2

.PHONY: release
release: site dist checksum sign
	if ! test -d $(SITE_INSTALL_DIR)/release; then echo "No SITE_INSTALL_DIR/release"; exit 1; fi
	$(INSTALL) \
		-m 644 -p \
		-t $(SITE_INSTALL_DIR)/release \
		$(PACKAGE)-$(VERSION).tar.bz2 \
		$(PACKAGE)-$(VERSION).tar.bz2.sha1sum \
		$(PACKAGE)-$(VERSION).tar.bz2.asc

.PHONY: git-release
git-release: release
	git --git-dir=$(SITE_INSTALL_DIR) add --update
	$(SED) \
		-e 's/@VERSION@/$(VERSION)/' \
		$(top_srcdir)/misc/site-commit-template.txt |\
		git --git-dir=$(SITE_INSTALL_DIR) commit -F - -m
