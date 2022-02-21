PREFIX = /usr/local
CFLAGS = -O3 -Wall -Wno-unused-result -g
LDFLAGS = -g
LIBS = ${MODULES_LIBS} -pthread -lxcb
CC = gcc

MODULES_LIBS = -lpulse -lm -lmpdclient
MODULES = modules/print\
					modules/clockm\
					modules/backlight\
					modules/battery\
					modules/pulse\
					modules/mpd\
					modules/run_command

all: kwstatus

config.h:
	cp config.def.h $@

kwstatus.o: modules.h config.h kwstatus.h

kwstatus: kwstatus.o util.o ${MODULES:=.o}
	${CC} -o $@ kwstatus.o util.o ${MODULES:=.o} ${LDFLAGS} ${LIBS}

.c.o:
	${CC} ${CFLAGS} -c $< -o $@

install: kwstatus
	mkdir -p ${PREFIX}/bin
	cp -f kwstatus ${PREFIX}/bin/kwstatus

clean:
	rm -f *.o modules/*.o kwstatus

