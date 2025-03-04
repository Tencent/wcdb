use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier, IdentifierStaticTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::indexed_column_convertible::IndexedColumnConvertibleTrait;
use crate::winq::ordering_term::Order;
use std::ffi::{c_char, c_int, c_void};
use std::ptr::null;

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

impl IdentifierConvertibleTrait for IndexedColumn {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

impl CppObjectConvertibleTrait for IndexedColumn {
    fn as_cpp_object(&self) -> *mut c_void {
        self.identifier.as_cpp_object()
    }
}

impl IndexedColumnConvertibleTrait for IndexedColumn {}

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

impl IdentifierStaticTrait for IndexedColumn {
    fn get_type() -> i32 {
        CPPType::IndexedColumn as i32
    }
}

impl IndexedColumn {
    pub fn new_with_indexed_column_convertible_trait<T>(indexed_column_convertible: &T) -> Self
    where
        T: IndexedColumnConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    {
        let cpp_obj = unsafe {
            WCDBRustIndexedColumn_create(
                Identifier::get_cpp_type(indexed_column_convertible) as c_int,
                CppObject::get(indexed_column_convertible),
                null(),
            )
        };
        IndexedColumn {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn new_with_column_name(column_name: &str) -> Self {
        let cstr = column_name.to_cstring();
        let cpp_obj = unsafe {
            WCDBRustIndexedColumn_create(CPPType::String as c_int, 0 as *mut c_void, cstr.as_ptr())
        };
        IndexedColumn {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn collate(&self, collation: &str) -> &Self {
        let cstr = collation.to_cstring();
        unsafe { WCDBRustIndexedColumn_configCollation(self.get_cpp_obj(), cstr.as_ptr()) }
        self
    }

    pub fn order(&self, order: Order) -> &Self {
        unsafe { WCDBRustIndexedColumn_configOrder(self.get_cpp_obj(), (order as c_int) + 1) }
        self
    }
}
