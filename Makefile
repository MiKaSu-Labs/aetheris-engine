# Aetheris - build for the C foundation library and test suite.
#
# The recipes use a POSIX shell. On Windows, run make from an MSYS2
# environment (C:\msys64\usr\bin\make.exe) or ensure sh.exe is on PATH;
# GNUWin32 make falls back to cmd.exe and will fail on these recipes.
#
# Targets:
#   all     - build build/libaetheris.a
#   test    - build and run all tests under tests/
#   clean   - remove the build directory

CC      ?= gcc
AR      ?= ar
CFLAGS  ?= -std=c11 -Wall -Wextra -Wpedantic -Wshadow \
           -Wconversion -Wsign-conversion -Wundef -g -O2
CPPFLAGS += -Iinclude -Itests
LDFLAGS ?=

BUILD    := build
BUILDDIR := $(BUILD)/obj
LIB      := $(BUILD)/libaetheris.a

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:src/%.c=$(BUILDDIR)/%.o)

TESTS_SRC := $(wildcard tests/test_*.c)
TESTS_BIN := $(TESTS_SRC:tests/%.c=$(BUILD)/%)

.PHONY: all test clean

all: $(LIB)

$(LIB): $(OBJS)
	$(AR) rcs $@ $^

$(BUILDDIR)/%.o: src/%.c | $(BUILDDIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/%: tests/%.c $(LIB) tests/ae_test.c | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< tests/ae_test.c $(LIB) $(LDFLAGS) -o $@

test: $(TESTS_BIN)
	@for t in $(TESTS_BIN); do \
		echo "== $$t =="; \
		$$t || exit 1; \
	done

$(BUILD):
	mkdir -p $(BUILD)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILD)