use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::column::Column;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use std::ffi::{c_char, c_void};

extern "C" {
    fn WCDBRustCommonTableExpression_createWithTable(table_name: *const c_char) -> *mut c_void;

    fn WCDBRustCommonTableExpression_configColumn(self_obj: *mut c_void, column: *mut c_void);

    fn WCDBRustCommonTableExpression_configSelect(self_obj: *mut c_void, select: *mut c_void);
}

pub struct CommonTableExpression {
    identifier: Identifier,
}

impl CppObjectTrait for CommonTableExpression {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.identifier.set_cpp_obj(cpp_obj);
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.identifier.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.identifier.release_cpp_object();
    }
}

impl CppObjectConvertibleTrait for CommonTableExpression {
    fn as_cpp_object(&self) -> &CppObject {
        self.identifier.as_cpp_object()
    }
}

impl IdentifierTrait for CommonTableExpression {
    fn get_type(&self) -> CPPType {
        self.identifier.get_type()
    }

    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierConvertibleTrait for CommonTableExpression {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

impl CommonTableExpression {
    pub fn new(table_name: &str) -> Self {
        let c_table_name = table_name.to_cstring();
        let cpp_obj =
            unsafe { WCDBRustCommonTableExpression_createWithTable(c_table_name.as_ptr()) };
        Self {
            identifier: Identifier::new(CPPType::CommonTableExpression, Some(cpp_obj)),
        }
    }

    pub fn column(mut self, column: Column) -> Self {
        unsafe {
            WCDBRustCommonTableExpression_configColumn(
                self.identifier.get_cpp_obj(),
                column.get_cpp_obj(),
            );
        }
        self
    }
}
