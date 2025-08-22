use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use std::ffi::c_void;
use std::ops::{Deref, DerefMut};
use std::ptr::null_mut;

extern "C" {
    fn WCDBRustBase_releaseObject(cpp_obj: *mut c_void);
}

#[derive(Debug, Clone)]
pub(crate) struct CppObject {
    cpp_obj: *mut c_void,
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

pub trait CppObjectTrait {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void);
    fn get_cpp_obj(&self) -> *mut c_void;
    fn release_cpp_object(&mut self);
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
        self.cpp_obj = null_mut()
    }
}

impl CppObject {
    pub fn new() -> CppObject {
        CppObject {
            cpp_obj: null_mut(),
        }
    }

    pub fn new_with_obj(cpp_obj: *mut c_void) -> Self {
        CppObject { cpp_obj }
    }

    pub fn get<T: CppObjectTrait>(obj: &T) -> *mut c_void {
        obj.get_cpp_obj()
    }

    pub(crate) fn get_by_cpp_object_convertible_trait<T>(obj: &Option<&T>) -> i64
    where
        T: CppObjectConvertibleTrait,
    {
        if let Some(obj) = obj {
            obj.as_cpp_object() as i64
        } else {
            0
        }
    }
}
