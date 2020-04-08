.PHONY: default
default: all

include ../rules.mk

# Build program flags
CFLAGS 	+= -std=c99 -O2 -Wall -Wextra -pedantic -Iinclude -fPIC
LDFLAGS += -shared -Wl,-O1 -fPIC

# Target base name
LIBNAME = iuab

# Targets to build
TARGET_STATIC    = lib$(LIBNAME)$(STATICEXT)
TARGET_SHARED    = lib$(LIBNAME)$(SHAREDEXT)
TARGET_PKGCONFIG = lib$(LIBNAME).pc

.PHONY: all
all: $(builddir)/$(TARGET_STATIC) $(builddir)/$(TARGET_SHARED) \
	$(builddir)/$(TARGET_PKGCONFIG)

$(builddir)/$(TARGET_STATIC): $(objs)
	@echo "  AR      $(call basetarget,$@)"
	$(Q)$(AR) $(ARFLAGS) $@ $(objs)

$(builddir)/$(TARGET_SHARED): $(objs)
	@echo "  LD      $(call basetarget,$@)"
	$(Q)$(LD) $(LDFLAGS) -o $@ $(objs)

$(builddir)/$(TARGET_PKGCONFIG): $(TARGET_PKGCONFIG).in
	@echo "  GEN     $(call basetarget,$@)"
	$(Q)sed $< \
		-e 's#@includedir@#$(includedir)#g' \
		-e 's#@libdir@#$(libdir)#g' \
		> $@

.PHONY: install
install: install_static install_shared install_headers install_pkgconfig

.PHONY: install_static
install_static: $(builddir)/$(TARGET_STATIC)
	@echo "  INSTALL $(libdir)/$(TARGET_STATIC)"
	$(Q)$(INSTALL_DATA) $< $(libdir)/$(TARGET_STATIC)

.PHONY: install_shared
install_shared: $(builddir)/$(TARGET_SHARED)
	@echo "  INSTALL $(libdir)/$(TARGET_SHARED)"
	$(Q)$(INSTALL_PROGRAM) $< $(libdir)/$(TARGET_SHARED)

.PHONY: install_headers
install_headers:
	@echo "  CP      $(includedir)/$(LIBNAME)"
	@mkdir -p $(includedir)
	$(Q)cp -r include/$(LIBNAME) $(includedir)

.PHONY: install_pkgconfig
install_pkgconfig: $(builddir)/$(TARGET_PKGCONFIG)
	@echo "  INSTALL $(libdir)/pkgconfig/$(TARGET_PKGCONFIG)"
	$(Q)$(INSTALL_DATA) $< $(libdir)/pkgconfig/$(TARGET_PKGCONFIG)

.PHONY: uninstall
uninstall: uninstall_static uninstall_shared uninstall_headers \
	uninstall_pkgconfig

.PHONY: uninstall_static
uninstall_static:
	@echo "  RM      $(libdir)/$(TARGET_STATIC)"
	$(Q)$(RM) $(libdir)/$(TARGET_STATIC)

.PHONY: uninstall_shared
uninstall_shared:
	@echo "  RM      $(libdir)/$(TARGET_SHARED)"
	$(Q)$(RM) $(libdir)/$(TARGET_SHARED)

.PHONY: uninstall_headers
uninstall_headers:
	@echo "  RM      $(includedir)/$(LIBNAME)"
	$(Q)$(RM) -r $(includedir)/$(LIBNAME)

.PHONY: uninstall_pkgconfig
uninstall_pkgconfig:
	@echo "  RM      $(libdir)/pkgconfig/$(TARGET_PKGCONFIG)"
	$(Q)$(RM) $(libdir)/pkgconfig/$(TARGET_PKGCONFIG)
