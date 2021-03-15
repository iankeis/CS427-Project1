.SUFFIXES:.c.o
CC = gcc
CFLAGS = -g #-std=c99 -Wall -pedantic
EXEC = wsu_crypt
OBJS = wsu_crypt.o #main.o

all: wsu_crypt.o #main.o
	${CC} -o ${EXEC} ${OBJS}

${EXEC}: ${OBJS}
	        ${CC} ${CFLAGS} -o ${EXEC} ${OBJS}
%.c%.o:
	        ${CC} ${CFLAGS} -c $<

# run: clean cleanscr ${EXEC}
# 	        ./${EXEC}

run: ${EXEC}
	    ./${EXEC}

clean:
	        rm -f ${EXEC} ${OBJS}
cleanscr:
	        clear

valgrind: clean ${EXEC}
		valgrind -s ./${EXEC}

gdb: clean ${EXEC}
		gdb ${EXEC}

wsu_crypt.o: wsu_crypt.c #assignment7.h
#main.o: main.c assignment7.h
