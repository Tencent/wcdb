use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::param::enum_string_result_column::StringResultColumn;
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

impl ResultColumn {
    pub(crate) fn new_with_cpp_obj(cpp_obj: *mut c_void) -> Self {
        ResultColumn {
            identifier: Identifier::new(CPPType::ResultColumn, Some(cpp_obj)),
        }
    }

    pub fn new<'a, T>(param: T) -> Self
    where
        T: Into<StringResultColumn<'a>>,
    {
        let cpp_obj = match param.into() {
            StringResultColumn::String(column_name) => {
                let cstr = column_name.to_cstring();
                unsafe {
                    WCDBRustResultColumn_create(
                        CPPType::String as i32,
                        0 as *mut c_void,
                        cstr.as_ptr(),
                    )
                }
            }
            StringResultColumn::ResultColumn(result_column_convertible) => unsafe {
                WCDBRustResultColumn_create(
                    Identifier::get_cpp_type(result_column_convertible) as c_int,
                    CppObject::get(result_column_convertible),
                    std::ptr::null(),
                )
            },
        };
        ResultColumn {
            identifier: Identifier::new(CPPType::ResultColumn, Some(cpp_obj)),
        }
    }

    pub fn as_(&self, alias: &str) -> &ResultColumn {
        unsafe { WCDBRustResultColumn_configAlias(self.get_cpp_obj(), alias.to_cstring().as_ptr()) }
        self
    }
}
