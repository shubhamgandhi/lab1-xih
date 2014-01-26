# CS 111 Lab 1 Makefile

CC = gcc
CFLAGS = -g -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -Werror
LAB = 1
DISTDIR = lab1-$(USER)

all: timetrash

TESTS = $(wildcard test*.sh)
TEST_BASES = $(subst .sh,,$(TESTS))

TIMETRASH_SOURCES = \
  alloc.c \
  execute-command.c \
  main.c \
  read-command.c \
  print-command.c \
  stack.c \
  binary_expression_tree_generator.c \
  infix_expression_generator.c \
  postfix_expression_generator.c \
  infix_error_checker.c \
  checked_sys_calls.c \

TIMETRASH_OBJECTS = $(subst .c,.o,$(TIMETRASH_SOURCES))

DIST_SOURCES = \
  $(TIMETRASH_SOURCES) alloc.h command.h command-internals.h Makefile \
  binary_expression_tree_generator.h \
  infix_expression_generator.h \
  postfix_expression_generator.h \
  infix_error_checker.h \
  stack.h \
  structs.h \
  checked_sys_calls.h \
  $(TESTS) check-dist README

timetrash: $(TIMETRASH_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(TIMETRASH_OBJECTS)

alloc.o: alloc.h
execute-command.o main.o print-command.o read-command.o: command.h
execute-command.o print-command.o read-command.o: command-internals.h
read-command.o binary_expression_tree_generator.o \
infix_expression_generator.o postfix_expression_generator.o infix_error_checker.o: \
structs.h


dist: $(DISTDIR).tar.gz

$(DISTDIR).tar.gz: $(DIST_SOURCES) check-dist
	rm -fr $(DISTDIR)
	tar -czf $@.tmp --transform='s,^,$(DISTDIR)/,' $(DIST_SOURCES)
	./check-dist $(DISTDIR)
	mv $@.tmp $@

check: $(TEST_BASES)

$(TEST_BASES): timetrash
	./$@.sh

clean:
	rm -fr *.o *~ *.bak *.tar.gz core *.core *.tmp timetrash $(DISTDIR)

.PHONY: all dist check $(TEST_BASES) clean
