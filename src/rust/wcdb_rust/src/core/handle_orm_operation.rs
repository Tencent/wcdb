use std::ffi::c_void;

use crate::core::handle_operation::HandleOperation;
use crate::core::handle_operation_trait::HandleOperationTrait;
use crate::orm::table_binding::TableBinding;

pub struct HandleORMOperation {
    handle_operation: HandleOperation,
}

impl HandleORMOperation {
    pub fn create_table<T: TableBinding<T>>(&self, table_name: &str, binding: T, handle_operation_trait: &dyn HandleOperationTrait) -> bool {
        let handle = handle_operation_trait.get_handle(true);
        binding.base_binding().create_table(table_name, handle)
    }
}

/// HandleOperation
impl HandleORMOperation {
    pub fn release_cpp_object(&mut self) {
        self.handle_operation.release_cpp_object();
    }
}

/// Rust
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
