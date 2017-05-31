//
//  ___FILENAME___
//  ___PROJECTNAME___
//
//  Created by ___FULLUSERNAME___ on ___DATE___.
//___COPYRIGHT___
//

#import <WCDB/WCDB.h>

template <>
struct ColumnIsBLOBType<___FILEBASENAMEASIDENTIFIER___>
    : public std::true_type {
};

template <>
___FILEBASENAMEASIDENTIFIER___
WCTColumnRuntimeCppAccessor<___FILEBASENAMEASIDENTIFIER___>::
    convertCTypeToPropertyType(
        WCTColumnRuntimeCppAccessor<___FILEBASENAMEASIDENTIFIER___>::CType
            cvalue,
        WCTColumnRuntimeCppAccessor<___FILEBASENAMEASIDENTIFIER___>::SizeType
            size);

template <>
WCTColumnRuntimeCppAccessor<___FILEBASENAMEASIDENTIFIER___>::CType
WCTColumnRuntimeCppAccessor<___FILEBASENAMEASIDENTIFIER___>::
    convertPropertyTypeToCType(
        ___FILEBASENAMEASIDENTIFIER___ value,
        WCTColumnRuntimeCppAccessor<___FILEBASENAMEASIDENTIFIER___>::SizeType
            &size);
