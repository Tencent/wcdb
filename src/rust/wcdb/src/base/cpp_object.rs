use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use std::ffi::c_void;
use std::ops::{Deref, DerefMut};

extern "C" {
    fn WCDBRustBase_releaseObject(cpp_obj: *mut c_void);
}

#[derive(Debug, Clone)]
pub(crate) struct CppObject {
    pub(crate) cpp_obj: *mut c_void,
}

impl Deref for CppObject {
    type Target = *mut c_void;

    fn deref(&self) -> &Self::Target {
        &self.cpp_obj
    }
}

impl DerefMut for CppObject {
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.cpp_obj
    }
}

impl Drop for CppObject {
    fn drop(&mut self) {
        unsafe { WCDBRustBase_releaseObject(self.cpp_obj) };
    }
}

unsafe impl Send for CppObject {}

unsafe impl Sync for CppObject {}

/// 供“继承类”直接操作 cpp_obj
pub trait CppObjectTrait : CppObjectConvertibleTrait {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void);
    fn get_cpp_obj(&self) -> *mut c_void;
    fn release_cpp_object(&mut self);
}

impl CppObjectConvertibleTrait for CppObject {
    fn as_cpp_object(&self) -> &CppObject {
        self
    }
}

impl CppObjectTrait for CppObject {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.cpp_obj = cpp_obj;
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.cpp_obj
    }

    fn release_cpp_object(&mut self) {
        unsafe { WCDBRustBase_releaseObject(self.cpp_obj) };
        self.cpp_obj = std::ptr::null_mut()
    }
}

impl CppObject {
    pub fn new(cpp_obj_opt: Option<*mut c_void>) -> Self {
        let cpp_obj = cpp_obj_opt.unwrap_or_else(|| std::ptr::null_mut());
        CppObject { cpp_obj }
    }

    pub fn get<T: CppObjectConvertibleTrait>(obj: &T) -> *mut c_void {
        obj.as_cpp_object().cpp_obj
    }
}
