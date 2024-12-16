use crate::base::cpp_object::CppObjectTrait;
use crate::core::database::Database;
use crate::core::handle_operation::HandleOperationTrait;
use crate::core::handle_orm_operation::HandleORMOperation;
use crate::core::prepared_statement::PreparedStatement;
use std::ffi::c_void;

pub struct Handle<'a> {
    handle_orm_operation: HandleORMOperation,

    main_statement: Option<PreparedStatement>,
    database: &'a Database,
    write_hint: bool,
}

impl<'a> HandleOperationTrait for Handle<'a> {
    fn get_handle(&self, write_hint: bool) -> Handle {
        unreachable!()
    }

    fn auto_invalidate_handle(&self) -> bool {
        false
    }
}

impl<'a> CppObjectTrait for Handle<'a> {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.handle_orm_operation.set_cpp_obj(cpp_obj);
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.handle_orm_operation.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.handle_orm_operation.release_cpp_object()
    }
}

impl<'a> Handle<'a> {
    pub fn new(database: &'a Database, write_hint: bool) -> Self {
        Self {
            handle_orm_operation: HandleORMOperation::new(),
            main_statement: None,
            database,
            write_hint,
        }
    }

    pub fn get_cpp_handle(&mut self) -> *mut c_void {
        let mut cpp_obj = self.handle_orm_operation.get_cpp_obj();
        if cpp_obj.is_null() {
            self.set_cpp_obj(Database::get_handle_raw(
                self.database.get_cpp_obj(),
                self.write_hint,
            ));
            cpp_obj = self.handle_orm_operation.get_cpp_obj();
        }
        cpp_obj
    }

    pub fn invalidate(&mut self) {
        self.main_statement.take();
        if !self.handle_orm_operation.get_cpp_obj().is_null() {
            self.handle_orm_operation.release_cpp_object();
            self.write_hint = false;
        }
    }
}