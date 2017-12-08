//___FILEHEADER___

#import "___FILEBASENAME___+WCTColumnAccessor.h"

template <>
WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::CType WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::convertPropertyTypeToCType(___VARIABLE_productName___ value)
// double convertPropertyTypeToCType(___VARIABLE_productName___ value)
{
    return (double) /* <#C value#> */;
}

template <>
___VARIABLE_productName___ WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::convertCTypeToPropertyType(WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::CType cvalue)
// ___VARIABLE_productName___ convertCTypeToPropertyType(double cvalue)
{
    return (___VARIABLE_productName___) /* <#value#> */;
}
