use crate::winq::column::Column;
use crate::winq::column_type::ColumnType;
use crate::winq::identifier::{get_cpp_type, CPPType, Identifier, IdentifierTrait};
use std::ffi::{c_char, c_void};

extern "C" {
    pub fn WCDBRustColumnDef_create(
        cpp_type: i32,
        column_cpp_obj: *mut c_void,
        name: *mut c_char,
        column_type: i32,
    ) -> *mut c_void;
}

pub struct ColumnDef {
    identifier: Identifier,
}

impl IdentifierTrait for ColumnDef {
    fn get_type() -> i32 {
        CPPType::ColumnDef as i32
    }
}

/// Identifier
impl ColumnDef {
    pub fn get_cpp_obj(&self) -> *mut c_void {
        self.identifier.get_cpp_obj()
    }
}

impl ColumnDef {
    pub fn new_with_column_type(column: &Column, column_type: ColumnType) -> Self {
        let cpp_obj = unsafe {
            WCDBRustColumnDef_create(
                get_cpp_type(column),
                column.get_cpp_obj(),
                std::ptr::null_mut(),
                column_type as i32,
            )
        };
        Self {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }
}
