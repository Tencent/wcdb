use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::param::expression_convertible_param::ExpressionConvertibleParam;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use std::ffi::{c_char, c_double, c_int, c_void};

extern "C" {
    fn WCDBRustLiteralValue_create(
        value_type: c_int,
        value_long: i64,
        value_double: c_double,
        value_string: *const c_char,
    ) -> *mut c_void;
}

pub struct LiteralValue {
    pub(crate) identifier: Identifier,
}

impl CppObjectTrait for LiteralValue {
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

impl CppObjectConvertibleTrait for LiteralValue {
    fn as_cpp_object(&self) -> &CppObject {
        self.identifier.as_cpp_object()
    }
}

impl IdentifierTrait for LiteralValue {
    fn get_type(&self) -> CPPType {
        self.identifier.get_type()
    }

    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierConvertibleTrait for LiteralValue {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

impl LiteralValue {
    pub fn new<'a, T>(param: T) -> Self
    where
        T: Into<ExpressionConvertibleParam<'a>>,
    {
        let (arg_type, arg_long, arg_double, arg_string) = param.into().get_params();
        let cpp_obj = unsafe {
            WCDBRustLiteralValue_create(arg_type as c_int, arg_long, arg_double, arg_string)
        };
        LiteralValue {
            identifier: Identifier::new(CPPType::LiteralValue, Some(cpp_obj)),
        }
    }
}
