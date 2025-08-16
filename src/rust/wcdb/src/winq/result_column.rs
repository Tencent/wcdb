use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::identifier::{CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::result_column_convertible_trait::ResultColumnConvertibleTrait;
use std::ffi::{c_char, c_int, c_void, CString};
use std::ptr::null;

extern "C" {
    fn WCDBRustResultColumn_create(
        cpp_type: c_int,
        convertible_obj: *mut c_void,
        column_name: *const c_char,
    ) -> *mut c_void;

    fn WCDBRustResultColumn_configAlias(result_column: *mut c_void, alias: *const c_char);
}

pub struct ResultColumn {
    identifier: Identifier,
}

impl IdentifierConvertibleTrait for ResultColumn {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

impl CppObjectConvertibleTrait for ResultColumn {
    fn as_cpp_object(&self) -> *mut c_void {
        self.identifier.as_cpp_object()
    }
}

impl ResultColumnConvertibleTrait for ResultColumn {}

impl IdentifierStaticTrait for ResultColumn {
    fn get_type() -> i32 {
        CPPType::ResultColumn as i32
    }
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

impl IdentifierTrait for ResultColumn {
    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl ExpressionConvertibleTrait for ResultColumn {}

impl ResultColumn {
    pub(crate) fn new_with_cpp_obj(cpp_obj: *mut c_void) -> Self {
        ResultColumn {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn new_with_result_column_convertible<T>(result_column_convertible: &T) -> Self
    where
        T: ResultColumnConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    {
        let cpp_obj = unsafe {
            WCDBRustResultColumn_create(
                Identifier::get_cpp_type(result_column_convertible),
                CppObject::get(result_column_convertible),
                null(),
            )
        };
        ResultColumn {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn new_with_column_name(column_name: &str) -> Self {
        let cstr = column_name.to_cstring();
        let cpp_obj = unsafe {
            WCDBRustResultColumn_create(CPPType::String as i32, 0 as *mut c_void, cstr.as_ptr())
        };
        ResultColumn {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn as_(self, alias: &str) -> ResultColumn {
        let cstr = alias.to_cstring();
        unsafe { WCDBRustResultColumn_configAlias(self.get_cpp_obj(), cstr.as_ptr()) }
        self
    }
}
