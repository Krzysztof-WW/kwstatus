#include "../kwstatus.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void
run_command(void* self) {
  struct modules* mod = (struct modules*)self;
  FILE* fp;
  int len;
  char out[MODSIZE];

  while(1) {
    fp = popen(mod->str, "r");
    if(fp == NULL) {
      fputs("run_command: cannot execute cmd\n", stderr);
      return;
    }

    fgets(out, MODSIZE, fp);
    pclose(fp);

    len = strlen(out);
    out[len-1] = 0;
    mod_update(mod, out);

    sleep(mod->num);
  }
}
