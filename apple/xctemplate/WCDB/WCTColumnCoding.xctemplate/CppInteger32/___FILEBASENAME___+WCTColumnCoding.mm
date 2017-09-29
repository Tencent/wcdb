//___FILEHEADER___

#import "___FILEBASENAME___+WCTColumnAccessor.h"

template <>
WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::CType WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::convertPropertyTypeToCType(___VARIABLE_productName___ t)
// int32_t convertPropertyTypeToCType(___VARIABLE_productName___ value)
{
    return (int32_t) /* <#C value#> */;
}

template <>
___VARIABLE_productName___ WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::convertCTypeToPropertyType(WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::CType cvalue)
// ___VARIABLE_productName___ convertCTypeToPropertyType(int32_t cvalue)
{
    return (___VARIABLE_productName___) /* <#value#> */;
}
