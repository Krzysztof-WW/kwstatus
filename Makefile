include config.mk

cf_pulse_1 = `pkg-config --cflags libpulse`
lib_pulse_1 = `pkg-config --libs libpulse`
file_pulse_1 = modules/pulse

cf_mpd_1 = `pkg-config --cflags libmpdclient`
lib_mpd_1 = `pkg-config --libs libmpdclient`
file_mpd_1 = modules/mpd

LIBS = ${MODULES_LIBS} -pthread -lxcb

CPPFLAGS = ${cf_pulse_${ENABLE_PULSEAUDIO}}\
					 ${cf_mpd_${ENABLE_MPD}}

MODULES_LIBS = -lm \
							 ${lib_pulse_${ENABLE_PULSEAUDIO}}\
							 ${lib_mpd_${ENABLE_MPD}}

MODULES = modules/print\
					modules/clockm\
					modules/backlight\
					modules/battery\
					${file_pulse_${ENABLE_PULSEAUDIO}}\
					${file_mpd_${ENABLE_MPD}}\
					modules/run_command\
					modules/disk\
					modules/interface\
					modules/ram

all: kwstatus

config.h:
	cp config.def.h $@

kwstatus.o: modules.h config.h kwstatus.h

kwstatus: kwstatus.o util.o ${MODULES:=.o}
	${CC} -o $@ kwstatus.o util.o ${MODULES:=.o} ${LDFLAGS} ${LIBS}

.c.o:
	${CC} ${CFLAGS} ${CPPFLAGS} -c $< -o $@

install: kwstatus
	mkdir -p ${PREFIX}/bin
	cp -f kwstatus ${PREFIX}/bin/kwstatus

clean:
	rm -f *.o modules/*.o kwstatus

