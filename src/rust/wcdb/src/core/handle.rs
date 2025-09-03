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

pub struct Handle<'a> {
    handle_orm_operation: HandleORMOperation,
    main_statement: RefCell<Option<Arc<PreparedStatement>>>,
    write_hint: RefCell<bool>,
    database: &'a Database,
}

impl<'a> CppObjectConvertibleTrait for Handle<'a> {
    fn as_cpp_object(&self) -> &CppObject {
        self.handle_orm_operation.as_cpp_object()
    }
}

impl<'a> CppObjectTrait for Handle<'a> {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.handle_orm_operation.set_cpp_obj(cpp_obj)
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.handle_orm_operation.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.handle_orm_operation.release_cpp_object()
    }
}

impl<'a> HandleOperationTrait for Handle<'a> {
    fn get_handle(&self, _: bool) -> Handle {
        Handle {
            handle_orm_operation: self.handle_orm_operation.clone(),
            main_statement: self.main_statement.clone(),
            write_hint: self.write_hint.clone(),
            database: self.database,
        }
    }

    fn auto_invalidate_handle(&self) -> bool {
        false
    }

    fn run_transaction<F: FnOnce(&Handle) -> bool>(&self, closure: F) -> WCDBResult<()> {
        self.handle_orm_operation.run_transaction(closure)
    }

    fn execute<T: StatementTrait>(&self, statement: &T) -> WCDBResult<()> {
        self.handle_orm_operation.execute(statement)
    }

    fn execute_sql(&self, sql: &str) -> WCDBResult<()> {
        self.handle_orm_operation.execute_sql(sql)
    }
}

impl<'a> HandleORMOperationTrait for Handle<'a> {
    fn create_table<T, R: TableBinding<T>>(
        &self,
        table_name: &str,
        binding: &R,
    ) -> WCDBResult<bool> {
        self.handle_orm_operation.create_table(table_name, binding)
    }

    fn table_exist(&self, table_name: &str) -> WCDBResult<bool> {
        self.handle_orm_operation.table_exist(table_name)
    }

    fn drop_table(&self, table_name: &str) -> WCDBResult<()> {
        self.handle_orm_operation.drop_table(table_name)
    }

    fn prepare_insert<T>(&self) -> Insert<T> {
        self.handle_orm_operation.prepare_insert()
    }

    fn prepare_update<T>(&self) -> Update<T> {
        self.handle_orm_operation.prepare_update()
    }

    fn prepare_select<T>(&self) -> Select<T> {
        self.handle_orm_operation.prepare_select()
    }

    fn prepare_delete(&self) -> Delete {
        self.handle_orm_operation.prepare_delete()
    }

    fn insert_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.handle_orm_operation
            .insert_object(object, fields, table_name)
    }

    fn insert_or_replace_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.handle_orm_operation
            .insert_or_replace_object(object, fields, table_name)
    }

    fn insert_or_ignore_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.handle_orm_operation
            .insert_or_ignore_object(object, fields, table_name)
    }

    fn insert_objects<T>(
        &self,
        objects: Vec<T>,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.handle_orm_operation
            .insert_objects(objects, fields, table_name)
    }

    fn insert_or_replace_objects<T>(
        &self,
        objects: Vec<T>,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.handle_orm_operation
            .insert_or_replace_objects(objects, fields, table_name)
    }

    fn insert_or_ignore_objects<T>(
        &self,
        objects: Vec<T>,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.handle_orm_operation
            .insert_or_ignore_objects(objects, fields, table_name)
    }

    fn delete_objects(
        &self,
        table_name: &str,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()> {
        self.handle_orm_operation.delete_objects(
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
        self.handle_orm_operation.update_object(
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
        self.handle_orm_operation.get_first_object(
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
        self.handle_orm_operation.get_all_objects(
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
        Self {
            handle_orm_operation: HandleORMOperation::new(None),
            main_statement: RefCell::new(None),
            write_hint: RefCell::new(write_hint),
            database,
        }
    }

    pub fn new_with_obj(cpp_obj: *mut c_void, database: &'a Database) -> Self {
        let mut this = Self {
            handle_orm_operation: HandleORMOperation::new(None),
            main_statement: RefCell::new(None),
            write_hint: RefCell::new(false),
            database,
        };
        this.set_cpp_obj(cpp_obj);
        this
    }

    pub fn get_cpp_handle(&self) -> WCDBResult<*mut c_void> {
        let mut cpp_obj = self.get_cpp_obj();
        if cpp_obj.is_null() {
            let handle = self.database.get_handle(*self.write_hint.borrow());
            cpp_obj = handle.get_cpp_obj();
            if cpp_obj.is_null() {
                return Err(self.database.create_exception());
            }
        }
        Ok(cpp_obj)
    }

    pub fn create_exception(&self) -> WCDBException {
        WCDBException::create_exception(unsafe { WCDBRustHandle_getError(self.get_cpp_obj()) })
    }

    pub fn invalidate(&mut self) {
        self.main_statement.take();
        let cpp_obj = self.get_cpp_obj();
        if !cpp_obj.is_null() {
            self.release_cpp_object();
            self.write_hint.take();
        }
    }

    pub fn close(&mut self) {
        self.invalidate();
    }

    pub fn get_changes(&self) -> WCDBResult<i32> {
        Ok(unsafe { WCDBRustHandle_getChanges(self.get_cpp_handle()?) })
    }

    pub fn get_last_inserted_row_id(&self) -> WCDBResult<i64> {
        Ok(unsafe { WCDBRustHandle_getLastInsertRowid(self.get_cpp_handle()?) })
    }

    pub fn prepared_with_main_statement<T: StatementTrait>(
        &self,
        statement: &T,
    ) -> WCDBResult<Arc<PreparedStatement>> {
        if self.main_statement.borrow().is_none() {
            let mut stat = PreparedStatement::new(Some(unsafe {
                WCDBRustHandle_getMainStatement(self.get_cpp_handle()?)
            }));
            stat.auto_finalize = true;
            self.main_statement.replace(Some(Arc::new(stat)));
        }
        match self.main_statement.borrow().as_ref() {
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
        &self,
        sql: &str,
    ) -> WCDBResult<Arc<PreparedStatement>> {
        if self.main_statement.borrow().is_none() {
            let mut stat = PreparedStatement::new(Some(unsafe {
                WCDBRustHandle_getMainStatement(self.get_cpp_handle()?)
            }));
            stat.auto_finalize = true;
            self.main_statement.replace(Some(Arc::new(stat)));
        }
        match self.main_statement.borrow().as_ref() {
            None => Err(WCDBException::new_with_message(
                ExceptionLevel::Error,
                ExceptionCode::Error,
                String::from(
                    "Method :prepared_with_main_statement error, cause :main_statement is none",
                ),
            )),
            Some(main_statement) => {
                main_statement.prepare_with_sql(sql)?;
                Ok(main_statement.clone())
            }
        }
    }

    pub fn table_exist(cpp_obj: *mut c_void, table_name: &str) -> i32 {
        let c_table_name = CString::new(table_name).unwrap_or_default();
        unsafe { WCDBRustHandle_tableExist(cpp_obj, c_table_name.as_ptr()) }
    }

    pub fn execute_inner<T: StatementTrait>(cpp_obj: *mut c_void, statement: &T) -> bool {
        unsafe { WCDBRustHandle_execute(cpp_obj, CppObject::get(statement)) }
    }

    pub fn execute<T: StatementTrait>(&self, statement: &T) -> WCDBResult<()> {
        let mut handle = self.get_handle(statement.is_write_statement());
        let mut exception_opt = None;
        if !unsafe { WCDBRustHandle_execute(handle.get_cpp_handle()?, CppObject::get(statement)) } {
            exception_opt = Some(handle.create_exception());
        }
        if self.auto_invalidate_handle() {
            handle.invalidate();
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
