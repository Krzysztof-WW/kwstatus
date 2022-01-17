#ifndef KWSTATUS_H
#define KWSTATUS_H

#include <stdio.h>
#include <pthread.h>

#define LENGTH(x) (sizeof(x)/sizeof(x[0]))

#define MODSIZE 100 /* out of module length */
#define BARSIZE 500 /* max bar length */

struct modules {
  void (*fun)(void*);
  const long num;
  const char* str;

  char* out;
  pthread_mutex_t mut;
};

extern pthread_cond_t cupdate;
extern pthread_mutex_t mupdate;

void* emalloc(size_t size);
void* ecalloc(size_t nmemb, size_t size);
void mod_update(struct modules* self, const char* str);

#endif /* KWSTATUS_H */

