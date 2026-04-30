use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::param::enum_string_indexed_column::StringIndexedColumn;
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::indexed_column_convertible::IndexedColumnConvertibleTrait;
use crate::winq::ordering_term::Order;
use std::ffi::{c_char, c_int, c_void};

extern "C" {
    fn WCDBRustIndexedColumn_create(
        cpp_type: c_int,
        object: *mut c_void,
        column_name: *const c_char,
    ) -> *mut c_void;

    fn WCDBRustIndexedColumn_configCollation(cpp_obj: *mut c_void, collation: *const c_char);
    fn WCDBRustIndexedColumn_configOrder(cpp_obj: *mut c_void, order: c_int);
}

pub struct IndexedColumn {
    identifier: Identifier,
}

impl CppObjectTrait for IndexedColumn {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.identifier.set_cpp_obj(cpp_obj)
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.identifier.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.identifier.release_cpp_object()
    }
}

impl CppObjectConvertibleTrait for IndexedColumn {
    fn as_cpp_object(&self) -> &CppObject {
        self.identifier.as_cpp_object()
    }
}

impl IdentifierTrait for IndexedColumn {
    fn get_type(&self) -> CPPType {
        self.identifier.get_type()
    }

    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierConvertibleTrait for IndexedColumn {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

impl IndexedColumnConvertibleTrait for IndexedColumn {}

impl IndexedColumn {
    pub fn new<'a, T>(param: T) -> Self
    where
        T: Into<StringIndexedColumn<'a>>,
    {
        let (cpp_type, c_obj, c_str_opt) = param.into().get_params();
        let c_str_ptr = match c_str_opt.as_ref() {
            None => std::ptr::null(),
            Some(s) => s.as_ptr(),
        };
        let cpp_obj = unsafe { WCDBRustIndexedColumn_create(cpp_type as c_int, c_obj, c_str_ptr) };
        IndexedColumn {
            identifier: Identifier::new(CPPType::IndexedColumn, Some(cpp_obj)),
        }
    }

    pub fn collate(&self, collation: &str) -> &Self {
        unsafe {
            WCDBRustIndexedColumn_configCollation(
                self.get_cpp_obj(),
                collation.to_cstring().as_ptr(),
            )
        }
        self
    }

    pub fn order(&self, order: Order) -> &Self {
        unsafe { WCDBRustIndexedColumn_configOrder(self.get_cpp_obj(), (order as c_int) + 1) }
        self
    }
}
