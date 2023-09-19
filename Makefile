LUA_INCDIR=/usr/include/lua5.3
LUA_BIN=/usr/bin/lua5.3
LIBDIR=/usr/local/lib/lua/5.3
CFLAGS=-Wall -Wextra -Wpointer-arith -Werror -fPIC -O3 -D_REENTRANT -D_GNU_SOURCE
LDFLAGS=-shared -fPIC

export LUA_CPATH=$(PWD)/?.so

default: all

all: h3.so

h3.so: h3.o
	gcc $(LDFLAGS) -o h3.so h3.o -lh3

h3.o: src/h3.h src/h3.c
	gcc -c -o h3.o $(CFLAGS) -I$(LUA_INCDIR) src/h3.c

.PHONY: test
test:
	$(LUA_BIN) test/test.lua

install:
	cp h3.so $(LIBDIR)

clean:
	-rm -f h3.o h3.so
