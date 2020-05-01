.PHONY: default
default: all

# Linker flags for libraries
LIBS = -L$(builddir) -liuab

# Build program flags
ALL_CFLAGS  += -std=gnu99 -Wall -Wextra -pedantic -I../lib/include
ALL_LDFLAGS += -Wl,--as-needed $(LIBS)

include ../rules.mk

# Target base name
CMDNAME = i-use-arch-btw

# Targets to build
TARGET = $(CMDNAME)$(EXEEXT)

.PHONY: all
all: $(builddir)/$(TARGET)

$(builddir)/$(TARGET): $(objs)
	@echo "  LD      $(call basetarget,$@)"
	$(Q)$(LD) -o $@ $(objs) $(ALL_LDFLAGS)

.PHONY: install
install: $(builddir)/$(TARGET)
	@echo "  INSTALL $(bindir)/$(TARGET)"
	$(Q)$(INSTALL) $< $(bindir)/$(TARGET)

.PHONY: uninstall
uninstall:
	@echo "  RM      $(bindir)/$(TARGET)"
	$(Q)$(RM) $(bindir)/$(TARGET)
