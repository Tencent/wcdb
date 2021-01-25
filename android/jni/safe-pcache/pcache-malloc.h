#ifndef __PCACHE_MALLOC_H__
#define __PCACHE_MALLOC_H__

#include <stddef.h>

void pcache_meminit();
void pcache_memdeinit();
void *pcache_malloc(size_t sz);
void *pcache_malloc_zero(size_t sz);
void pcache_free(void *p);
int pcache_memsize(void *p);

#endif
