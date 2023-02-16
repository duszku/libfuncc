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

LIB=libfuncc.so
INCLUDES=include

.PHONY: all ${LIB} clean

all: ${LIB} clean

install: uninstall all
	mv ${LIB} /usr/local/lib/${LIB}
	mkdir -p /usr/local/include/funcc
	cp ${INCLUDES}/* /usr/local/include/funcc

uninstall:
	rm -f /usr/local/lib/${LIB}
	rm -rf /usr/local/include/funcc

${LIB}: ${OBJ}
	${CC} ${L_FLAGS_${TARGET}} -o$@ ${OBJ} ${LIBS_${TARGET}}

.c.o:
	${CC} ${C_FLAGS_${TARGET}} -c -o$@ $<

clean:
	rm -f *.o
