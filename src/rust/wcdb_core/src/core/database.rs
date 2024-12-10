use std::ffi::{c_char, c_void, CString};
use std::ptr::null_mut;
use std::sync::{Arc, Mutex};

use crate::core::handle::Handle;
use crate::core::handle_operation_trait::HandleOperationTrait;
use crate::core::handle_orm_operation::HandleORMOperation;
use crate::orm::table_binding::TableBinding;

pub type DatabaseCloseCallback = extern "C" fn(context: *mut c_void);

extern "C" {
    pub fn WCDBRustCore_createDatabase(path: *const c_char) -> *mut c_void;
    pub fn WCDBRustDatabase_close(
        cpp_obj: *mut c_void,
        context: *mut c_void,
        cb: DatabaseCloseCallback,
    );
    pub fn WCDBRustDatabase_getHandle(cpp_obj: *mut c_void, write_hint: bool) -> *mut c_void;
}

pub struct Database {
    handle_orm_operation: HandleORMOperation,
    close_callback: Arc<Mutex<Option<Box<dyn FnOnce() + Send>>>>,
}

extern "C" fn close_callback_wrapper(context: *mut c_void) {
    if !context.is_null() {
        let boxed_cb: Box<Box<dyn FnOnce()>> = unsafe { Box::from_raw(context as *mut _) };
        boxed_cb();
    }
}

/// HandleORMOperation
impl Database {
    pub fn create_table<T, R: TableBinding<T>>(&self, table_name: &str, binding: &R) -> bool {
        self.handle_orm_operation
            .create_table(table_name, binding, self)
    }

    pub(crate) fn get_cpp_obj(&self) -> *mut c_void {
        self.handle_orm_operation.get_cpp_obj()
    }
}

impl HandleOperationTrait for Database {
    fn get_handle(&self, write_hint: bool) -> Handle {
        Handle::new(self, write_hint)
    }

    fn auto_invalidate_handle(&self) -> bool {
        true
    }
}

impl Database {
    pub fn new(path: &str) -> Self {
        let c_path = CString::new(path).unwrap_or_default();
        let cpp_obj = unsafe { WCDBRustCore_createDatabase(c_path.as_ptr()) };
        Database {
            handle_orm_operation: HandleORMOperation::new_with_obj(cpp_obj),
            close_callback: Arc::new(Mutex::new(None)),
        }
    }

    pub fn close<CB>(&self, cb_opt: Option<CB>)
    where
        CB: FnOnce() + Send + 'static,
    {
        match cb_opt {
            None => unsafe {
                WCDBRustDatabase_close(self.get_cpp_obj(), null_mut(), close_callback_wrapper)
            },
            Some(cb) => {
                let boxed_cb: Box<Box<dyn FnOnce()>> = Box::new(Box::new(cb));
                let context = Box::into_raw(boxed_cb) as *mut c_void;
                unsafe {
                    WCDBRustDatabase_close(self.get_cpp_obj(), context, close_callback_wrapper)
                }
            }
        }
    }

    /// static native long getHandle(long self, boolean writeHint);
    pub(crate) fn get_handle_raw(cpp_obj: *mut c_void, write_hint: bool) -> *mut c_void {
        unsafe { WCDBRustDatabase_getHandle(cpp_obj, write_hint) }
    }
}
