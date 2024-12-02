use std::ffi::c_void;

use crate::base::result_code::ResultCode;
use crate::core::handle_operation::HandleOperation;
use crate::orm::table_binding::TableBinding;

pub struct HandleORMOperation {
    handle_operation: HandleOperation,
}

impl HandleORMOperation {
    pub fn create_table<T: TableBinding>(&self, table_name: &str, binding: T) -> ResultCode {
        binding.base_binding();
        ResultCode::Success
    }
}

impl HandleORMOperation {
    pub fn new() -> HandleORMOperation {
        HandleORMOperation {
            handle_operation: HandleOperation::new(),
        }
    }

    pub fn new_with_obj(cpp_obj: *mut c_void) -> HandleORMOperation {
        HandleORMOperation {
            handle_operation: HandleOperation::new_with_obj(cpp_obj),
        }
    }

    pub fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.handle_operation.set_cpp_obj(cpp_obj);
    }

    pub fn get_cpp_obj(&self) -> *mut c_void {
        self.handle_operation.get_cpp_obj()
    }
}
