SUBDIRS= api .
WWW_FILES= \
	   _layouts/default.html \
	   _includes/footer.html \
	   _includes/menu.html \
	   _scripts/examples.rb \
	   css/syntax.css \
	   css/screen.css \
	   images/header.png \
	   images/favicon.png
EXTRA_DIST= $(WWW_FILES)

if ENABLE_SITE_COPY
site: $(WWW_FILES)
	$(RSYNC) -av --filter '- Makefile*' --filter '- *.in' --filter '- api' --filter '- _scripts' --filter '- _site' . $(SITE_INSTALL_DIR)/
	$(MAKE) -C api $@
site-check:
	@echo Run jekyll in $(SITE_INSTALL_DIR)
	exit 0
endif

if ENABLE_SITE_GEN
site: $(WWW_FILES)
	$(JEKYLL) --pygments $(SITE_INSTALL_DIR)

site-check: $(WWW_FILES)
	$(JEKYLL) --pygments --server $(SITE_INSTALL_DIR)
endif

doxygen:
	$(MAKE) -C api $@

.PHONY: site site-check doxygen
