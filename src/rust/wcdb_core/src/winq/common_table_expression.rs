use crate::base::cpp_object::CppObjectTrait;
use crate::winq::column::Column;
use crate::winq::identifier::{CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait};
use std::ffi::{c_char, c_void, CString};

extern "C" {
    pub fn WCDBRustCommonTableExpression_createWithTable(table_name: *const c_char) -> *mut c_void;
    pub fn WCDBRustCommonTableExpression_configColumn(self_obj: i64, column: i64);
    pub fn WCDBRustCommonTableExpression_configSelect(self_obj: i64, select: i64);
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

impl IdentifierTrait for CommonTableExpression {
    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierStaticTrait for CommonTableExpression {
    fn get_type() -> i32 {
        CPPType::CommonTableExpression as i32
    }
}

impl CommonTableExpression {
    pub fn new(table_name: &str) -> Self {
        let c_table_name = CString::new(table_name).unwrap_or_default();
        let cpp_obj =
            unsafe { WCDBRustCommonTableExpression_createWithTable(c_table_name.as_ptr()) };
        Self {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn column(mut self, column: Column) -> Self {
        unsafe {
            WCDBRustCommonTableExpression_configColumn(
                self.identifier.get_cpp_obj() as i64,
                column.get_cpp_obj() as i64,
            );
        }
        self
    }

    // fn config_column(self_obj: i64, column: i64) {
    //     // 调用本地方法
    //     unimplemented!()
    // }
    //
    // pub fn as_select(&mut self, select: Option<&StatementSelect>) -> &mut Self {
    //     Self::config_select(self.cpp_obj, select.map_or(0, |s| s.get_cpp_obj()));
    //     self
    // }
    //
    // fn config_select(self_obj: i64, select: i64) {
    //     // 调用本地方法
    //     unimplemented!()
    // }
}
