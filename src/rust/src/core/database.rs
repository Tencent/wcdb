use std::ffi::{c_char, c_void, CString};
use std::ptr::null_mut;
use std::sync::{Arc, Mutex};

use crate::base::cpp_object::CppObject;

pub type DatabaseCloseCallback = extern "C" fn(context: *mut c_void);

extern "C" {
    pub fn WCDBRustCore_createDatabase(path: *const c_char) -> *mut c_void;

    pub fn WCDBRustDatabase_close(
        cpp_obj: *mut c_void,
        context: *mut c_void,
        cb: DatabaseCloseCallback,
    );
}

pub struct Database {
    cpp_obj: CppObject,
    close_callback: Arc<Mutex<Option<Box<dyn FnOnce() + Send>>>>,
}

extern "C" fn close_callback_wrapper(context: *mut c_void) {
    if !context.is_null() {
        let boxed_cb: Box<Box<dyn FnOnce()>> = unsafe { Box::from_raw(context as *mut _) };
        boxed_cb();
    }
}

impl Database {
    pub fn try_new(path: &str) -> Option<Database> {
        let c_path = CString::new(path).unwrap_or_default();
        let cpp_obj_raw = unsafe { WCDBRustCore_createDatabase(c_path.as_ptr()) };
        if !cpp_obj_raw.is_null() {
            let cpp_obj = CppObject::new(cpp_obj_raw);
            Some(Database {
                cpp_obj,
                close_callback: Arc::new(Mutex::new(None)),
            })
        } else {
            None
        }
    }

    pub fn close<CB>(&self, cb_opt: Option<CB>)
    where
        CB: FnOnce() + Send + 'static,
    {
        match cb_opt {
            None => unsafe {
                WCDBRustDatabase_close(*self.cpp_obj, null_mut(), close_callback_wrapper)
            },
            Some(cb) => {
                let boxed_cb: Box<Box<dyn FnOnce()>> = Box::new(Box::new(cb));
                let context = Box::into_raw(boxed_cb) as *mut c_void;
                unsafe { WCDBRustDatabase_close(*self.cpp_obj, context, close_callback_wrapper) }
            }
        }
    }
}
