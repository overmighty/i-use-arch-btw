.PHONY: default
default: all

include ../rules.mk

# Build program flags
CFLAGS 	+= -std=c99 -O2 -Wall -Wextra -pedantic -Iinclude -fPIC
LDFLAGS += -shared -Wl,-O1 -fPIC

# Target base name
LIBNAME = iuab

# Targets to build
TARGET_STATIC = lib$(LIBNAME)$(STATICEXT)
TARGET_SHARED = lib$(LIBNAME)$(SHAREDEXT)

.PHONY: all
all: $(builddir)/$(TARGET_STATIC) $(builddir)/$(TARGET_SHARED)

$(builddir)/$(TARGET_STATIC): $(objs)
	@echo "  AR      $(call basetarget,$@)"
	$(Q)$(AR) $(ARFLAGS) $@ $(objs)

$(builddir)/$(TARGET_SHARED): $(objs)
	@echo "  LD      $(call basetarget,$@)"
	$(Q)$(LD) $(LDFLAGS) -o $@ $(objs)

.PHONY: install
install: install_static install_shared install_headers

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

.PHONY: uninstall
uninstall: uninstall_static uninstall_shared uninstall_headers

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
