MAKEFLAGS += -rR --no-print-directory

SHELL = /bin/sh

# Build programs
CC = cc
LD = $(CC)
AR = ar

# Build program flags
CFLAGS  = -O2
LDFLAGS = -Wl,-O1
ARFLAGS = rcs

# Debugging flags
DEBUG_CFLAGS = -g -Og

ALL_CFLAGS  = $(CFLAGS)
ALL_LDFLAGS = $(LDFLAGS)

ifeq ($(DEBUG), 1)
	ALL_CFLAGS += $(DEBUG_CFLAGS)
endif

# Install commands
INSTALL         = install -D
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA    = $(INSTALL) -m 644

# Misc. commands
RM = rm -f

# Executable and library extensions
EXEEXT    =
STATICEXT = .a
SHAREDEXT = .so

# Project directories
SRCDIR   = src
BUILDDIR = build
builddir := $(CURDIR)/$(BUILDDIR)

# Install directories
DESTDIR    =
PREFIX     = /usr/local
BINDIR     = /bin
INCLUDEDIR = /include
LIBDIR     = /lib
destdir    = $(realpath $(DESTDIR))
prefix     = $(destdir)$(PREFIX)
bindir     = $(prefix)$(BINDIR)
includedir = $(prefix)$(INCLUDEDIR)
libdir     = $(prefix)$(LIBDIR)

# Silence commands by default
Q = @
# Do not silence commands if make is called with V=1
ifeq ($(V), 1)
	Q =
endif

export

MODULES = cmd lib
unexport MODULES

.PHONY: all install uninstall
all install uninstall: cmd

.PHONY: $(MODULES)
$(MODULES):
	@echo -e "\033[1m  MAKE    $@\033[0m"
	$(Q)$(MAKE) -C $@ $(MAKECMDGOALS)

cmd: lib

.PHONY: clean
clean:
	@echo "  CLEAN   $(BUILDDIR)"
	$(Q)$(RM) -r $(BUILDDIR)
