use crate::base::cpp_object::CppObjectTrait;
use crate::chaincall::insert::Insert;
use crate::core::handle_operation::HandleOperation;
use crate::orm::field::Field;
use crate::orm::table_binding::TableBinding;
use std::ffi::c_void;

pub struct HandleORMOperation {
    handle_operation: HandleOperation,
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

pub trait HandleORMOperationTrait {
    fn create_table<T, R: TableBinding<T>>(&self, table_name: &str, binding: &R) -> bool;
    fn insert_object<T>(&self, object: T, fields: Vec<&Field<T>>, table_name: &str);
    fn prepare_insert<T>(&self) -> Insert<T>;
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
}
