use crate::base::cpp_object::CppObjectTrait;
use crate::core::handle_operation::{HandleOperation, HandleOperationTrait};
use crate::orm::table_binding::TableBinding;
use std::ffi::c_void;

pub struct HandleORMOperation {
    handle_operation: HandleOperation,
}

impl HandleORMOperation {
    pub fn new() -> Self {
        HandleORMOperation {
            handle_operation: HandleOperation::new(),
        }
    }

    pub fn new_with_obj(cpp_obj: *mut c_void) -> Self {
        HandleORMOperation {
            handle_operation: HandleOperation::new_with_obj(cpp_obj),
        }
    }

    pub fn create_table<T, R: TableBinding<T>>(
        &self,
        table_name: &str,
        binding: &R,
        handle_operation_trait: &dyn HandleOperationTrait,
    ) -> bool {
        let handle = handle_operation_trait.get_handle(true);
        binding.base_binding().create_table(table_name, handle)
    }
}

impl CppObjectTrait for HandleORMOperation {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.handle_operation.set_cpp_obj(cpp_obj)
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.handle_operation.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.handle_operation.release_cpp_object();
    }
}
