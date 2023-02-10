CC=gcc

TARGET=DEBUG

C_FLAGS_DEBUG=-ansi -Wall -Wextra -Werror -Og -g -fpic \
	      -fsanitize=address,undefined
C_FLAGS_RELEASE=-Wall -O2 -fpic

L_FLAGS_DEBUG=-shared
L_FLAGS_RELEASE=-shared

LIBS_DEBUG=-lasan -lubsan -lc
LIBS_RELEASE=-lc

SRC=flist.c
OBJ=${SRC:.c=.o}

.PHONY: all libfunc.so clean

all: libfunc.so clean

libfunc.so: ${OBJ}
	${CC} ${L_FLAGS_${TARGET}} -o$@ ${OBJ} ${LIBS_${TARGET}}

.c.o:
	${CC} ${C_FLAGS_${TARGET}} -c -o$@ $<

clean:
	rm -f *.o
