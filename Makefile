.POSIX:
CC       = cc
CFLAGS   = -std=c99 -Wall -Wextra -Wno-switch -pedantic
LDFLAGS  =
DEBUG    = 0
BUILDDIR = build
DESTDIR  =
PREFIX   = /usr/local
BINDIR   = /bin
OBJECTS  = src/main.o src/parser.o src/safety.o src/util.o src/vm.o

ifeq ($(DEBUG), 1)
	CFLAGS += -O0 -g
else
	CFLAGS += -O2
endif

all: $(BUILDDIR) $(BUILDDIR)/i-use-arch-btw

$(BUILDDIR):
	mkdir -p $@

$(BUILDDIR)/i-use-arch-btw: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS)

install: $(BUILDDIR)/i-use-arch-btw
	cp $< $(DESTDIR)$(PREFIX)$(BINDIR)/i-use-arch-btw

uninstall:
	rm -f $(DESTDIR)$(PREFIX)$(BINDIR)/i-use-arch-btw

clean:
	rm -rf $(BUILDDIR) src/*.o

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

src/main.o: src/main.c src/parser.h src/opcode.h src/util.h src/vm.h
src/parser.o: src/parser.c src/parser.h src/opcode.h src/util.h
src/safety.o: src/safety.c src/safety.h src/vm.h src/opcode.h src/util.h
src/util.o: src/util.c src/util.h
src/vm.o: src/vm.c src/safety.h src/vm.h src/opcode.h src/util.h
