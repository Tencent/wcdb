use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::wcdb_exception::{ExceptionCode, ExceptionLevel, WCDBException, WCDBResult};
use crate::core::database::Database;
use crate::core::handle_operation::HandleOperationTrait;
use crate::core::handle_orm_operation::HandleORMOperation;
use crate::core::prepared_statement::PreparedStatement;
use crate::winq::statement::StatementTrait;
use std::ffi::{c_char, c_int, c_long, c_void, CString};
use std::sync::{Arc, Mutex};

extern "C" {
    fn WCDBRustHandle_getError(cpp_obj: *mut c_void) -> *mut c_void;
    fn WCDBRustHandle_getMainStatement(cpp_obj: *mut c_void) -> *mut c_void;
    fn WCDBRustHandle_tableExist(cpp_obj: *mut c_void, table_name: *const c_char) -> c_int;
    fn WCDBRustHandle_execute(cpp_obj: *mut c_void, statement: *mut c_void) -> bool;
    fn WCDBRustHandle_executeSQL(cpp_obj: *mut c_void, sql: *const c_char) -> bool;
    fn WCDBRustHandle_getChanges(cpp_obj: *mut c_void) -> c_int;
    fn WCDBRustHandle_getLastInsertRowid(cpp_obj: *mut c_void) -> c_long;
    fn WCDBRustHandle_runTransaction(
        cpp_obj: *mut c_void,
        transaction_callback: extern "C" fn(*mut c_void, *mut c_void, *mut c_void) -> bool,
        closure_raw: *mut c_void,
        database_raw: *mut c_void,
    ) -> bool;
}

extern "C" fn transaction_callback(
    closure_raw: *mut c_void,
    database_raw: *mut c_void,
    cpp_handle: *mut c_void,
) -> bool {
    let database = unsafe { *(database_raw as *const &Database) };
    let handle = Handle::new_with_obj(cpp_handle, &database);
    let closure: Box<Box<dyn FnOnce(Handle) -> bool>> =
        unsafe { Box::from_raw(closure_raw as *mut Box<dyn FnOnce(Handle) -> bool>) };
    closure(handle)
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
    pub fn get_cpp_handle(&mut self, database: &Database) -> WCDBResult<*mut c_void> {
        let mut cpp_obj = self.get_cpp_obj();
        if cpp_obj.is_null() {
            self.set_cpp_obj(Database::get_handle_raw(
                CppObject::get(database),
                self.write_hint,
            ));
            if self.get_cpp_obj().is_null() {
                return Err(database.create_exception());
            }
        }
        Ok(self.get_cpp_obj())
    }

    pub fn invalidate(&mut self) {
        self.main_statement.take();
        if !self.handle_orm_operation.get_cpp_obj().is_null() {
            self.handle_orm_operation.release_cpp_object();
            self.write_hint = false;
        }
    }

    pub fn get_changes(&mut self, database: &Database) -> WCDBResult<i32> {
        Ok(unsafe { WCDBRustHandle_getChanges(self.get_cpp_handle(database)?) })
    }

    pub fn prepared_with_main_statement<T: StatementTrait>(
        &mut self,
        database: &Database,
        statement: &T,
    ) -> WCDBResult<Arc<PreparedStatement>> {
        if self.main_statement.is_none() {
            let cpp_obj =
                unsafe { WCDBRustHandle_getMainStatement(self.get_cpp_handle(database)?) };
            let mut prepared_statement = PreparedStatement::new(cpp_obj);
            prepared_statement.auto_finalize = true;
            self.main_statement = Some(Arc::new(prepared_statement));
        }
        let main_statement = self.main_statement.as_ref().unwrap();
        main_statement.prepare(statement)?;
        Ok(main_statement.clone())
    }

    pub fn prepared_with_main_statement_and_sql(
        &mut self,
        database: &Database,
        sql: &str,
    ) -> WCDBResult<Arc<PreparedStatement>> {
        if self.main_statement.is_none() {
            let cpp_obj =
                unsafe { WCDBRustHandle_getMainStatement(self.get_cpp_handle(database)?) };
            let mut prepared_statement = PreparedStatement::new(cpp_obj);
            prepared_statement.auto_finalize = true;
            self.main_statement = Some(Arc::new(prepared_statement));
        }
        let main_statement = self.main_statement.as_ref().unwrap();
        main_statement.prepare_with_sql(sql)?;
        Ok(main_statement.clone())
    }
}

pub struct Handle<'a> {
    handle_inner: Arc<Mutex<HandleInner>>,
    database: &'a Database,
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

impl<'a> HandleOperationTrait for Handle<'a> {
    fn get_handle(&self, _: bool) -> Handle {
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
        let mut exception_opt = None;
        if !unsafe {
            WCDBRustHandle_runTransaction(
                handle.get_cpp_handle()?,
                transaction_callback,
                closure_raw,
                database_raw,
            )
        } {
            exception_opt = Some(handle.create_exception());
        }
        if self.auto_invalidate_handle() {
            self.invalidate();
        }
        match exception_opt {
            None => Ok(()),
            Some(exception) => Err(exception),
        }
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

    pub fn get_cpp_handle(&self) -> WCDBResult<*mut c_void> {
        let mut handle_inner_lock = self.handle_inner.lock();
        match handle_inner_lock {
            Ok(mut handle) => handle.get_cpp_handle(self.database),
            Err(error) => Err(WCDBException::new(
                ExceptionLevel::Error,
                ExceptionCode::Error,
                error.to_string(),
            )),
        }
    }

    pub fn create_exception(&self) -> WCDBException {
        WCDBException::create_exception(unsafe { WCDBRustHandle_getError(self.get_cpp_obj()) })
    }

    pub fn invalidate(&self) {
        let mut handle_inner_lock = self.handle_inner.lock().unwrap();
        handle_inner_lock.invalidate();
    }

    pub fn get_changes(&self) -> WCDBResult<i32> {
        let mut handle_inner_lock = self.handle_inner.lock().unwrap();
        handle_inner_lock.get_changes(self.database)
    }

    pub fn get_last_inserted_row_id(&self) -> WCDBResult<i64> {
        Ok(unsafe { WCDBRustHandle_getLastInsertRowid(self.get_cpp_handle()?) })
    }

    pub fn prepared_with_main_statement<T: StatementTrait>(
        &self,
        statement: &T,
    ) -> WCDBResult<Arc<PreparedStatement>> {
        let mut handle_inner_lock = self.handle_inner.lock().unwrap();
        handle_inner_lock.prepared_with_main_statement(self.database, statement)
    }

    pub fn prepared_with_main_statement_and_sql(
        &self,
        sql: &str,
    ) -> WCDBResult<Arc<PreparedStatement>> {
        let mut handle_inner_lock = self.handle_inner.lock().unwrap();
        handle_inner_lock.prepared_with_main_statement_and_sql(self.database, sql)
    }

    pub fn table_exist(cpp_obj: *mut c_void, table_name: &str) -> i32 {
        let c_table_name = CString::new(table_name).unwrap_or_default();
        unsafe { WCDBRustHandle_tableExist(cpp_obj, c_table_name.as_ptr()) }
    }

    pub fn execute_inner<T: StatementTrait>(cpp_obj: *mut c_void, statement: &T) -> bool {
        unsafe { WCDBRustHandle_execute(cpp_obj, CppObject::get(statement)) }
    }

    pub fn execute<T: StatementTrait>(&self, statement: &T) -> WCDBResult<()> {
        let handle = self.get_handle(statement.is_write_statement());
        let mut exception_opt = None;
        if !unsafe { WCDBRustHandle_execute(handle.get_cpp_handle()?, CppObject::get(statement)) } {
            exception_opt = Some(handle.create_exception());
        }
        if self.auto_invalidate_handle() {
            self.invalidate();
        }
        match exception_opt {
            None => Ok(()),
            Some(exception) => Err(exception),
        }
    }

    pub fn execute_sql(cpp_obj: *mut c_void, sql: &str) -> bool {
        let c_sql = CString::new(sql).unwrap_or_default();
        unsafe { WCDBRustHandle_executeSQL(cpp_obj, c_sql.as_ptr()) }
    }
}
