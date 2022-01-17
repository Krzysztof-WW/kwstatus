#include "kwstatus.h"
#include "config.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 500

/* global variables */
pthread_cond_t cupdate;
pthread_mutex_t mupdate;

void
die(char* str) {
  fputs(str, stderr);
  exit(1);
}

void*
emalloc(size_t size) {
  void* ret;
  ret = malloc(size);
  if(ret == NULL)
    die("emalloc failed");
  return ret;
}

void
init_modules(pthread_t* thr, size_t len) {
  size_t n;

  for(n=0; n<len; n++) {
    pthread_mutex_init(&mdl[n].mut, NULL);
    pthread_create(&thr[n], NULL, (void*)mdl[n].fun, &mdl[n]);
  }
}

int
main(int argc, char* argv[]) {
  size_t mlen = LENGTH(mdl);
  size_t n, outpos;
  char out[BUFSIZE] = {0};
  pthread_t thr[mlen];

  /* initialization */
  pthread_cond_init(&cupdate, NULL);
  pthread_mutex_init(&mupdate, NULL);
  init_modules(thr, mlen);

  /* main loop */
  {
    pthread_mutex_lock(&mupdate);
    pthread_cond_wait(&cupdate, &mupdate);

    for(n = 0; n < mlen; n++) {
      pthread_mutex_lock(&mdl[n].mut);
      puts("aa");
      outpos = strlen(out);
      strncpy(out+outpos, mdl[n].out, BUFSIZE-outpos);
      pthread_mutex_unlock(&mdl[n].mut);
    }

    puts(out);

    pthread_mutex_unlock(&mupdate);
  }

	return 0;
}

