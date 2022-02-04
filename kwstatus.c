#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>

#include "kwstatus.h"
#include "config.h"

/* global variables */
static pthread_cond_t cupdate;
static pthread_mutex_t mupdate;

void
mod_update(struct modules* self, const char* out) {
  /* copy text to module */
  pthread_mutex_lock(&self->mut);
  strncpy(self->out, out, MODSIZE-1);
  if(!self->no_delim && self->out[0]) /* add delimiter at the end */
    strncat(self->out, delim, MODSIZE-1);
  pthread_mutex_unlock(&self->mut);

  /* send signal to update status bar */
  pthread_mutex_lock(&mupdate);
  pthread_cond_signal(&cupdate);
  pthread_mutex_unlock(&mupdate);
}

static void
init_modules(pthread_t* thr, size_t len) {
  size_t n;

  /* initialize objects for module and lunch it */
  for(n=0; n<len; n++) {
    pthread_mutex_init(&mdl[n].mut, NULL);
    pthread_create(&thr[n], NULL, (void*)mdl[n].fun, &mdl[n]);
  }
}

int
main(int argc, char* argv[]) {
  size_t mlen = LENGTH(mdl);
  size_t n;
  char out[BARSIZE] = {0};
  pthread_t thr[mlen];
  xcb_connection_t *c;
  xcb_window_t root;
  short dry = 0;
  const struct timespec update_delay = {0, align_ms*1000000};

  /* check args */
  if(argc >= 2) {
    if(!strcmp(argv[1], "-d"))
      dry = 1;
    else {
      fprintf(stderr, "usage: %s [-d]\n", argv[0]);
      exit(1);
    }
  }

  /* initialization */
  pthread_cond_init(&cupdate, NULL);
  pthread_mutex_init(&mupdate, NULL);
  init_modules(thr, mlen);
  if(!dry) {
    c = xcb_connect(NULL, NULL);
    if(!c) {
      fputs("Cannot connect to X server\n", stderr);
      exit(1);
    }
    root = xcb_setup_roots_iterator(xcb_get_setup(c)).data->root;
  }

  pthread_mutex_lock(&mupdate);
  /* main loop */
  while(1) {
    /* wait until next event */
    pthread_cond_wait(&cupdate, &mupdate);

    /* wait for modules with similar timings to avoid unnecessary updates */
    pthread_mutex_unlock(&mupdate);
    nanosleep(&update_delay, NULL);
    pthread_mutex_lock(&mupdate);

    /* read all modules */
    out[0] = 0;
    for(n = 0; n < mlen; n++) {
      pthread_mutex_lock(&mdl[n].mut);
        strncat(out, mdl[n].out, BARSIZE-1);
      pthread_mutex_unlock(&mdl[n].mut);
    }

    /* update bar */
    if(dry)
      puts(out);
    else {
      xcb_change_property(c, XCB_PROP_MODE_REPLACE, root, XCB_ATOM_WM_NAME,
          XCB_ATOM_STRING, 8, strlen(out), out);
      xcb_flush(c);
    }
  }

	return 0;
}

