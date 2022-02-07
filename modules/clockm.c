/*  clock module

    Display time and date with clock icon formatted with syntax from strftime(3).
    To get clock icon displayed you can use "%i" sequence or customize it with CLOCK_SEQ.

    Configuration:
      .str - date format
      .num - refresh time
      CLOCK_SEQ macro - sequence for clock icon in format
*/

#define CLOCK_SEQ "%i"
/* end of configuration */

#include "../kwstatus.h"
#include <stdio.h>
#include <string.h>
#include <time.h>


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
	struct tm tm;
  char out[MODSIZE] = {0};

  size_t icon_len = strlen(cf[0]);
  char fmt[strlen(mod->str)-LENGTH(CLOCK_SEQ)+icon_len];
  char* clock_pos;

  /* find position of clock icon */
  clock_pos = strstr(mod->str, CLOCK_SEQ);
  if(clock_pos) {
    strncpy(fmt, mod->str, clock_pos - mod->str);
    strcpy(fmt + (clock_pos-mod->str) + icon_len, clock_pos + LENGTH(CLOCK_SEQ));
    clock_pos = fmt + (clock_pos-mod->str);
  }

  while(1) {
    /* get time */
    now = time(NULL);
    localtime_r(&now, &tm);

    /* insert clock icon */
    if(clock_pos)
      strncpy(clock_pos, cf[tm.tm_hour > 12 ? tm.tm_hour - 12 : tm.tm_hour], 4);

    /* format and update */
    strftime(out, MODSIZE, fmt, &tm);
    mod_update(mod, out);

    nanosleep(&wait, NULL);
  }
}
