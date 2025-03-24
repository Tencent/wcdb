use crate::base::cpp_object::CppObjectTrait;
use crate::winq::identifier::{CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait};
use std::ffi::{c_char, c_double, c_int, c_long, c_void};
use std::ptr::null;

extern "C" {
    fn WCDBRustLiteralValue_create(
        value_type: c_int,
        value_long: c_long,
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

impl IdentifierTrait for LiteralValue {
    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierStaticTrait for LiteralValue {
    fn get_type() -> i32 {
        CPPType::LiteralValue as i32
    }
}

impl LiteralValue {
    pub fn new_with_i32(value: i32) -> Self {
        let cpp_obj =
            unsafe { WCDBRustLiteralValue_create(CPPType::Int as i32, value as i64, 0f64, null()) };
        LiteralValue {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn new_with_i64(value: i64) -> Self {
        let cpp_obj =
            unsafe { WCDBRustLiteralValue_create(CPPType::Int as i32, value, 0f64, null()) };
        LiteralValue {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn new_with_f32(value: f32) -> Self {
        let cpp_obj = unsafe {
            WCDBRustLiteralValue_create(CPPType::Double as i32, 0i64, value as f64, null())
        };
        LiteralValue {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn new_with_f64(value: f64) -> Self {
        let cpp_obj =
            unsafe { WCDBRustLiteralValue_create(CPPType::Double as i32, 0i64, value, null()) };
        LiteralValue {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn new_with_bool(value: bool) -> Self {
        let cpp_obj = unsafe {
            WCDBRustLiteralValue_create(
                CPPType::Bool as i32,
                if value { 1 } else { 0 } as i64,
                0f64,
                null(),
            )
        };
        LiteralValue {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn new_with_str(value_opt: Option<&str>) -> Self {
        let cpp_obj = match value_opt {
            None => unsafe {
                WCDBRustLiteralValue_create(CPPType::Null as i32, 0i64, 0f64, null())
            },
            Some(value) => unsafe {
                WCDBRustLiteralValue_create(
                    CPPType::String as i32,
                    0i64,
                    0f64,
                    value.as_ptr() as *const c_char,
                )
            },
        };
        LiteralValue {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }
}
