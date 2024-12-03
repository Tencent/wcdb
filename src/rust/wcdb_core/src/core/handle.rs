use std::ffi::c_void;

use crate::core::database::Database;
use crate::core::handle_operation_trait::HandleOperationTrait;
use crate::core::handle_orm_operation::HandleORMOperation;
use crate::core::prepared_statement::PreparedStatement;

pub struct Handle<'a> {
    handle_orm_operation: HandleORMOperation,

    main_statement: Option<PreparedStatement>,
    database: &'a Database,
    write_hint: bool,
}

impl<'a> Handle<'a> {
    pub fn new(database: &Database, write_hint: bool) -> Handle {
        Handle {
            handle_orm_operation: HandleORMOperation::new(),
            main_statement: None,
            database,
            write_hint,
        }
    }

    pub fn get_cpp_handle(&self) -> *mut c_void {
        let cpp_obj = self.handle_orm_operation.get_cpp_obj();
        if cpp_obj.is_null() {
            // TODO
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

impl<'a> HandleOperationTrait for Handle<'a> {
    fn get_handle(&self, write_hint: bool) -> Handle {
        unreachable!()
    }

    fn auto_invalidate_handle(&self) -> bool {
        false
    }
}
