#include "../kwstatus.h"
#include <stdio.h>
#include <time.h>

#define CLOCK_BUF 20

static char* cf[] = {
  "🕛",
  "🕐",
  "🕑",
  "🕒",
  "🕓",
  "🕔",
  "🕕",
  "🕖",
  "🕗",
  "🕘",
  "🕙",
  "🕚",
  "🕛",
};

void
clockm(void* self) {
  struct modules* mod = (struct modules*)self;

  struct timespec wait = {mod->num, 0};
  time_t now;
	struct tm *tm;
  char out[CLOCK_BUF] = {0};

  while(1) {
    now = time(NULL);
    tm = localtime(&now);
    snprintf(out, CLOCK_BUF, "%s %02d:%02d:%02d", 
        cf[tm->tm_hour > 12 ? tm->tm_hour - 12 : tm->tm_hour], 
        tm->tm_hour, tm->tm_min, tm->tm_sec);
    mod_update(mod, out);

    nanosleep(&wait, NULL);
  }
}
