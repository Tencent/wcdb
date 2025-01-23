use crate::base::cpp_object::CppObjectTrait;
use crate::winq::column::Column;
use crate::winq::column_constraint::ColumnConstraint;
use crate::winq::column_type::ColumnType;
use crate::winq::identifier::{get_cpp_type, CPPType, Identifier, IdentifierStaticTrait};
use std::ffi::{c_char, c_int, c_void};

extern "C" {
    pub fn WCDBRustColumnDef_create(
        cpp_type: c_int,
        column_cpp_obj: *mut c_void,
        name: *mut c_char,
        column_type: c_int,
    ) -> *mut c_void;

    pub fn WCDBRustColumnDef_constraint(cpp_obj: *mut c_void, constraint_cpp_obj: *mut c_void);
}

pub struct ColumnDef {
    identifier: Identifier,
}

impl CppObjectTrait for ColumnDef {
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

impl IdentifierStaticTrait for ColumnDef {
    fn get_type() -> i32 {
        CPPType::ColumnDef as i32
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

    pub fn constraint(&self, constraint: ColumnConstraint) -> &Self {
        unsafe {
            WCDBRustColumnDef_constraint(self.get_cpp_obj(), constraint.get_cpp_obj());
        }
        self
    }
}
