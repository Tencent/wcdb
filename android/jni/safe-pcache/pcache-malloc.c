
#include "pcache-malloc.h"
#include "malloc.h"
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>

#ifdef __ANDROID__
#include <sys/prctl.h>
#endif

#define ROUND8(x)     (((x)+7)&~7)

void* mmap_impl(size_t sz) {
    size_t pageSize = getpagesize();
    uint8_t *base = (uint8_t *) mmap(NULL, sz + pageSize * 2, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, 
            -1, 0);
    if (base == MAP_FAILED) goto failure;

    if (mprotect(base + pageSize, sz, PROT_READ | PROT_WRITE) != 0)
        goto failure;
    
#ifdef __ANDROID__
    prctl(0x53564d41, 0, (uintptr_t) base, sz + pageSize * 2, (uintptr_t) "SQLite Safe PCache Guard");
    prctl(0x53564d41, 0, (uintptr_t) base + pageSize, sz, (uintptr_t) "SQLite Safe PCache");
#endif

    return base + pageSize;

failure:
    if (base != MAP_FAILED)
        munmap(base, sz + pageSize * 2);
    return MAP_FAILED;
}

int munmap_impl(void *addr, size_t sz) {
    size_t pageSize = getpagesize();
    return munmap((uint8_t *) addr - pageSize, sz + pageSize * 2);
}

static mspace pcache_msp;

void pcache_meminit() {
    pcache_msp = create_mspace(0, 1);
}

void pcache_memdeinit() {
    destroy_mspace(pcache_msp);
}

void *pcache_malloc(size_t sz) {
    size_t padding = ROUND8(sizeof(size_t));
    uint8_t *ptr = (uint8_t *) mspace_malloc(pcache_msp, sz + padding);
    *(size_t *) ptr = sz + padding;
    return ptr + padding;
}

void *pcache_malloc_zero(size_t sz) {
    size_t padding = ROUND8(sizeof(size_t));
    uint8_t *ptr = (uint8_t *) mspace_calloc(pcache_msp, sz + padding, 1);
    *(size_t *) ptr = sz + padding;
    return ptr + padding;
}

void pcache_free(void *p) {
    if (!p) return;
    size_t padding = ROUND8(sizeof(size_t));
    mspace_free(pcache_msp, (uint8_t *)p - padding);
}

int pcache_memsize(void *p) {
    if (!p) return 0;
    size_t padding = ROUND8(sizeof(size_t));
    return (int) *(size_t *)((uint8_t *)p - padding);
}
