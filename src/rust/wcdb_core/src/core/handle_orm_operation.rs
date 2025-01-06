use crate::base::cpp_object::CppObjectTrait;
use crate::base::wcdb_exception::WCDBResult;
use crate::chaincall::delete::Delete;
use crate::chaincall::insert::Insert;
use crate::chaincall::update::Update;
use crate::core::handle_operation::{HandleOperation, HandleOperationTrait};
use crate::orm::field::Field;
use crate::orm::table_binding::TableBinding;
use crate::winq::expression::Expression;
use std::ffi::c_void;

#[derive(Debug)]
pub struct HandleORMOperation {
    handle_operation: HandleOperation,
}

pub trait HandleORMOperationTrait: HandleOperationTrait {
    fn create_table<T, R: TableBinding<T>>(
        &self,
        table_name: &str,
        binding: &R,
    ) -> WCDBResult<bool>;
    fn insert_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()>;
    fn insert_or_replace_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()>;
    fn insert_or_ignore_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()>;
    fn insert_objects<T>(
        &self,
        objects: Vec<T>,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()>;
    fn insert_or_replace_objects<T>(
        &self,
        objects: Vec<T>,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()>;
    fn insert_or_ignore_objects<T>(
        &self,
        objects: Vec<T>,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()>;
    fn prepare_insert<T>(&self) -> Insert<T>;
    fn prepare_update<T>(&self) -> Update<T>;
    fn prepare_delete(&self) -> Delete;
    fn delete_objects(&self, table_name: &str) -> WCDBResult<()>;
    fn delete_objects_by_expression(
        &self,
        table_name: &str,
        expression: Expression,
    ) -> WCDBResult<()>;

    fn update_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()>;
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
