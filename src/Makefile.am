lib_LTLIBRARIES = libdlib.la
include_HEADERS = dlib_comm.h

libdlib_la_SOURCES = $(include_HEADERS) \
										 dlib_comm.c

noinst_PROGRAMS = test
test_SOURCES = test.c
test_CFLAGS = -L. -ldlib

.PHONY: run
run:
	./test loadfile test.c
	valgrind .libs/lt-test loadfile test.c
	./test owner
	./test echo e c h o
