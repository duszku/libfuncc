CC=gcc

C_FLAGS=-Wall -g
LIBS=-lasan -lubsan -lfunc -lcriterion

BINDIR=bin

TEST_S=$(wildcard *.c)
TEST_B=$(patsubst %.c, ${BINDIR}/%, ${TEST_S})

.PHONY: all

all: clean ${TEST_B}

${BINDIR}:
	mkdir $@

${BINDIR}/%: %.c ${BINDIR}
	${CC} ${C_FLAGS} -o$@ $< ${LIBS}

clean:
	rm -f ${BINDIR}/*
