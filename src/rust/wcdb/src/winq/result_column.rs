use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::result_column_convertible_trait::ResultColumnConvertibleTrait;
use std::ffi::{c_char, c_int, c_void};

extern "C" {
    fn WCDBRustResultColumn_create(
        r#type: c_int,
        convertible_obj: *mut c_void,
        column_name: *const c_char,
    ) -> *mut c_void;

    fn WCDBRustResultColumn_configAlias(result_column: *mut c_void, alias: *const c_char);
}

pub struct ResultColumn {
    identifier: Identifier,
}

impl CppObjectTrait for ResultColumn {
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

impl CppObjectConvertibleTrait for ResultColumn {
    fn as_cpp_object(&self) -> &CppObject {
        self.identifier.as_cpp_object()
    }
}

impl IdentifierTrait for ResultColumn {
    fn get_type(&self) -> CPPType {
        self.identifier.get_type()
    }

    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierConvertibleTrait for ResultColumn {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

impl ResultColumnConvertibleTrait for ResultColumn {}

pub(crate) trait ResultColumnParam {
    fn create_cpp_obj(&self) -> *mut c_void;
}

impl ResultColumnParam for *mut c_void {
    fn create_cpp_obj(&self) -> *mut c_void {
        *self
    }
}

impl<T: ResultColumnConvertibleTrait> ResultColumnParam for T {
    fn create_cpp_obj(&self) -> *mut c_void {
        unsafe {
            WCDBRustResultColumn_create(
                Identifier::get_cpp_type(self) as c_int,
                CppObject::get(self),
                std::ptr::null(),
            )
        }
    }
}

impl ResultColumnParam for &str {
    fn create_cpp_obj(&self) -> *mut c_void {
        unsafe {
            WCDBRustResultColumn_create(
                CPPType::String as c_int,
                std::ptr::null_mut(),
                self.to_cstring().as_ptr(),
            )
        }
    }
}

impl ResultColumn {
    pub fn new<T: ResultColumnParam>(param: T) -> Self {
        ResultColumn {
            identifier: Identifier::new(CPPType::ResultColumn, Some(param.create_cpp_obj())),
        }
    }

    pub fn r#as(&self, alias: &str) -> &ResultColumn {
        unsafe { WCDBRustResultColumn_configAlias(self.get_cpp_obj(), alias.to_cstring().as_ptr()) }
        self
    }
}
