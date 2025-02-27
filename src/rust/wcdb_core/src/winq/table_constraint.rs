use crate::base::cpp_object::CppObjectTrait;
use crate::winq::identifier::{CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait};
use crate::winq::indexed_column_convertible::IndexedColumnConvertibleTrait;
use std::ffi::{c_char, c_int, c_void, CString};

extern "C" {
    pub fn WCDBRustTableConstraint_create(name: *const c_char) -> *mut c_void;
    pub fn WCDBRustTableConstraint_configPrimaryKey(cpp_obj: *mut c_void);
    pub fn WCDBRustTableConstraint_configIndexedColumn(
        cpp_obj: *mut c_void,
        columns_type: c_int,
        columns_void_vec: *const *mut c_void,
        columns_string_vec: *const *const c_char,
        columns_vec_len: c_int,
    );
}

pub struct TableConstraint {
    identifier: Identifier,
}

impl CppObjectTrait for TableConstraint {
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

impl IdentifierTrait for TableConstraint {
    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierStaticTrait for TableConstraint {
    fn get_type() -> i32 {
        CPPType::TableConstraint as i32
    }
}

impl TableConstraint {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustTableConstraint_create(std::ptr::null_mut()) };
        Self {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn new_by_constraint_name(constraint_name: &str) -> Self {
        let c_name = CString::new(constraint_name).unwrap_or_default();
        let cpp_obj = unsafe { WCDBRustTableConstraint_create(c_name.as_ptr()) };
        Self {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn primary_key(&self) -> &Self {
        unsafe {
            WCDBRustTableConstraint_configPrimaryKey(self.get_cpp_obj());
        }
        self
    }

    pub fn indexed_by<T>(&self, column_convertible_vec: Vec<T>) -> &Self
    where
        T: IndexedColumnConvertibleTrait + IdentifierStaticTrait,
    {
        if column_convertible_vec.is_empty() {
            return self;
        }
        let columns_void_vec_len = column_convertible_vec.len() as i32;
        let mut c_void_vec: Vec<*mut c_void> = Vec::with_capacity(column_convertible_vec.len());
        let cpp_type = Identifier::get_cpp_type(&column_convertible_vec[0]);
        for item in column_convertible_vec {
            c_void_vec.push(item.as_identifier().get_cpp_obj());
        }
        unsafe {
            WCDBRustTableConstraint_configIndexedColumn(
                self.get_cpp_obj(),
                cpp_type,
                c_void_vec.as_ptr(),
                std::ptr::null(),
                columns_void_vec_len,
            );
        }
        self
    }
}
