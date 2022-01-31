CFLAGS = -O0 -Wall -g
LDFLAGS = -g
LIBS = ${MODULES_LIBS} -pthread -lX11
CC = gcc

MODULES_LIBS = -lpulse
MODULES = modules/test\
					modules/print\
					modules/clockm\
					modules/backlight\
					modules/battery\
					modules/pulse

all: kwstatus

kwstatus.o: modules.h config.h kwstatus.h

kwstatus: kwstatus.o ${MODULES:=.o}
	${CC} -o $@ kwstatus.o ${MODULES:=.o} ${LDFLAGS} ${LIBS}

.c.o:
	${CC} ${CFLAGS} -c $< -o $@

clean:
	rm -f *.o modules/*.o kwstatus

