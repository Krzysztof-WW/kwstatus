#include "../kwstatus.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <string.h>

#include <stdlib.h>

void test_update(struct modules* self) {
  pthread_mutex_lock(&self->mut);
  strcpy(self->out, "abc");
  pthread_mutex_unlock(&self->mut);

  pthread_mutex_lock(&mupdate);
  pthread_cond_signal(&cupdate);
  pthread_mutex_unlock(&mupdate);
}

void test(void* self) {
  struct modules* mod = (struct modules*)self;
  
  pthread_mutex_lock(&mod->mut);

  mod->out = malloc(20);
  memset(mod->out, 0, 20);

  pthread_mutex_unlock(&mod->mut);

  test_update(mod);
}
