//
//  ___FILENAME___
//  ___PROJECTNAME___
//
//  Created by ___FULLUSERNAME___ on ___DATE___.
//___COPYRIGHT___
//

#import <WCDB/WCDB.h>

template <>
struct ColumnIsInteger64Type<___FILEBASENAMEASIDENTIFIER___> : public std::true_type {};

template <>
WCTColumnRuntimeCppAccessor<___FILEBASENAMEASIDENTIFIER___>::CType WCTColumnRuntimeCppAccessor<___FILEBASENAMEASIDENTIFIER___>::convertPropertyTypeToCType(___FILEBASENAMEASIDENTIFIER___);
 
template <>
___FILEBASENAMEASIDENTIFIER___ WCTColumnRuntimeCppAccessor<___FILEBASENAMEASIDENTIFIER___>::convertCTypeToPropertyType(WCTColumnRuntimeCppAccessor<___FILEBASENAMEASIDENTIFIER___>::CType);

