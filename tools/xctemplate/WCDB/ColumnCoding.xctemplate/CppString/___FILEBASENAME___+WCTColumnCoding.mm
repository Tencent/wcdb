//___FILEHEADER___

#import "___FILEBASENAME___+WCTColumnAccessor.h"

template <>
WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::CType WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::convertPropertyTypeToCType(___VARIABLE_productName___ value)
// const char* convertPropertyTypeToCType(___VARIABLE_productName___ value)
{
    return (const char *) /* <#C value#> */;
}

template <>
___VARIABLE_productName___ WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::convertCTypeToPropertyType(WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::CType cvalue)
// ___VARIABLE_productName___ convertCTypeToPropertyType(const char* cvalue)
{
    return (___VARIABLE_productName___) /* <#value#> */;
}
