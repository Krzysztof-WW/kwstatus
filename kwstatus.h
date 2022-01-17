#ifndef KWSTATUS_H
#define KWSTATUS_H

#include <stdio.h>
#include <pthread.h>

#define LENGTH(x) (sizeof(x)/sizeof(x[0]))

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

#endif /* KWSTATUS_H */

