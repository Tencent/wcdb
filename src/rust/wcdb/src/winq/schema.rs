use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use std::ffi::{c_char, c_void};

extern "C" {
    fn WCDBRustSchema_createWithName(table_name: *const c_char) -> *mut c_void;

    fn WCDBRustSchema_main() -> *mut c_void;

    fn WCDBRustSchema_temp() -> *mut c_void;
}
pub struct Schema {
    identifier: Identifier,
}

pub trait SchemaTrait: IdentifierTrait {
    fn main() -> Schema;
    fn temp() -> Schema;
}

impl CppObjectTrait for Schema {
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

impl CppObjectConvertibleTrait for Schema {
    fn as_cpp_object(&self) -> &CppObject {
        self.identifier.as_cpp_object()
    }
}

impl IdentifierTrait for Schema {
    fn get_type(&self) -> CPPType {
        self.identifier.get_type()
    }

    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierConvertibleTrait for Schema {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

impl SchemaTrait for Schema {
    fn main() -> Schema {
        let cpp_obj = unsafe { WCDBRustSchema_main() };
        Schema::new_with_cpp_obj(cpp_obj)
    }

    fn temp() -> Schema {
        let cpp_obj = unsafe { WCDBRustSchema_temp() };
        Schema::new_with_cpp_obj(cpp_obj)
    }
}

impl Schema {
    pub fn new(name: &str) -> Self {
        let cstr = name.to_cstring();
        let cpp_obj = unsafe { WCDBRustSchema_createWithName(cstr.as_ptr()) };
        Schema {
            identifier: Identifier::new(CPPType::Schema, Some(cpp_obj)),
        }
    }

    pub(crate) fn new_with_cpp_obj(cpp_obj: *mut c_void) -> Self {
        Schema {
            identifier: Identifier::new(CPPType::Schema, Some(cpp_obj)),
        }
    }
}
