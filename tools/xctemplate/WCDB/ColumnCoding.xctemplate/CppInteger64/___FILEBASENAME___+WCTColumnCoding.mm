//___FILEHEADER___

#import "___FILEBASENAME___+WCTColumnAccessor.h"

template <>
WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::CType WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::convertPropertyTypeToCType(___VARIABLE_productName___ value)
// int64_t convertPropertyTypeToCType(___VARIABLE_productName___ value)
{
    return (int64_t) /* <#C value#> */;
}

template <>
___VARIABLE_productName___ WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::convertCTypeToPropertyType(WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::CType cvalue)
// ___VARIABLE_productName___ convertCTypeToPropertyType(int64_t cvalue)
{
    return (___VARIABLE_productName___) /* <#value#> */;
}
