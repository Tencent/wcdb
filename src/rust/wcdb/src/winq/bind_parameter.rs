use crate::base::cpp_object::CppObjectTrait;
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait};
use std::ffi::{c_char, c_int, c_void};
use std::sync::LazyLock;

extern "C" {
    fn WCDBRustBindParameter_createQuestionSignType(num: c_int) -> *mut c_void;
    fn WCDBRustBindParameter_createColonSignType(name: *const c_char) -> *mut c_void;
    fn WCDBRustBindParameter_createAtSignType(name: *const c_char) -> *mut c_void;
    fn WCDBRustBindParameter_createDollarSignType(name: *const c_char) -> *mut c_void;
    fn WCDBRustBindParameter_bindParameters(buffers: *mut i64, size: c_int) -> *mut i64;
}

pub struct BindParameter {
    pub(crate) identifier: Identifier,
}

impl CppObjectTrait for BindParameter {
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

impl IdentifierTrait for BindParameter {
    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierStaticTrait for BindParameter {
    fn get_type() -> i32 {
        CPPType::BindParameter as i32
    }
}

impl BindParameter {
    fn new() -> Self {
        BindParameter {
            identifier: Identifier::new(),
        }
    }

    pub fn new_with_i32(num: i32) -> Self {
        let cpp_obj = unsafe { WCDBRustBindParameter_createQuestionSignType(num) };
        BindParameter {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn new_with_str(name: &str) -> Self {
        let cpp_obj = {
            let cstr = name.to_cstring();
            unsafe { WCDBRustBindParameter_createColonSignType(cstr.as_ptr()) }
        };
        BindParameter {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn at(name: &str) -> Self {
        let cpp_obj = {
            let cstr = name.to_cstring();
            unsafe { WCDBRustBindParameter_createAtSignType(cstr.as_ptr()) }
        };
        BindParameter {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn colon(name: &str) -> Self {
        BindParameter::new_with_str(name)
    }

    pub fn dollar(name: &str) -> Self {
        let cpp_obj = {
            let cstr = name.to_cstring();
            unsafe { WCDBRustBindParameter_createDollarSignType(cstr.as_ptr()) }
        };
        BindParameter {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn bind_parameters(num: i32) -> Vec<BindParameter> {
        if num <= 0 {
            return vec![];
        }
        let size: usize = num as usize;

        let mut cpp_obj_vec: Vec<i64> = vec![0; size];
        unsafe { WCDBRustBindParameter_bindParameters(cpp_obj_vec.as_mut_ptr(), num as c_int) };

        let mut bind_parameters_vec = Vec::with_capacity(size);
        for index in 0..size {
            let mut parameter = BindParameter::new();
            parameter
                .identifier
                .set_cpp_obj(cpp_obj_vec[index] as *mut c_void);
            bind_parameters_vec.push(parameter);
        }
        bind_parameters_vec
        // cpp_obj_vec 在这里自动释放
    }
}

//?
pub static DEF: LazyLock<BindParameter> = LazyLock::new(|| BindParameter::new_with_i32(0));
//?1
pub static _1: LazyLock<BindParameter> = LazyLock::new(|| BindParameter::new_with_i32(1));
//?2
pub static _2: LazyLock<BindParameter> = LazyLock::new(|| BindParameter::new_with_i32(2));
//?3
pub static _3: LazyLock<BindParameter> = LazyLock::new(|| BindParameter::new_with_i32(3));
//?4
pub static _4: LazyLock<BindParameter> = LazyLock::new(|| BindParameter::new_with_i32(4));
//?5
pub static _5: LazyLock<BindParameter> = LazyLock::new(|| BindParameter::new_with_i32(5));
//?6
pub static _6: LazyLock<BindParameter> = LazyLock::new(|| BindParameter::new_with_i32(6));
//?7
pub static _7: LazyLock<BindParameter> = LazyLock::new(|| BindParameter::new_with_i32(7));
//?8
pub static _8: LazyLock<BindParameter> = LazyLock::new(|| BindParameter::new_with_i32(8));
//?9
pub static _9: LazyLock<BindParameter> = LazyLock::new(|| BindParameter::new_with_i32(9));
//?10
pub static _10: LazyLock<BindParameter> = LazyLock::new(|| BindParameter::new_with_i32(10));
