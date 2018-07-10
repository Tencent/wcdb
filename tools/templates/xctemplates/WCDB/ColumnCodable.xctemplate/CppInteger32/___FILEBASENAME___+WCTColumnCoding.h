//___FILEHEADER___

#import <WCDB/WCDB.h>

template<>
struct ColumnIsInteger32Type<___VARIABLE_productName___> : public std::true_type {
};

template<>
WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::CType
WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::convertPropertyTypeToCType(___VARIABLE_productName___ t);

template<>
___VARIABLE_productName___
WCTColumnRuntimeCppAccessor<___VARIABLE_productName___ *>::convertCTypeToPropertyType(
WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::CType t);
