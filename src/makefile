CC=gcc
CFLAGS=-O2 -Wall -Werror
LDFLAGS=-lpthread -lm

lib = libdlib.a

heads = dlib_array.h \
				dlib_comm.h \
				dlib_hash.h \

srcs = dlib_array.c \
			 dlib_comm.c \
			 dlib_hash.c \

objs = $(subst .c,.o,$(srcs))

all: libdlib.a $(heads)

$(lib): $(objs)
	ar -r -cusv $@ $^

clean:
	rm -f $(objs) $(lib) test

test: test.c $(lib)

.PHONY: run
run:
	./test array
	./test hash 1000000 10
