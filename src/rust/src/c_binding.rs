use std::ffi::{c_char, c_void};

pub type DatabaseCloseCallback = extern "C" fn(context: *mut c_void);

extern "C" {
    pub fn WCDBRustCore_createDatabase(path: *const c_char) -> *mut c_void;

    pub fn WCDBRustDatabase_close(cpp_obj: *mut c_void, context: *mut c_void, cb: DatabaseCloseCallback);
}
