#include "kwstatus.h"
#include <stdarg.h>
#include <stdlib.h>

/* safe memory allocation functions for modules */
void*
emalloc(size_t size) {
  void* ret;
  ret = malloc(size);
  if(ret == NULL) {
    fputs("malloc: out of memory\n", stderr);
    pthread_exit(NULL);
  }
  return ret;
}

void*
ecalloc(size_t nmemb, size_t size) {
  void* ret;
  ret = calloc(nmemb, size);
  if(ret == NULL) {
    warn("calloc: out of memory");
    pthread_exit(NULL);
  }
  return ret;
}

/* taken from dwmstatus, for use in modules */
char *
smprintf(const char *fmt, ...)
{
	va_list fmtargs;
	char *ret;
	int len;

	va_start(fmtargs, fmt);
	len = vsnprintf(NULL, 0, fmt, fmtargs);
	va_end(fmtargs);

	ret = emalloc(++len);

	va_start(fmtargs, fmt);
	vsnprintf(ret, len, fmt, fmtargs);
	va_end(fmtargs);

	return ret;
}

