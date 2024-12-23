use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::core::database::Database;
use crate::core::handle_operation::HandleOperationTrait;
use crate::core::handle_orm_operation::HandleORMOperation;
use crate::core::prepared_statement::PreparedStatement;
use crate::wcdb_error::{WCDBError, WCDBResult};
use crate::winq::statement::StatementTrait;
use std::ffi::c_void;
use std::sync::{Arc, Mutex};

extern "C" {
    pub fn WCDBRustHandle_runTransaction(
        cpp_obj: *mut c_void,
        transaction_callback: extern "C" fn(*mut c_void, *mut c_void, *mut c_void),
        closure_raw: *mut c_void,
        database_raw: *mut c_void,
    ) -> bool;
}

extern "C" fn transaction_callback(
    closure_raw: *mut c_void,
    database_raw: *mut c_void,
    cpp_handle: *mut c_void,
) {
    let database = unsafe { *(database_raw as *const &Database) };
    let handle = Handle::new_with_obj(cpp_handle, &database);
    let closure: Box<Box<dyn FnOnce(Handle) -> bool>> =
        unsafe { Box::from_raw(closure_raw as *mut Box<dyn FnOnce(Handle) -> bool>) };
    closure(handle);
}

pub struct HandleInner {
    handle_orm_operation: HandleORMOperation,
    main_statement: Option<Arc<PreparedStatement>>,
    write_hint: bool,
}

impl CppObjectTrait for HandleInner {
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

impl HandleInner {
    pub fn get_cpp_handle(&mut self, database: &Database) -> *mut c_void {
        let mut cpp_obj = self.handle_orm_operation.get_cpp_obj();
        if cpp_obj.is_null() {
            self.set_cpp_obj(Database::get_handle_raw(
                CppObject::get(database),
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

    pub fn prepared_with_main_statement<T: StatementTrait + CppObjectTrait>(
        &mut self,
        database: &Database,
        statement: &T,
    ) -> Arc<PreparedStatement> {
        if self.main_statement.is_none() {
            let mut prepared_statement = PreparedStatement::new(self.get_cpp_handle(database));
            prepared_statement.auto_finalize = true;
            self.main_statement = Some(Arc::new(prepared_statement));
        }
        let main_statement = self.main_statement.as_ref().unwrap();
        main_statement.prepare(statement);
        main_statement.clone()
    }
}

pub struct Handle<'a> {
    handle_inner: Arc<Mutex<HandleInner>>,
    database: &'a Database,
}

impl<'a> HandleOperationTrait for Handle<'a> {
    fn get_handle(&self, write_hint: bool) -> Handle {
        Handle {
            handle_inner: self.handle_inner.clone(),
            database: self.database,
        }
    }

    fn auto_invalidate_handle(&self) -> bool {
        false
    }

    fn run_transaction<F: FnOnce(Handle) -> bool>(&self, closure: F) -> WCDBResult<()> {
        let mut handle = self.get_handle(true);
        let closure_box: Box<Box<dyn FnOnce(Handle) -> bool>> = Box::new(Box::new(closure));
        let closure_raw = Box::into_raw(closure_box) as *mut c_void;
        let database_raw = unsafe { &self.database as *const &Database as *mut c_void };
        if !unsafe {
            WCDBRustHandle_runTransaction(
                handle.get_cpp_handle(),
                transaction_callback,
                closure_raw,
                database_raw,
            )
        } {
            return Err(WCDBError::Exception);
        }
        if self.auto_invalidate_handle() {
            self.invalidate();
        }
        Ok(())
    }
}

impl<'a> CppObjectTrait for Handle<'a> {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        let mut handle_inner_lock = self.handle_inner.lock().unwrap();
        handle_inner_lock.set_cpp_obj(cpp_obj);
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        let handle_inner_lock = self.handle_inner.lock().unwrap();
        handle_inner_lock.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        let mut handle_inner_lock = self.handle_inner.lock().unwrap();
        handle_inner_lock.release_cpp_object();
    }
}

impl<'a> Handle<'a> {
    pub fn new(database: &'a Database, write_hint: bool) -> Self {
        let handle_inner = Arc::new(Mutex::new(HandleInner {
            handle_orm_operation: HandleORMOperation::new(),
            main_statement: None,
            write_hint,
        }));
        Self {
            handle_inner,
            database,
        }
    }

    pub fn new_with_obj(cpp_obj: *mut c_void, database: &'a Database) -> Self {
        let handle_inner = Arc::new(Mutex::new(HandleInner {
            handle_orm_operation: HandleORMOperation::new_with_obj(cpp_obj),
            main_statement: None,
            write_hint: false,
        }));
        Self {
            handle_inner,
            database,
        }
    }

    pub fn get_cpp_handle(&self) -> *mut c_void {
        let mut handle_inner_lock = self.handle_inner.lock().unwrap();
        handle_inner_lock.get_cpp_handle(self.database)
    }

    pub fn invalidate(&self) {
        let mut handle_inner_lock = self.handle_inner.lock().unwrap();
        handle_inner_lock.invalidate();
    }

    pub fn prepared_with_main_statement<T: StatementTrait + CppObjectTrait>(
        &self,
        statement: &T,
    ) -> Arc<PreparedStatement> {
        let mut handle_inner_lock = self.handle_inner.lock().unwrap();
        handle_inner_lock.prepared_with_main_statement(self.database, statement)
    }
}
