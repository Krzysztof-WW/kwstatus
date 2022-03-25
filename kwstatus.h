/* Copyright (C) 2022 Krzysztof Wolski
 * See COPYING for license details */

#ifndef KWSTATUS_H
#define KWSTATUS_H

#include <pthread.h>

#define LENGTH(x) (sizeof(x)/sizeof(x[0]))

#define MODSIZE 100 /* module max length */

struct modules {
  void (*fun)(void*);
  
  /* options of module */
  const char* str;
  const long num;
  const short no_delim;
  const char* icon;

  char* out;
  pthread_mutex_t mut;
};

/* modules routines */
void* emalloc(size_t size);
void* ecalloc(size_t nmemb, size_t size);
char* smprintf(const char *fmt, ...);
void mod_update(struct modules* self, const char* str);

#endif /* KWSTATUS_H */

