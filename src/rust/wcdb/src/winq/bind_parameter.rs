use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use std::ffi::{c_char, c_int, c_void};
use std::sync::LazyLock;

extern "C" {
    fn WCDBRustBindParameter_createQuestionSignType(num: c_int) -> *mut c_void;

    fn WCDBRustBindParameter_createColonSignType(name: *const c_char) -> *mut c_void;

    fn WCDBRustBindParameter_createAtSignType(name: *const c_char) -> *mut c_void;

    fn WCDBRustBindParameter_createDollarSignType(name: *const c_char) -> *mut c_void;

    fn WCDBRustBindParameter_bindParameters(size: c_int) -> *mut *mut c_void;
}

pub struct BindParameter {
    identifier: Identifier,
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

impl CppObjectConvertibleTrait for BindParameter {
    fn as_cpp_object(&self) -> &CppObject {
        self.identifier.as_cpp_object()
    }
}

impl IdentifierTrait for BindParameter {
    fn get_type(&self) -> CPPType {
        self.identifier.get_type()
    }

    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierConvertibleTrait for BindParameter {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

pub trait BindParameterParam {
    fn create_cpp_obj(&self) -> *mut c_void;
}

impl BindParameterParam for i32 {
    fn create_cpp_obj(&self) -> *mut c_void {
        unsafe { WCDBRustBindParameter_createQuestionSignType(*self) }
    }
}

impl BindParameterParam for &str {
    fn create_cpp_obj(&self) -> *mut c_void {
        let cstr = self.to_cstring();
        unsafe { WCDBRustBindParameter_createColonSignType(cstr.as_ptr()) }
    }
}

impl BindParameter {
    pub fn new<T: BindParameterParam>(param: T) -> Self {
        let cpp_obj = param.create_cpp_obj();
        BindParameter {
            identifier: Identifier::new(CPPType::BindParameter, Some(cpp_obj)),
        }
    }

    pub fn at(name: &str) -> Self {
        let cpp_obj =
            { unsafe { WCDBRustBindParameter_createAtSignType(name.to_cstring().as_ptr()) } };
        BindParameter {
            identifier: Identifier::new(CPPType::BindParameter, Some(cpp_obj)),
        }
    }

    pub fn colon(name: &str) -> Self {
        BindParameter::new(name)
    }

    pub fn dollar(name: &str) -> Self {
        let cpp_obj =
            { unsafe { WCDBRustBindParameter_createDollarSignType(name.to_cstring().as_ptr()) } };
        BindParameter {
            identifier: Identifier::new(CPPType::BindParameter, Some(cpp_obj)),
        }
    }

    pub fn bind_parameters(num: i32) -> Vec<BindParameter> {
        if num <= 0 {
            return vec![];
        }
        let size: usize = num as usize;

        let cpp_obj_c_vec = unsafe { WCDBRustBindParameter_bindParameters(num as c_int) };
        let cpp_obj_vec_slice: &[*mut c_void] =
            unsafe { std::slice::from_raw_parts(cpp_obj_c_vec, size) };
        let cpp_obj_vec = cpp_obj_vec_slice.to_vec();
        let mut bind_parameter_vec: Vec<BindParameter> = Vec::with_capacity(size);
        for index in 0..size {
            let identifier = Identifier::new(CPPType::BindParameter, Some(cpp_obj_vec[index]));
            bind_parameter_vec.push(BindParameter { identifier });
        }
        unsafe { libc::free(cpp_obj_c_vec as *mut c_void) };
        bind_parameter_vec
    }
}

pub static DEF: LazyLock<BindParameter> = LazyLock::new(|| BindParameter::new(0)); //?
pub static _1: LazyLock<BindParameter> = LazyLock::new(|| BindParameter::new(1)); //?1
pub static _2: LazyLock<BindParameter> = LazyLock::new(|| BindParameter::new(2)); //?2
pub static _3: LazyLock<BindParameter> = LazyLock::new(|| BindParameter::new(3)); //?3
pub static _4: LazyLock<BindParameter> = LazyLock::new(|| BindParameter::new(4)); //?4
pub static _5: LazyLock<BindParameter> = LazyLock::new(|| BindParameter::new(5)); //?5
pub static _6: LazyLock<BindParameter> = LazyLock::new(|| BindParameter::new(6)); //?6
pub static _7: LazyLock<BindParameter> = LazyLock::new(|| BindParameter::new(7)); //?7
pub static _8: LazyLock<BindParameter> = LazyLock::new(|| BindParameter::new(8)); //?8
pub static _9: LazyLock<BindParameter> = LazyLock::new(|| BindParameter::new(9)); //?9
pub static _10: LazyLock<BindParameter> = LazyLock::new(|| BindParameter::new(10)); //?10
