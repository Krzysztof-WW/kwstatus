#include "../kwstatus.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

void test(void* self) {
  struct modules* mod = (struct modules*)self;
  int wait = mod->num;

  while(1) {
    mod_update(mod, mod->str);
    sleep(wait);
  }
}
