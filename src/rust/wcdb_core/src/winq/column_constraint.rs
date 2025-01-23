use crate::base::cpp_object::CppObjectTrait;
use crate::winq::identifier::{CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait};
use std::ffi::{c_char, c_void, CString};

extern "C" {
    pub fn WCDBRustColumnConstraint_create(name: *const c_char) -> *mut c_void;

    pub fn WCDBRustColumnConstraint_configPrimaryKey(cpp_obj: *mut c_void);

    pub fn WCDBRustColumnConstraint_configAutoIncrement(cpp_obj: *mut c_void);
}

pub struct ColumnConstraint {
    identifier: Identifier,
}

impl CppObjectTrait for ColumnConstraint {
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

impl IdentifierTrait for ColumnConstraint {
    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierStaticTrait for ColumnConstraint {
    fn get_type() -> i32 {
        CPPType::ColumnConstraint as i32
    }
}

impl ColumnConstraint {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustColumnConstraint_create(std::ptr::null_mut()) };
        Self {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn new_by_column_name(column_name: &str) -> Self {
        let c_name = CString::new(column_name).unwrap_or_default();
        let cpp_obj = unsafe { WCDBRustColumnConstraint_create(c_name.as_ptr()) };
        Self {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn primary_key(&self) -> &Self {
        unsafe {
            WCDBRustColumnConstraint_configPrimaryKey(self.get_cpp_obj());
        }
        self
    }

    pub fn auto_increment(&self) -> &Self {
        unsafe {
            WCDBRustColumnConstraint_configAutoIncrement(self.get_cpp_obj());
        }
        self
    }
}
