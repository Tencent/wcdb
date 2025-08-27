use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
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

pub trait IndexedColumnParam {
    fn get_cpp_obj(&self) -> *mut c_void;
}

impl<T: IndexedColumnConvertibleTrait> IndexedColumnParam for &T {
    fn get_cpp_obj(&self) -> *mut c_void {
        unsafe {
            WCDBRustIndexedColumn_create(
                Identifier::get_cpp_type(self) as c_int,
                CppObject::get(self),
                std::ptr::null(),
            )
        }
    }
}

impl IndexedColumnParam for &str {
    fn get_cpp_obj(&self) -> *mut c_void {
        unsafe {
            WCDBRustIndexedColumn_create(
                CPPType::String as c_int,
                std::ptr::null_mut(),
                self.to_cstring().as_ptr(),
            )
        }
    }
}

impl IndexedColumn {
    pub fn new<T: IndexedColumnParam>(param: T) -> Self {
        let cpp_obj = param.get_cpp_obj();
        IndexedColumn {
            identifier: Identifier::new(CPPType::IndexedColumn, Some(cpp_obj)),
        }
    }

    pub fn collate(&self, collation: &str) -> &Self {
        unsafe { WCDBRustIndexedColumn_configCollation(self.get_cpp_obj(), collation.to_cstring().as_ptr()) }
        self
    }

    pub fn order(&self, order: Order) -> &Self {
        unsafe { WCDBRustIndexedColumn_configOrder(self.get_cpp_obj(), (order as c_int) + 1) }
        self
    }
}
