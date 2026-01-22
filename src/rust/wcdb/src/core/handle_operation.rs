use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::wcdb_exception::WCDBResult;
use crate::core::handle::Handle;
use crate::winq::statement::StatementTrait;
use std::ffi::c_void;

extern "C" {
    fn WCDBRustHandle_runTransaction(
        cpp_obj: *mut c_void,
        transaction_callback: extern "C" fn(
            cb_raw: *mut c_void,
            cpp_handle_raw: *mut c_void,
        ) -> bool,
        cb_raw: *mut c_void,
        rust_handle_raw: *mut c_void,
    ) -> bool;
}

extern "C" fn transaction_callback(cb_raw: *mut c_void, rust_handle_raw: *mut c_void) -> bool {
    let handle = unsafe { *(rust_handle_raw as *const &Handle) };
    let closure: Box<Box<dyn FnOnce(&Handle) -> bool>> =
        unsafe { Box::from_raw(cb_raw as *mut Box<dyn FnOnce(&Handle) -> bool>) };
    closure(handle)
}

#[derive(Debug)]
pub struct HandleOperation {
    cpp_obj: CppObject,
}

pub trait HandleOperationTrait: CppObjectTrait {
    fn get_handle(&self, write_hint: bool) -> Handle;

    fn auto_invalidate_handle(&self) -> bool;

    fn run_transaction<F: FnOnce(&Handle) -> bool>(&self, callback: F) -> WCDBResult<()>;

    fn execute<T: StatementTrait>(&self, statement: &T) -> WCDBResult<()>;

    fn execute_sql(&self, sql: &str) -> WCDBResult<()>;
}

impl CppObjectConvertibleTrait for HandleOperation {
    fn as_cpp_object(&self) -> &CppObject {
        self.cpp_obj.as_cpp_object()
    }
}

impl CppObjectTrait for HandleOperation {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        *self.cpp_obj = cpp_obj;
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        *self.cpp_obj
    }

    fn release_cpp_object(&mut self) {
        self.cpp_obj.release_cpp_object();
    }
}

impl HandleOperation {
    pub fn new(cpp_obj_opt: Option<*mut c_void>) -> Self {
        HandleOperation {
            cpp_obj: CppObject::new(cpp_obj_opt),
        }
    }

    pub(crate) fn run_transaction<F: FnOnce(&Handle) -> bool>(
        &self,
        handle: Handle,
        auto_invalidate_handle: bool,
        callback: F,
    ) -> WCDBResult<()> {
        let mut handle = handle;
        let closure_box: Box<Box<dyn FnOnce(&Handle) -> bool>> = Box::new(Box::new(callback));
        let closure_raw = Box::into_raw(closure_box) as *mut c_void;
        let rust_handle_raw = unsafe { &(&handle) as *const &Handle as *mut c_void };
        let mut exception_opt = None;
        if !unsafe {
            WCDBRustHandle_runTransaction(
                handle.get_cpp_handle()?,
                transaction_callback,
                closure_raw,
                rust_handle_raw,
            )
        } {
            exception_opt = Some(handle.create_exception());
        }
        if auto_invalidate_handle {
            handle.invalidate();
        }
        match exception_opt {
            None => Ok(()),
            Some(exception) => Err(exception),
        }
    }

    pub(crate) fn execute<T: StatementTrait>(
        &self,
        handle: Handle,
        auto_invalidate_handle: bool,
        statement: &T,
    ) -> WCDBResult<()> {
        let mut handle = handle;
        let mut exception_opt = None;
        if !Handle::execute_inner(handle.get_cpp_handle()?, statement) {
            exception_opt = Some(handle.create_exception());
        }
        if auto_invalidate_handle {
            handle.invalidate();
        }
        match exception_opt {
            None => Ok(()),
            Some(exception) => Err(exception),
        }
    }

    pub(crate) fn execute_sql(
        &self,
        handle: Handle,
        auto_invalidate_handle: bool,
        sql: &str,
    ) -> WCDBResult<()> {
        let mut handle = handle;
        let mut exception_opt = None;
        if !Handle::execute_sql(handle.get_cpp_handle()?, sql) {
            exception_opt = Some(handle.create_exception());
        }
        if auto_invalidate_handle {
            handle.invalidate();
        }
        match exception_opt {
            None => Ok(()),
            Some(exception) => Err(exception),
        }
    }
}
