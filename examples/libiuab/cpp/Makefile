CXX = g++

CXXFLAGS := -Wall -Wextra -pedantic $(shell pkg-config libiuab --cflags)
LDFLAGS  := $(shell pkg-config libiuab --libs)

BUILDDIR = build

all: $(BUILDDIR)/example

$(BUILDDIR)/example: example.cpp
	mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -rf $(BUILDDIR)
