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

# Compiler toolchain. GNU make's built-in default is `cc`, but MSYS2 ships
# gcc.exe (no `cc` driver), so pin the exact binaries the recipes rely on.
CC := gcc
AR := ar
CFLAGS  ?= -std=c11 -Wall -Wextra -Wpedantic -Wshadow \
           -Wconversion -Wsign-conversion -Wundef -g -O2
CPPFLAGS += -Iinclude -Itests -Ithird_party/cJSON
LDFLAGS ?=

# The crypto module draws CSPRNG bytes from BCrypt on Windows and from
# getrandom() elsewhere. Link the system library only where needed.
ifeq ($(OS),Windows_NT)
LDFLAGS += -lbcrypt
endif

BUILD    := build
BUILDDIR := $(BUILD)/obj
LIB      := $(BUILD)/libaetheris.a

SRCS := $(wildcard src/*.c third_party/cJSON/cJSON.c)
OBJS := $(patsubst %.c,$(BUILDDIR)/%.o,$(notdir $(SRCS)))
VPATH := src third_party/cJSON

TESTS_SRC := $(wildcard tests/test_*.c)
TESTS_BIN := $(TESTS_SRC:tests/%.c=$(BUILD)/%)

.PHONY: all test clean

all: $(LIB)

$(LIB): $(OBJS)
	$(AR) rcs $@ $^

$(BUILDDIR)/%.o: %.c | $(BUILDDIR)
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
