//___FILEHEADER___

#import <WCDB/WCDB.h>

template<>
struct ColumnIsBLOBType<___VARIABLE_productName___> : public std::true_type {
};

template<>
___VARIABLE_productName___
WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::convertCTypeToPropertyType(
WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::CType cvalue,
WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::SizeType size);

template<>
WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::CType
WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::convertPropertyTypeToCType(
___VARIABLE_productName___ value,
WCTColumnRuntimeCppAccessor<___VARIABLE_productName___>::SizeType &size);
