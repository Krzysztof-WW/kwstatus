/* Copyright (C) 2022 Krzysztof Wolski
 * See COPYING for license details */

/*  disk module

    Display occupied percentage of disk.

    Configuration:
      .str - mount point to get fs size
      .num - percentage level after which module is displayed
      REFRESH_TIME - time between updates
*/

#define REFRESH_TIME 10
/* end of configuration */

#include "../kwstatus.h"
#include <sys/vfs.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

void
disk(void* self) {
  struct modules* mod = (struct modules*)self;
  struct statfs stats;
  unsigned char proc;
  char out[MODSIZE];

  while(1) {
    if(!statfs(mod->str, &stats)) {
      proc = roundf(100 - (float)stats.f_bfree / stats.f_blocks * 100);

      if(proc >= mod->num)
        snprintf(out, MODSIZE, "%d%%", proc);
      else
        out[0] = 0;
      mod_update(mod, out);
    } else
      mod_update(mod, "");

    sleep(REFRESH_TIME);
  }
}
