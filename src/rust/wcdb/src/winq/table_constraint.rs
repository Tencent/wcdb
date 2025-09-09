use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::param::string_indexed_column_convertible_param::StringIndexedColumnConvertibleParam;
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use core::ffi::c_size_t;
use std::ffi::{c_char, c_int, c_void};

extern "C" {
    fn WCDBRustTableConstraint_create(name: *const c_char) -> *mut c_void;

    fn WCDBRustTableConstraint_configPrimaryKey(cpp_obj: *mut c_void);

    fn WCDBRustTableConstraint_configUnique(cpp_obj: *mut c_void);

    fn WCDBRustTableConstraint_configIndexedColumn(
        cpp_obj: *mut c_void,
        columns_type: c_int,
        column_vec: *const *mut c_void,
        column_name_vec: *const *const c_char,
        column_vec_len: c_size_t,
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

impl CppObjectConvertibleTrait for TableConstraint {
    fn as_cpp_object(&self) -> &CppObject {
        self.identifier.as_cpp_object()
    }
}

impl IdentifierTrait for TableConstraint {
    fn get_type(&self) -> CPPType {
        self.identifier.get_type()
    }

    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierConvertibleTrait for TableConstraint {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

impl TableConstraint {
    pub fn new(name_opt: Option<&str>) -> Self {
        let cpp_obj = match name_opt {
            Some(name) => unsafe { WCDBRustTableConstraint_create(name.to_cstring().as_ptr()) },
            None => unsafe { WCDBRustTableConstraint_create(std::ptr::null_mut()) },
        };
        Self {
            identifier: Identifier::new(CPPType::TableConstraint, Some(cpp_obj)),
        }
    }

    pub fn primary_key(&self) -> &Self {
        unsafe {
            WCDBRustTableConstraint_configPrimaryKey(self.get_cpp_obj());
        }
        self
    }

    pub fn unique(&self) -> &Self {
        unsafe {
            WCDBRustTableConstraint_configUnique(self.get_cpp_obj());
        }
        self
    }

    pub fn indexed_by<'a, I, S>(&self, column_vec: I) -> &Self
    where
        I: IntoIterator<Item = S>,
        S: Into<StringIndexedColumnConvertibleParam<'a>>,
    {
        let mut data_vec = column_vec.into_iter().map(Into::into).peekable();
        if data_vec.peek().is_none() {
            return self;
        }
        let mut cpp_type = CPPType::String;
        let mut cpp_str_vec = vec![];
        let mut cpp_obj_vec = vec![];
        for item in data_vec {
            match item {
                StringIndexedColumnConvertibleParam::String(str) => {
                    cpp_str_vec.push(str.as_str().to_cstring().as_ptr());
                }
                StringIndexedColumnConvertibleParam::IndexedColumnConvertible(obj) => {
                    cpp_type = Identifier::get_cpp_type(obj.as_identifier());
                    cpp_obj_vec.push(CppObject::get(obj));
                }
            }
        }
        if !cpp_str_vec.is_empty() {
            unsafe {
                WCDBRustTableConstraint_configIndexedColumn(
                    self.get_cpp_obj(),
                    CPPType::String as c_int,
                    std::ptr::null(),
                    cpp_str_vec.as_ptr(),
                    cpp_obj_vec.len(),
                );
            }
        } else {
            unsafe {
                WCDBRustTableConstraint_configIndexedColumn(
                    self.get_cpp_obj(),
                    cpp_type as c_int,
                    cpp_obj_vec.as_ptr(),
                    std::ptr::null(),
                    cpp_obj_vec.len(),
                );
            }
        }
        self
    }
}
