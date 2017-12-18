//___FILEHEADER___

#import "___FILEBASENAME___+WCTColumnAccessor.h"

template <>
WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::CType WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::convertPropertyTypeToCType(___VARIABLE_productName___ value, WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::SizeType &size)
// const void* convertPropertyTypeToCType(___VARIABLE_productName___ value)
{
    size = /* <#data size#> */;
    return (const void *) /* /* <#C value#> */ * / ;
}

template <>
___VARIABLE_productName___ WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::convertCTypeToPropertyType(WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::CType cvalue, WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::SizeType size)
// ___VARIABLE_productName___ convertCTypeToPropertyType(const void* cvalue, int size)
{
    return (___VARIABLE_productName___) /* /* <#value#> */ * / ;
}
