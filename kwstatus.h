#ifndef KWSTATUS_H
#define KWSTATUS_H

#include <pthread.h>

#define LENGTH(x) (sizeof(x)/sizeof(x[0]))

#define MODSIZE 100 /* module max length */
#define BARSIZE MODSIZE*LENGTH(mdl) /* max bar length */

struct modules {
  void (*fun)(void*);
  const long num;
  const char* str;
  const short no_delim;

  char out[MODSIZE];
  pthread_mutex_t mut;
};

/* modules routines */
void* emalloc(size_t size);
void* ecalloc(size_t nmemb, size_t size);
char* smprintf(const char *fmt, ...);
void mod_update(struct modules* self, const char* str);

#endif /* KWSTATUS_H */

