#include "CoreRust.h"
#include "CoreBridge.h"

void* WCDBRustCoreClassMethod(createDatabase, const char* path)
{
    void* ret = (void*) WCDBCoreCreateDatabase(path).innerValue;
    return ret;
}