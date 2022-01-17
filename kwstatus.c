#include "kwstatus.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <X11/Xlib.h>

/* global variables */
pthread_cond_t cupdate;
pthread_mutex_t mupdate;

static void
die(char* str) {
  fputs(str, stderr);
  exit(1);
}

void*
emalloc(size_t size) {
  void* ret;
  ret = malloc(size);
  if(ret == NULL)
    die("malloc failed");
  return ret;
}

void mod_update(struct modules* self, const char* out) {
  pthread_mutex_lock(&self->mut);
  strncpy(self->out, out, MODSIZE);
  pthread_mutex_unlock(&self->mut);

  pthread_mutex_lock(&mupdate);
  pthread_cond_signal(&cupdate);
  pthread_mutex_unlock(&mupdate);
}

static void
init_modules(pthread_t* thr, size_t len) {
  size_t n;

  for(n=0; n<len; n++) {
    pthread_mutex_init(&mdl[n].mut, NULL);
    mdl[n].out = emalloc(MODSIZE);
    memset(mdl[n].out, 0, MODSIZE);
    pthread_create(&thr[n], NULL, (void*)mdl[n].fun, &mdl[n]);
  }
}
int
main(int argc, char* argv[]) {
  size_t mlen = LENGTH(mdl);
  size_t n, outpos;
  short dry = 0;
  char out[BARSIZE] = {0};
  pthread_t thr[mlen];
  Display* dpy;
  const struct timespec update_delay = {0, align_ms*1000000};

  /* check args */
  if(argc >= 2 && !strcmp(argv[1], "-d"))
      dry = 1;

  /* initialization */
  pthread_cond_init(&cupdate, NULL);
  pthread_mutex_init(&mupdate, NULL);
  init_modules(thr, mlen);
	if(!dry && !(dpy = XOpenDisplay(NULL)))
    die("Cannot open display");

  pthread_mutex_lock(&mupdate);
  /* main loop */
  while(1) {
    /* wait until next event */
    pthread_cond_wait(&cupdate, &mupdate);
    pthread_mutex_unlock(&mupdate);
    /* wait to align parallel modules with similar timings to avoid unnecessary updates */
    nanosleep(&update_delay, NULL);

    pthread_mutex_lock(&mupdate);
    /* read all modules */
    outpos = 0;
    for(n = 0; n < mlen; n++) {
      pthread_mutex_lock(&mdl[n].mut);
        strncpy(out+outpos, mdl[n].out, BARSIZE-outpos);
        outpos += strlen(mdl[n].out);
      pthread_mutex_unlock(&mdl[n].mut);
    }

    /* update bar */
    if(dry)
      puts(out);
    else {
      XStoreName(dpy, DefaultRootWindow(dpy), out);
      XSync(dpy, False);
    }
  }

	return 0;
}

