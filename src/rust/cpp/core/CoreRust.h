#pragma once
#include "WCDBRust.h"

//#define WCDBRustCoreFuncName(funcName) WCDBRust(Core, funcName)
//#define WCDBRustCoreObjectMethod(funcName, ...)                                 \
//    WCDBRustObjectMethod(Core, funcName, __VA_ARGS__)
//#define WCDBRustCoreObjectMethodWithNoArg(funcName)                             \
//    WCDBRustObjectMethodWithNoArg(Core, funcName)
//#define WCDBRustCoreClassMethodWithNoArg(funcName)                              \
//    WCDBRustClassMethodWithNoArg(Core, funcName)
#define WCDBRustCoreClassMethod(funcName, ...)                                  \
    WCDBRustClassMethod(Core, funcName, __VA_ARGS__)

WCDB_EXTERN void* WCDBRustCoreClassMethod(createDatabase, const char* path);
//void WCDBRustCoreClassMethod(setDefaultCipherConfig, jint version);
//void WCDBRustCoreClassMethodWithNoArg(purgeAllDatabase);
//void WCDBRustCoreClassMethod(releaseSQLiteMemory, jint bytes);
//void WCDBRustCoreClassMethod(setSoftHeapLimit, jlong limit);
//void WCDBRustCoreClassMethod(setAutoCheckpointMinFrames, jint frames);
//jlong WCDBRustCoreClassMethodWithNoArg(getThreadedError);