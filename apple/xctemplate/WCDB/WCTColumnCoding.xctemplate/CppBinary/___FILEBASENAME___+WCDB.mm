//
//  ___FILENAME___
//  ___PROJECTNAME___
//
//  Created by ___FULLUSERNAME___ on ___DATE___.
//___COPYRIGHT___
//

#import "___FILEBASENAME___+WCTColumnAccessor.h"

template <>
WCTColumnRuntimeCppAccessor<___FILEBASENAMEASIDENTIFIER___>::CType WCTColumnRuntimeCppAccessor<___FILEBASENAMEASIDENTIFIER___>::convertPropertyTypeToCType(___FILEBASENAMEASIDENTIFIER___ value, WCTColumnRuntimeCppAccessor<___FILEBASENAMEASIDENTIFIER___>::SizeType &size)
// const void* convertPropertyTypeToCType(___FILEBASENAMEASIDENTIFIER___ value)
{
    size = <#data size #>;
    return (const void *) <#C value #>;
}

template <>
___FILEBASENAMEASIDENTIFIER___ WCTColumnRuntimeCppAccessor<___FILEBASENAMEASIDENTIFIER___>::convertCTypeToPropertyType(WCTColumnRuntimeCppAccessor<___FILEBASENAMEASIDENTIFIER___>::CType cvalue, WCTColumnRuntimeCppAccessor<___FILEBASENAMEASIDENTIFIER___>::SizeType size)
// ___FILEBASENAMEASIDENTIFIER___ convertCTypeToPropertyType(const void* cvalue, int size)
{
    return (___FILEBASENAMEASIDENTIFIER___)<#value #>;
}
