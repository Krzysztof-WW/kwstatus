/* Copyright (C) 2022 Krzysztof Wolski
 * See COPYING for license details */

/*  ram module
    
    This module shows percent of used memory, just like htop would show.

    Configuration:
      REFRESH_TIME - see below
*/
#define REFRESH_TIME 2
/* end of configuration */

#include "../kwstatus.h"
#include <stdio.h>
#include <unistd.h>

void
ram(void* self) {
  struct modules* mod = (struct modules*)self;
  char out[MODSIZE] = {0};
  FILE* meminfo;
  unsigned int total, free, buffer, cached;

  meminfo = fopen("/proc/meminfo", "r");
  if(meminfo == NULL)
    return;

  while(1) {
    fseek(meminfo, 0, SEEK_SET);
    fscanf(meminfo, "MemTotal: %u kB\n"
        "MemFree: %u kB\n"
        "MemAvailable: %*u kB\n"
        "Buffers: %u kB\n"
        "Cached: %u kB\n", &total, &free, &buffer, &cached);

    snprintf(out, MODSIZE, "%.0f%%", 100-100*(double)(free+buffer+cached)/total);
    mod_update(mod, out);

    sleep(REFRESH_TIME);
  }
}
