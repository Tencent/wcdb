use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::wcdb_exception::{ExceptionCode, ExceptionLevel, WCDBException, WCDBResult};
use crate::chaincall::delete::Delete;
use crate::chaincall::insert::Insert;
use crate::chaincall::select::Select;
use crate::chaincall::update::Update;
use crate::core::database::Database;
use crate::core::handle_operation::HandleOperationTrait;
use crate::core::handle_orm_operation::{HandleORMOperation, HandleORMOperationTrait};
use crate::core::prepared_statement::PreparedStatement;
use crate::orm::field::Field;
use crate::orm::table_binding::TableBinding;
use crate::winq::expression::Expression;
use crate::winq::ordering_term::OrderingTerm;
use crate::winq::statement::StatementTrait;
use std::cell::RefCell;
use std::ffi::{c_char, c_int, c_void, CString};
use std::sync::Arc;

extern "C" {
    fn WCDBRustHandle_getError(cpp_obj: *mut c_void) -> *mut c_void;
    fn WCDBRustHandle_getMainStatement(cpp_obj: *mut c_void) -> *mut c_void;
    fn WCDBRustHandle_tableExist(cpp_obj: *mut c_void, table_name: *const c_char) -> c_int;
    fn WCDBRustHandle_execute(cpp_obj: *mut c_void, statement: *mut c_void) -> bool;
    fn WCDBRustHandle_executeSQL(cpp_obj: *mut c_void, sql: *const c_char) -> bool;
    fn WCDBRustHandle_getChanges(cpp_obj: *mut c_void) -> c_int;
    fn WCDBRustHandle_getLastInsertRowid(cpp_obj: *mut c_void) -> i64;
}

pub struct HandleInner {
    handle_orm_operation: HandleORMOperation,
    main_statement: Option<Arc<PreparedStatement>>,
    write_hint: bool,
}

impl CppObjectConvertibleTrait for HandleInner {
    fn as_cpp_object(&self) -> &CppObject {
        self.handle_orm_operation.as_cpp_object()
    }
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
            match self.get_cpp_handle(database) {
                Ok(handle_cpp) => {
                    let cpp_obj = unsafe { WCDBRustHandle_getMainStatement(handle_cpp) };
                    let mut prepared_statement = PreparedStatement::new(Some(cpp_obj));
                    prepared_statement.auto_finalize = true;
                    self.main_statement = Some(Arc::new(prepared_statement));
                }
                Err(error) => {
                    return Err(error.into());
                }
            }
        }
        match self.main_statement.as_ref() {
            None => Err(WCDBException::new_with_message(
                ExceptionLevel::Error,
                ExceptionCode::Error,
                String::from(
                    "Method :prepared_with_main_statement error, cause :main_statement is none",
                ),
            )),
            Some(main_statement) => {
                main_statement.prepare(statement)?;
                Ok(main_statement.clone())
            }
        }
    }

    pub fn prepared_with_main_statement_and_sql(
        &mut self,
        database: &Database,
        sql: &str,
    ) -> WCDBResult<Arc<PreparedStatement>> {
        if self.main_statement.is_none() {
            let cpp_obj =
                unsafe { WCDBRustHandle_getMainStatement(self.get_cpp_handle(database)?) };
            let mut prepared_statement = PreparedStatement::new(Some(cpp_obj));
            prepared_statement.auto_finalize = true;
            self.main_statement = Some(Arc::new(prepared_statement));
        }
        match self.main_statement.as_ref() {
            None => {
                Err(WCDBException::new_with_message(
                    ExceptionLevel::Error,
                    ExceptionCode::Error,
                    String::from("Method :prepared_with_main_statement_and_sql error, cause :main_statement is none"),
                ))
            }
            Some(statement) => {
                statement.prepare_with_sql(sql)?;
                Ok(statement.clone())
            }
        }
    }
}

pub struct Handle<'a> {
    handle_inner: Arc<RefCell<HandleInner>>,
    database: &'a Database,
    cpp_object: CppObject,
}

impl<'a> CppObjectConvertibleTrait for Handle<'a> {
    fn as_cpp_object(&self) -> &CppObject {
        self.cpp_object.as_cpp_object()
    }
}

impl<'a> CppObjectTrait for Handle<'a> {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        let mut handle_inner = self.handle_inner.borrow_mut();
        handle_inner.set_cpp_obj(cpp_obj);
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        let handle_inner_lock = self.handle_inner.borrow_mut();
        handle_inner_lock.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        let mut handle_inner = self.handle_inner.borrow_mut();
        handle_inner.release_cpp_object();
    }
}

impl<'a> HandleOperationTrait for Handle<'a> {
    fn get_handle(&self, _: bool) -> Handle {
        let cpp_obj = self.cpp_object.get_cpp_obj();
        Handle {
            handle_inner: self.handle_inner.clone(),
            database: self.database,
            cpp_object: CppObject::new(Some(cpp_obj)),
        }
    }

    fn auto_invalidate_handle(&self) -> bool {
        false
    }

    fn run_transaction<F: FnOnce(&Handle) -> bool>(&self, closure: F) -> WCDBResult<()> {
        let handle_orm_operation = { self.handle_inner.borrow().handle_orm_operation.clone() };
        handle_orm_operation.run_transaction(
            self.get_handle(true),
            self.auto_invalidate_handle(),
            closure,
        )
    }

    fn execute<T: StatementTrait>(&self, statement: &T) -> WCDBResult<()> {
        let handle_orm_operation = { self.handle_inner.borrow().handle_orm_operation.clone() };
        handle_orm_operation.execute(
            self.get_handle(true),
            self.auto_invalidate_handle(),
            statement,
        )
    }

    fn execute_sql(&self, sql: &str) -> WCDBResult<()> {
        let handle_orm_operation = { self.handle_inner.borrow().handle_orm_operation.clone() };
        handle_orm_operation.execute_sql(self.get_handle(true), self.auto_invalidate_handle(), sql)
    }
}

impl<'a> HandleORMOperationTrait for Handle<'a> {
    fn create_table<T, R: TableBinding<T>>(
        &self,
        table_name: &str,
        binding: &R,
    ) -> WCDBResult<bool> {
        let handle_orm_operation = { self.handle_inner.borrow().handle_orm_operation.clone() };
        handle_orm_operation.create_table(self.get_handle(true), table_name, binding)
    }

    fn table_exist(&self, table_name: &str) -> WCDBResult<bool> {
        let handle_orm_operation = { self.handle_inner.borrow().handle_orm_operation.clone() };
        handle_orm_operation.table_exist(
            self.get_handle(true),
            self.auto_invalidate_handle(),
            table_name,
        )
    }

    fn drop_table(&self, table_name: &str) -> WCDBResult<()> {
        let handle_orm_operation = { self.handle_inner.borrow().handle_orm_operation.clone() };
        handle_orm_operation.drop_table(
            self.get_handle(true),
            self.auto_invalidate_handle(),
            table_name,
        )
    }

    fn prepare_insert<T>(&self) -> Insert<T> {
        let handle_orm_operation = { self.handle_inner.borrow().handle_orm_operation.clone() };
        handle_orm_operation.prepare_insert(self.get_handle(true), self.auto_invalidate_handle())
    }

    fn prepare_update<T>(&self) -> Update<T> {
        let handle_orm_operation = { self.handle_inner.borrow().handle_orm_operation.clone() };
        handle_orm_operation.prepare_update(self.get_handle(true), self.auto_invalidate_handle())
    }

    fn prepare_select<T>(&self) -> Select<T> {
        let handle_orm_operation = { self.handle_inner.borrow().handle_orm_operation.clone() };
        handle_orm_operation.prepare_select(self.get_handle(true), self.auto_invalidate_handle())
    }

    fn prepare_delete(&self) -> Delete {
        let handle_orm_operation = { self.handle_inner.borrow().handle_orm_operation.clone() };
        handle_orm_operation.prepare_delete(self.get_handle(true), self.auto_invalidate_handle())
    }

    fn insert_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        let handle_orm_operation = { self.handle_inner.borrow().handle_orm_operation.clone() };
        handle_orm_operation.insert_object(
            self.get_handle(true),
            self.auto_invalidate_handle(),
            object,
            fields,
            table_name,
        )
    }

    fn insert_or_replace_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        let handle_orm_operation = { self.handle_inner.borrow().handle_orm_operation.clone() };
        handle_orm_operation.insert_or_replace_object(
            self.get_handle(true),
            self.auto_invalidate_handle(),
            object,
            fields,
            table_name,
        )
    }

    fn insert_or_ignore_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        let handle_orm_operation = { self.handle_inner.borrow().handle_orm_operation.clone() };
        handle_orm_operation.insert_or_ignore_object(
            self.get_handle(true),
            self.auto_invalidate_handle(),
            object,
            fields,
            table_name,
        )
    }

    fn insert_objects<T>(
        &self,
        objects: Vec<T>,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        let handle_orm_operation = { self.handle_inner.borrow().handle_orm_operation.clone() };
        handle_orm_operation.insert_objects(
            self.get_handle(true),
            self.auto_invalidate_handle(),
            objects,
            fields,
            table_name,
        )
    }

    fn insert_or_replace_objects<T>(
        &self,
        objects: Vec<T>,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        let handle_orm_operation = { self.handle_inner.borrow().handle_orm_operation.clone() };
        handle_orm_operation.insert_or_replace_objects(
            self.get_handle(true),
            self.auto_invalidate_handle(),
            objects,
            fields,
            table_name,
        )
    }

    fn insert_or_ignore_objects<T>(
        &self,
        objects: Vec<T>,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        let handle_orm_operation = { self.handle_inner.borrow().handle_orm_operation.clone() };
        handle_orm_operation.insert_or_ignore_objects(
            self.get_handle(true),
            self.auto_invalidate_handle(),
            objects,
            fields,
            table_name,
        )
    }

    fn delete_objects(
        &self,
        table_name: &str,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()> {
        let handle_orm_operation = { self.handle_inner.borrow().handle_orm_operation.clone() };
        handle_orm_operation.delete_objects(
            self.get_handle(true),
            self.auto_invalidate_handle(),
            table_name,
            condition_opt,
            order_opt,
            limit_opt,
            offset_opt,
        )
    }

    fn update_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()> {
        let handle_orm_operation = { self.handle_inner.borrow().handle_orm_operation.clone() };
        handle_orm_operation.update_object(
            self.get_handle(true),
            self.auto_invalidate_handle(),
            object,
            fields,
            table_name,
            condition_opt,
            order_opt,
            limit_opt,
            offset_opt,
        )
    }

    fn get_first_object<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<Option<T>> {
        let handle_orm_operation = { self.handle_inner.borrow().handle_orm_operation.clone() };
        handle_orm_operation.get_first_object(
            self.get_handle(true),
            self.auto_invalidate_handle(),
            fields,
            table_name,
            condition_opt,
            order_opt,
            offset_opt,
        )
    }

    fn get_all_objects<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<Vec<T>> {
        let handle_orm_operation = { self.handle_inner.borrow().handle_orm_operation.clone() };
        handle_orm_operation.get_all_objects(
            self.get_handle(true),
            self.auto_invalidate_handle(),
            fields,
            table_name,
            condition_opt,
            order_opt,
            limit_opt,
            offset_opt,
        )
    }
}

impl<'a> Handle<'a> {
    pub fn new(database: &'a Database, write_hint: bool) -> Self {
        let handle_inner = Arc::new(RefCell::new(HandleInner {
            handle_orm_operation: HandleORMOperation::new(None),
            main_statement: None,
            write_hint,
        }));
        let cpp_obj = handle_inner.borrow().get_cpp_obj();
        Self {
            handle_inner,
            database,
            cpp_object: CppObject::new(Some(cpp_obj)),
        }
    }

    pub fn new_with_obj(cpp_obj: *mut c_void, database: &'a Database) -> Self {
        let handle_inner = Arc::new(RefCell::new(HandleInner {
            handle_orm_operation: HandleORMOperation::new(Some(cpp_obj)),
            main_statement: None,
            write_hint: false,
        }));
        Self {
            handle_inner,
            database,
            cpp_object: CppObject::new(Some(cpp_obj)),
        }
    }

    pub fn get_cpp_handle(&self) -> WCDBResult<*mut c_void> {
        let mut handle_inner = self.handle_inner.borrow_mut();
        handle_inner.get_cpp_handle(self.database)
    }

    pub fn create_exception(&self) -> WCDBException {
        WCDBException::create_exception(unsafe { WCDBRustHandle_getError(self.get_cpp_obj()) })
    }

    pub fn invalidate(&self) {
        let mut handle_inner = self.handle_inner.borrow_mut();
        handle_inner.invalidate();
    }

    pub fn close(&self) {
        self.invalidate();
    }

    pub fn get_changes(&self) -> WCDBResult<i32> {
        let mut handle_inner = self.handle_inner.borrow_mut();
        handle_inner.get_changes(self.database)
    }

    pub fn get_last_inserted_row_id(&self) -> WCDBResult<i64> {
        Ok(unsafe { WCDBRustHandle_getLastInsertRowid(self.get_cpp_handle()?) })
    }

    pub fn prepared_with_main_statement<T: StatementTrait>(
        &self,
        statement: &T,
    ) -> WCDBResult<Arc<PreparedStatement>> {
        let mut handle_inner = self.handle_inner.borrow_mut();
        handle_inner.prepared_with_main_statement(self.database, statement)
    }

    pub fn prepared_with_main_statement_and_sql(
        &self,
        sql: &str,
    ) -> WCDBResult<Arc<PreparedStatement>> {
        let mut handle_inner = self.handle_inner.borrow_mut();
        handle_inner.prepared_with_main_statement_and_sql(self.database, sql)
    }

    pub fn table_exist(cpp_obj: *mut c_void, table_name: &str) -> i32 {
        let c_table_name = CString::new(table_name).unwrap_or_default();
        unsafe { WCDBRustHandle_tableExist(cpp_obj, c_table_name.as_ptr()) }
    }

    pub fn execute_inner<T: StatementTrait>(cpp_obj: *mut c_void, statement: &T) -> bool {
        unsafe { WCDBRustHandle_execute(cpp_obj, CppObject::get(statement)) }
    }
    pub fn execute_sql(cpp_obj: *mut c_void, sql: &str) -> bool {
        let c_sql = CString::new(sql).unwrap_or_default();
        unsafe { WCDBRustHandle_executeSQL(cpp_obj, c_sql.as_ptr()) }
    }
}
