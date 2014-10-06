lib_LTLIBRARIES = libdlib.la
include_HEADERS = dlib_comm.h \
									dlib_array.h \
									dlib_hash.h

libdlib_la_SOURCES = $(include_HEADERS) \
										 dlib_comm.c \
										 dlib_array.c \
										 dlib_hash.c

noinst_PROGRAMS = test
test_SOURCES = test.c
test_CFLAGS = -L. -ldlib

.PHONY: run
run:
	./test hash 1000000 10
	./test utils
	./test array
	./test loadfile test.c
	./test owner
	./test echo e c h o
