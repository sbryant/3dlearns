#-*- mode:makefile-gmake; -*-
#
# Copyright (c) Vicent Marti. All rights reserved.
#
# This file is part of clar, distributed under the ISC license.
# For full terms see the included COPYING file.
#

#
# Set up the path to the clar sources and to the fixtures directory
#
# The fixture path needs to be an absolute path so it can be used
# even after we have chdir'ed into the test directory while testing.
#
CURRENT_MAKEFILE  := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
TEST_DIRECTORY    := $(abspath $(dir $(CURRENT_MAKEFILE)))
CLAR_PATH         := $(TEST_DIRECTORY)/clar/
CLAR_FIXTURE_PATH := $(TEST_DIRECTORY)/fixtures/

CFLAGS=-g -std=c99 -I../src -I. -I$(CLAR_PATH)  -Wall -Werror -DCLAR_FIXTURE_PATH=\"$(CLAR_FIXTURE_PATH)\" -D_GNU_SOURCE

.PHONY: clean test

# App sources
LIB_DIR = $(abspath $(TEST_DIRECTORY)/../src)
export DYLD_LIBRARY_PATH = $(LIB_DIR)
export LD_LIBRARY_PATH = $(LIB_DIR)

SOURCE_DIR = $(abspath $(TEST_DIRECTORY)/../src)
mains = $(SOURCE_DIR)/mfgd.c
SOURCES = $(filter-out $(mains), $(wildcard $(SOURCE_DIR)/*.c))

# Tests
TESTS = $(wildcard $(TEST_DIRECTORY)/*.c)

# list the objects that go into our test
TOBJS = $(patsubst %.c,%.o,$(SOURCES))
objects = $(CLAR_PATH)main.o $(patsubst %.c,%.o,$(TESTS))

# build the test executable itself
clar_test: $(objects) clar.suite $(TEST_DIRECTORY)/clar_test.h  $(CLAR_PATH)clar.c
	make -C $(LIB_DIR)/.. all
	$(CC) $(CFLAGS) -L$(LIB_DIR) "$(CLAR_PATH)clar.c" $(objects) $(TOBJS) $(LIBS) -o $@

# test object files depend on clar macros
$(objects) : $(CLAR_PATH)clar.h

# build the clar.suite file of test metadata
clar.suite:
	python "$(CLAR_PATH)generate.py" $(TEST_DIRECTORY)/

# remove all generated files
clean:
	$(RM) -rf $(CLAR_PATH)*.o *.o clar.suite .clarcache clar_test clar_test.dSYM a.out a.out.dSYM

# run tests
test: clar_test
	./clar_test
