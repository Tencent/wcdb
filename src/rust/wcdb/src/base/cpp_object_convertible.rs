use std::ffi::c_void;

pub trait CppObjectConvertibleTrait {
    fn as_cpp_object(&self) -> *mut c_void;
}
