CC=gcc

C_FLAGS=-Wall -g
L_FLAGS=-I/usr/local/include -L/usr/local/lib
LIBS=-lasan -lubsan -lfuncc -lcriterion

BINDIR=bin

TEST_S=$(wildcard *.c)
TEST_B=$(patsubst %.c, ${BINDIR}/%, ${TEST_S})

.PHONY: all

all: clean ${TEST_B} run

${BINDIR}:
	mkdir $@

${BINDIR}/%: %.c ${BINDIR}
	${CC} ${C_FLAGS} ${L_FLAGS} -o$@ $< ${LIBS}

run: ${TEST_B}
	@echo
	@echo Running tests...
	@for f in ${TEST_B} ; do ./$$f ; done

clean:
	rm -f ${BINDIR}/*
