.PHONY: default
default: all

include ../rules.mk

# Build program flags
CFLAGS  += -std=gnu99 -O2 -Wall -Wextra -pedantic -I../lib/include
LDFLAGS += -Wl,-O1,--as-needed $(LIBS)

# Linker flags for libraries
LIBS = -L$(builddir) -liuab

# Target base name
CMDNAME = i-use-arch-btw

# Targets to build
TARGET = $(CMDNAME)$(EXEEXT)

.PHONY: all
all: $(builddir)/$(TARGET)

$(builddir)/$(TARGET): $(objs)
	@echo "  LD      $(call basetarget,$@)"
	$(Q)$(LD) -o $@ $(objs) $(LDFLAGS)

.PHONY: install
install: $(builddir)/$(TARGET)
	@echo "  INSTALL $(bindir)/$(TARGET)"
	$(Q)$(INSTALL) $< $(bindir)/$(TARGET)

.PHONY: uninstall
uninstall:
	@echo "  RM      $(bindir)/$(TARGET)"
	$(Q)$(RM) $(bindir)/$(TARGET)
