#ifndef _SQLITE_COMPACT_H_
#define _SQLITE_COMPACT_H_

/*
** NDEBUG and SQLITE_DEBUG are opposites.  It should always be true that
** defined(NDEBUG)==!defined(SQLITE_DEBUG).  If this is not currently true,
** make it true by defining or undefining NDEBUG.
**
** Setting NDEBUG makes the code smaller and faster by disabling the
** assert() statements in the code.  So we want the default action
** to be for NDEBUG to be set and NDEBUG to be undefined only if SQLITE_DEBUG
** is set.  Thus NDEBUG becomes an opt-in rather than an opt-out
** feature.
*/
#if !defined(NDEBUG) && !defined(SQLITE_DEBUG)
# define NDEBUG 1
#endif
#if defined(NDEBUG) && defined(SQLITE_DEBUG)
# undef NDEBUG
#endif

#include <sqlite3.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/*
** When SQLITE_OMIT_WSD is defined, it means that the target platform does
** not support Writable Static Data (WSD) such as global and static variables.
** All variables must either be on the stack or dynamically allocated from
** the heap.  When WSD is unsupported, the variable declarations scattered
** throughout the SQLite code must become constants instead.  The SQLITE_WSD
** macro is used for this purpose.  And instead of referencing the variable
** directly, we use its constant as a key to lookup the run-time allocated
** buffer that holds real variable.  The constant is also the initializer
** for the run-time allocated buffer.
**
** In the usual case where WSD is supported, the SQLITE_WSD and GLOBAL
** macros become no-ops and have zero performance impact.
*/
#ifdef SQLITE_OMIT_WSD
  #define SQLITE_WSD const
  #define GLOBAL(t,v) (*(t*)sqlite3_wsd_find((void*)&(v), sizeof(v)))
  #define sqlite3GlobalConfig GLOBAL(struct Sqlite3Config, sqlite3Config)
  int sqlite3_wsd_init(int N, int J);
  void *sqlite3_wsd_find(void *K, int L);
#else
  #define SQLITE_WSD
  #define GLOBAL(t,v) v
  #define sqlite3GlobalConfig sqlite3Config
#endif

/*
** The TESTONLY macro is used to enclose variable declarations or
** other bits of code that are needed to support the arguments
** within testcase() and assert() macros.
*/
#if !defined(NDEBUG) || defined(SQLITE_COVERAGE_TEST)
# define TESTONLY(X)  X
#else
# define TESTONLY(X)
#endif

/*
** A macro to hint to the compiler that a function should not be
** inlined.
*/
#if defined(__GNUC__)
#  define SQLITE_NOINLINE  __attribute__((noinline))
#elif defined(_MSC_VER) && _MSC_VER>=1310
#  define SQLITE_NOINLINE  __declspec(noinline)
#else
#  define SQLITE_NOINLINE
#endif

/*
** Round up a number to the next larger multiple of 8.  This is used
** to force 8-byte alignment on 64-bit architectures.
*/
#define ROUND8(x)     (((x)+7)&~7)

/*
** Round down to the nearest multiple of 8
*/
#define ROUNDDOWN8(x) ((x)&~7)

/*
** The following macros are used to suppress compiler warnings and to
** make it clear to human readers when a function parameter is deliberately
** left unused within the body of a function. This usually happens when
** a function is called via a function pointer. For example the
** implementation of an SQL aggregate step callback may not use the
** parameter indicating the number of arguments passed to the aggregate,
** if it knows that this is enforced elsewhere.
**
** When a function parameter is not used at all within the body of a function,
** it is generally named "NotUsed" or "NotUsed2" to make things even clearer.
** However, these macros may also be used to suppress warnings related to
** parameters that may or may not be used depending on compilation options.
** For example those parameters only used in assert() statements. In these
** cases the parameters are named as per the usual conventions.
*/
#define UNUSED_PARAMETER(x) (void)(x)
#define UNUSED_PARAMETER2(x,y) UNUSED_PARAMETER(x),UNUSED_PARAMETER(y)


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int64_t i64;
typedef uintptr_t uptr;

/*
** The SQLITE_WITHIN(P,S,E) macro checks to see if pointer P points to
** something between S (inclusive) and E (exclusive).
**
** In other words, S is a buffer and E is a pointer to the first byte after
** the end of buffer S.  This macro returns true if P points to something
** contained within the buffer S.
*/
#define SQLITE_WITHIN(P,S,E) (((uptr)(P)>=(uptr)(S))&&((uptr)(P)<(uptr)(E)))

extern SQLITE_WSD struct Sqlite3Config sqlite3Config;

/*
** Structure containing global configuration data for the SQLite library.
**
** This structure also contains some state information.
*/
struct Sqlite3Config {
  int bMemstat;                     /* True to enable memory status */
  int bCoreMutex;                   /* True to enable core mutexing */
  int bFullMutex;                   /* True to enable full mutexing */
  int bOpenUri;                     /* True to interpret filenames as URIs */
  int bUseCis;                      /* Use covering indices for full-scans */
  int bSmallMalloc;                 /* Avoid large memory allocations if true */
  int mxStrlen;                     /* Maximum string length */
  int neverCorrupt;                 /* Database is always well-formed */
  int szLookaside;                  /* Default lookaside buffer size */
  int nLookaside;                   /* Default lookaside buffer count */
  int nStmtSpill;                   /* Stmt-journal spill-to-disk threshold */
  sqlite3_mem_methods m;            /* Low-level memory allocation interface */
  sqlite3_mutex_methods mutex;      /* Low-level mutex interface */
  sqlite3_pcache_methods2 pcache2;  /* Low-level page-cache interface */
  void *pHeap;                      /* Heap storage space */
  int nHeap;                        /* Size of pHeap[] */
  int mnReq, mxReq;                 /* Min and max heap requests sizes */
  sqlite3_int64 szMmap;             /* mmap() space per open file */
  sqlite3_int64 mxMmap;             /* Maximum value for szMmap */
  void *pPage;                      /* Page cache memory */
  int szPage;                       /* Size of each page in pPage[] */
  int nPage;                        /* Number of pages in pPage[] */
  int mxParserStack;                /* maximum depth of the parser stack */
  int sharedCacheEnabled;           /* true if shared-cache mode enabled */
  u32 szPma;                        /* Maximum Sorter PMA size */
  /* The above might be initialized to non-zero.  The following need to always
  ** initially be zero, however. */
  int isInit;                       /* True after initialization has finished */
  int inProgress;                   /* True while initialization in progress */
  int isMutexInit;                  /* True after mutexes are initialized */
  int isMallocInit;                 /* True after malloc is initialized */
  int isPCacheInit;                 /* True after malloc is initialized */
  int nRefInitMutex;                /* Number of users of pInitMutex */
  sqlite3_mutex *pInitMutex;        /* Mutex used by sqlite3_initialize() */
  void (*xLog)(void*,int,const char*); /* Function for logging */
  void *pLogArg;                       /* First argument to xLog() */
#ifdef SQLITE_ENABLE_SQLLOG
  void(*xSqllog)(void*,sqlite3*,const char*, int);
  void *pSqllogArg;
#endif
#ifdef SQLITE_VDBE_COVERAGE
  /* The following callback (if not NULL) is invoked on every VDBE branch
  ** operation.  Set the callback using SQLITE_TESTCTRL_VDBE_COVERAGE.
  */
  void (*xVdbeBranch)(void*,unsigned iSrcLine,u8 eThis,u8 eMx);  /* Callback */
  void *pVdbeBranchArg;                                     /* 1st argument */
#endif
#ifdef SQLITE_ENABLE_DESERIALIZE
  sqlite3_int64 mxMemdbSize;        /* Default max memdb size */
#endif
#ifndef SQLITE_UNTESTABLE
  int (*xTestCallback)(int);        /* Invoked by sqlite3FaultSim() */
#endif
  int bLocaltimeFault;              /* True to fail localtime() calls */
  int bInternalFunctions;           /* Internal SQL functions are visible */
  int iOnceResetThreshold;          /* When to reset OP_Once counters */
  u32 szSorterRef;                  /* Min size in bytes to use sorter-refs */
};

int sqlite3HeapNearlyFull(void);

void sqlite3StatusUp(int, int);
void sqlite3StatusDown(int, int);
void sqlite3StatusHighwater(int, int);

#ifndef SQLITE_MUTEX_OMIT
  sqlite3_mutex_methods const *sqlite3DefaultMutex(void);
  sqlite3_mutex_methods const *sqlite3NoopMutex(void);
  sqlite3_mutex *sqlite3MutexAlloc(int);
  int sqlite3MutexInit(void);
  int sqlite3MutexEnd(void);
#endif

#endif //_SQLITE_COMPACT_H_
