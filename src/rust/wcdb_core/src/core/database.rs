use crate::base::cpp_object::CppObjectTrait;
use crate::base::wcdb_exception::{WCDBException, WCDBResult};
use crate::chaincall::delete::Delete;
use crate::chaincall::insert::Insert;
use crate::chaincall::select::Select;
use crate::chaincall::update::Update;
use crate::core::handle::Handle;
use crate::core::handle_operation::HandleOperationTrait;
use crate::core::handle_orm_operation::{HandleORMOperation, HandleORMOperationTrait};
use crate::core::table::Table;
use crate::orm::field::Field;
use crate::orm::table_binding::TableBinding;
use crate::utils::ToCow;
use crate::winq::expression::Expression;
use crate::winq::ordering_term::OrderingTerm;
use crate::winq::statement::StatementTrait;
use crate::winq::statement_drop_table::StatementDropTable;
use lazy_static::lazy_static;
use std::ffi::{c_char, c_double, c_void, CStr, CString};
use std::ptr::null_mut;
use std::sync::{Arc, Mutex};

// 定义性能跟踪回调的特性
pub trait TracePerformanceCallbackTrait:
Fn(/*tag*/i64, /*path*/String, /*handleId*/i64, /*sql*/String, /*info*/PerformanceInfo) + Send
{}
pub type TracePerformanceCallback = Box<dyn TracePerformanceCallbackTrait>;
impl<T> TracePerformanceCallbackTrait for T where
    T: Fn(
            /*tag*/ i64,
            /*path*/ String,
            /*handleId*/ i64,
            /*sql*/ String,
            /*info*/ PerformanceInfo,
        ) + Send
{
}

// 定义 sql 执行回调的特性
pub trait TraceSqlCallbackTrait: Fn(/*tag*/i64, /*path*/String, /*handleId*/i64, /*sql*/String, /*info*/String) + Send {}
pub type TraceSqlCallback = Box<dyn TraceSqlCallbackTrait>;
impl<T> TraceSqlCallbackTrait for T where
    T: Fn(
            /*tag*/ i64,
            /*path*/ String,
            /*handleId*/ i64,
            /*sql*/ String,
            /*info*/ String,
        ) + Send
{
}

// 定义异常回调的特性
pub trait TraceExceptionCallbackTrait: Fn(WCDBException) + Send {}
pub type TraceExceptionCallback = Box<dyn TraceExceptionCallbackTrait>;
impl<T> TraceExceptionCallbackTrait for T where T: Fn(WCDBException) + Send {}

// 定义损坏检测回调的特性
pub trait CorruptionNotificationTrait: Fn(Database) + Send {}
impl<T> CorruptionNotificationTrait for T where T: Fn(Database) + Send {}
pub type CorruptionNotificationCallback = Box<dyn CorruptionNotificationTrait>;

pub trait BackupFilterTrait {
    fn table_should_be_backup(&self, table_name: &str) -> bool;
}

// 定义备份回调的特性
pub trait BackupFilterCallbackTrait: Fn(&str) -> bool + Send {}
impl<T> BackupFilterCallbackTrait for T where T: Fn(&str) -> bool + Send {}
pub type BackupFilterCallback = Box<dyn BackupFilterCallbackTrait>;

// return True to continue current operation.
pub trait ProgressMonitorTrait: Fn(f64, f64) -> bool + Send {}
impl<T> ProgressMonitorTrait for T where T: Fn(f64, f64) -> bool + Send {}
pub type ProgressMonitorTraitCallback = Box<dyn ProgressMonitorTrait>;

// 定义一个全局静态变量来存储闭包
lazy_static! {
    static ref GLOBAL_TRACE_PERFORMANCE_CALLBACK: Arc<Mutex<Option<TracePerformanceCallback>>> =
        Arc::new(Mutex::new(None));
    static ref GLOBAL_TRACE_SQL_CALLBACK: Arc<Mutex<Option<TraceSqlCallback>>> =
        Arc::new(Mutex::new(None));
    static ref DATABASE_TRACE_SQL_CALLBACK: Arc<Mutex<Option<TraceSqlCallback>>> =
        Arc::new(Mutex::new(None));
    static ref GLOBAL_TRACE_EXCEPTION_CALLBACK: Arc<Mutex<Option<TraceExceptionCallback>>> =
        Arc::new(Mutex::new(None));
    static ref DATABASE_TRACE_EXCEPTION_CALLBACK: Arc<Mutex<Option<TraceExceptionCallback>>> =
        Arc::new(Mutex::new(None));
    static ref GLOBAL_CORRUPTION_NOTIFICATION_CALLBACK: Arc<Mutex<Option<CorruptionNotificationCallback>>> =
        Arc::new(Mutex::new(None));
    static ref GLOBAL_BACKUP_FILTER_CALLBACK: Arc<Mutex<Option<BackupFilterCallback>>> =
        Arc::new(Mutex::new(None));
    static ref GLOBAL_PROGRESS_MONITOR_TRAIT_CALLBACK: Arc<Mutex<Option<ProgressMonitorTraitCallback>>> =
        Arc::new(Mutex::new(None));
    static ref GLOBAL_VACUUM_PROGRESS_MONITOR_TRAIT_CALLBACK: Arc<Mutex<Option<ProgressMonitorTraitCallback>>> =
        Arc::new(Mutex::new(None));
}

pub type DatabaseCloseCallback = extern "C" fn(context: *mut c_void);

extern "C" {
    pub fn WCDBRustCore_createDatabase(path: *const c_char) -> *mut c_void;
    pub fn WCDBRustDatabase_getPath(cpp_obj: *mut c_void) -> *const c_char;

    pub fn WCDBRustDatabase_removeFiles(cpp_obj: *mut c_void) -> bool;

    pub fn WCDBRustDatabase_close(
        cpp_obj: *mut c_void,
        context: *mut c_void,
        cb: DatabaseCloseCallback,
    );

    pub fn WCDBRustDatabase_blockade(cpp_obj: *mut c_void);

    pub fn WCDBRustDatabase_unblockade(cpp_obj: *mut c_void);

    pub fn WCDBRustDatabase_isBlockaded(cpp_obj: *mut c_void) -> bool;
    pub fn WCDBRustDatabase_canOpen(cpp_obj: *mut c_void) -> bool;

    pub fn WCDBRustDatabase_isOpened(cpp_obj: *mut c_void) -> bool;

    pub fn WCDBRustDatabase_getHandle(cpp_obj: *mut c_void, write_hint: bool) -> *mut c_void;

    pub fn WCDBRustDatabase_vacuum(cpp_obj: *mut c_void, monitor: *const c_void) -> bool;

    pub fn WCDBRustDatabase_enableAutoVacuum(cpp_obj: *mut c_void, incremental: bool);

    pub fn WCDBRustDatabase_incrementalVacuum(cpp_obj: *mut c_void, pages: i32) -> bool;

    pub fn WCDBRustDatabase_getError(cpp_obj: *mut c_void) -> *mut c_void;

    pub fn WCDBRustDatabase_globalTracePerformance(
        global_trace_performance_callback: extern "C" fn(
            i64,
            *const c_char,
            i64,
            *const c_char,
            PerformanceInfo,
        ),
    );

    pub fn WCDBRustDatabase_globalTraceSQL(
        global_trace_sql_callback: extern "C" fn(
            i64,
            *const c_char,
            i64,
            *const c_char,
            *const c_char,
        ),
    );

    pub fn WCDBRustDatabase_traceSQL(
        cpp_obj: *mut c_void,
        trace_sql_callback: extern "C" fn(i64, *const c_char, i64, *const c_char, *const c_char),
    );

    pub fn WCDBRustDatabase_globalTraceException(
        global_trace_exception_callback: extern "C" fn(*mut c_void),
    );

    pub fn WCDBRustDatabase_traceException(
        cpp_obj: *mut c_void,
        trace_exception_callback: extern "C" fn(*mut c_void),
    );

    pub fn WCDBRustDatabase_getTag(cpp_obj: *mut c_void) -> *mut c_void;

    pub fn WCDBRustDatabase_setTag(cpp_obj: *mut c_void, tag: i64);

    pub fn WCDBRustDatabase_setNotificationWhenCorrupted(
        cpp_obj: *mut c_void,
        global_corruption_notification: *mut c_void,
    );

    pub fn WCDBRustDatabase_checkIfCorrupted(cpp_obj: *mut c_void) -> bool;

    pub fn WCDBRustDatabase_checkIfIsAlreadyCorrupted(cpp_obj: *mut c_void) -> bool;

    pub fn WCDBRustDatabase_enableAutoBackup(cpp_obj: *mut c_void, enable: bool);

    pub fn WCDBRustDatabase_backup(cpp_obj: *mut c_void) -> bool;

    pub fn WCDBRustDatabase_filterBackup(cpp_obj: *mut c_void, filter: *const c_void);

    pub fn WCDBRustDatabase_retrieve(cpp_obj: *mut c_void, monitor: *const c_void) -> c_double;

    pub fn WCDBRustDatabase_deposit(cpp_obj: *mut c_void) -> bool;

    pub fn WCDBRustDatabase_removeDepositedFiles(cpp_obj: *mut c_void) -> bool;

    pub fn WCDBRustDatabase_containDepositedFiles(cpp_obj: *mut c_void) -> bool;
    pub fn WCDBRustDatabase_truncateCheckpoint(cpp_obj: *mut c_void) -> bool;

    pub fn WCDBRustDatabase_setAutoCheckpointEnable(cpp_obj: *mut c_void, enable: bool);
}

extern "C" fn close_callback_wrapper(context: *mut c_void) {
    if !context.is_null() {
        let boxed_cb: Box<Box<dyn FnOnce()>> = unsafe { Box::from_raw(context as *mut _) };
        boxed_cb();
    }
}

extern "C" fn global_trace_performance_callback(
    tag: i64,
    path: *const c_char,
    handle_id: i64,
    sql: *const c_char,
    info: PerformanceInfo,
) {
    if let Some(callback) = &*GLOBAL_TRACE_PERFORMANCE_CALLBACK.lock().unwrap() {
        callback(
            tag,
            path.to_cow().to_string(),
            handle_id,
            sql.to_cow().to_string(),
            info,
        );
    }
}

extern "C" fn global_trace_sql_callback(
    tag: i64,
    path: *const c_char,
    handle_id: i64,
    sql: *const c_char,
    info: *const c_char,
) {
    if let Some(callback) = &*GLOBAL_TRACE_SQL_CALLBACK.lock().unwrap() {
        callback(
            tag,
            path.to_cow().to_string(),
            handle_id,
            sql.to_cow().to_string(),
            info.to_cow().to_string(),
        );
    }
}

extern "C" fn trace_sql_callback(
    tag: i64,
    path: *const c_char,
    handle_id: i64,
    sql: *const c_char,
    info: *const c_char,
) {
    if let Some(callback) = &*DATABASE_TRACE_SQL_CALLBACK.lock().unwrap() {
        callback(
            tag,
            path.to_cow().to_string(),
            handle_id,
            sql.to_cow().to_string(),
            info.to_cow().to_string(),
        );
    }
}

extern "C" fn global_trace_exception_callback(exp_cpp_obj: *mut c_void) {
    if let Some(callback) = &*GLOBAL_TRACE_EXCEPTION_CALLBACK.lock().unwrap() {
        let ex = WCDBException::create_exception(exp_cpp_obj);
        callback(ex);
    }
}

extern "C" fn trace_exception_callback(exp_cpp_obj: *mut c_void) {
    if let Some(callback) = &*DATABASE_TRACE_EXCEPTION_CALLBACK.lock().unwrap() {
        let ex = WCDBException::create_exception(exp_cpp_obj);
        callback(ex);
    }
}

extern "C" fn global_corruption_notification_callback_wrapper(cpp_obj: *mut c_void) {
    if let Some(callback) = &*GLOBAL_CORRUPTION_NOTIFICATION_CALLBACK.lock().unwrap() {
        let database = Database::from(cpp_obj);
        callback(database);
    }
}

extern "C" fn backup_filter_callback_wrapper(table_name: *const c_char) -> bool {
    if let Some(callback) = &*GLOBAL_BACKUP_FILTER_CALLBACK.lock().unwrap() {
        let cstr = unsafe { CStr::from_ptr(table_name) };
        let table = cstr.to_str().unwrap();
        return callback(table);
    }
    return false;
}

// True to continue current operation.
extern "C" fn retrieve_progress_monitor_trait_wrapper(
    percentage: c_double,
    increment: c_double,
) -> bool {
    if let Some(callback) = &*GLOBAL_PROGRESS_MONITOR_TRAIT_CALLBACK.lock().unwrap() {
        return callback(percentage as f64, increment as f64);
    }
    return false;
}

// True to continue current operation.
extern "C" fn vacuum_progress_monitor_trait_wrapper(
    percentage: c_double,
    increment: c_double,
) -> bool {
    if let Some(callback) = &*GLOBAL_VACUUM_PROGRESS_MONITOR_TRAIT_CALLBACK
        .lock()
        .unwrap()
    {
        return callback(percentage as f64, increment as f64);
    }
    return false;
}

#[derive(Clone)]
pub struct Database {
    handle_orm_operation: HandleORMOperation,
    close_callback: Arc<Mutex<Option<Box<dyn FnOnce() + Send>>>>,
}

unsafe impl Send for Database {}

unsafe impl Sync for Database {}

impl CppObjectTrait for Database {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.handle_orm_operation.set_cpp_obj(cpp_obj)
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.handle_orm_operation.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.handle_orm_operation.release_cpp_object();
    }
}

impl HandleOperationTrait for Database {
    fn get_handle(&self, write_hint: bool) -> Handle {
        Handle::new(self, write_hint)
    }

    fn auto_invalidate_handle(&self) -> bool {
        true
    }

    fn run_transaction<F: FnOnce(Handle) -> bool>(&self, callback: F) -> WCDBResult<()> {
        let handle = self.get_handle(true);
        handle.run_transaction(callback)
    }
}

impl HandleORMOperationTrait for Database {
    fn create_table<T, R: TableBinding<T>>(
        &self,
        table_name: &str,
        binding: &R,
    ) -> WCDBResult<bool> {
        let handle = self.get_handle(true);
        binding.base_binding().create_table(table_name, handle)
    }

    fn drop_table(&self, table_name: &str) -> WCDBResult<()> {
        let statement = StatementDropTable::new();
        self.execute(statement.drop_table(table_name).if_exist())
    }

    fn insert_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.prepare_insert::<T>()
            .into_table(table_name)
            .value(object)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    fn insert_or_replace_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.prepare_insert::<T>()
            .or_replace()
            .into_table(table_name)
            .value(object)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    fn insert_or_ignore_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.prepare_insert::<T>()
            .or_ignore()
            .into_table(table_name)
            .value(object)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    fn insert_objects<T>(
        &self,
        objects: Vec<T>,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.prepare_insert::<T>()
            .into_table(table_name)
            .values(objects)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    fn insert_or_replace_objects<T>(
        &self,
        objects: Vec<T>,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.prepare_insert::<T>()
            .or_replace()
            .into_table(table_name)
            .values(objects)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    fn insert_or_ignore_objects<T>(
        &self,
        objects: Vec<T>,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.prepare_insert::<T>()
            .or_ignore()
            .into_table(table_name)
            .values(objects)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    fn prepare_insert<T>(&self) -> Insert<T> {
        Insert::new(self.get_handle(true), false, self.auto_invalidate_handle())
    }

    fn prepare_update<T>(&self) -> Update<T> {
        Update::new(self.get_handle(true), false, self.auto_invalidate_handle())
    }

    fn prepare_select<T>(&self) -> Select<T> {
        Select::new(self.get_handle(false), false, self.auto_invalidate_handle())
    }

    fn prepare_delete(&self) -> Delete {
        Delete::new(self.get_handle(true), false, self.auto_invalidate_handle())
    }

    fn delete_objects(&self, table_name: &str) -> WCDBResult<()> {
        self.prepare_delete().from_table(table_name).execute()?;
        Ok(())
    }

    fn delete_objects_by_expression(
        &self,
        table_name: &str,
        expression: Expression,
    ) -> WCDBResult<()> {
        self.prepare_delete()
            .from_table(table_name)
            .where_expression(expression)
            .execute()?;
        Ok(())
    }

    fn delete_objects_by_expression_order_limit(
        &self,
        table_name: &str,
        expression: Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()> {
        self.prepare_delete()
            .from_table(table_name)
            .where_expression(expression)
            .order_by(&vec![order])
            .limit(limit)
            .execute()?;
        Ok(())
    }

    fn delete_objects_by_expression_order_limit_offset(
        &self,
        table_name: &str,
        expression: Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()> {
        self.prepare_delete()
            .from_table(table_name)
            .where_expression(expression)
            .order_by(&vec![order])
            .limit(limit)
            .offset(offset)
            .execute()?;
        Ok(())
    }

    fn delete_objects_by_order_limit(
        &self,
        table_name: &str,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()> {
        self.prepare_delete()
            .from_table(table_name)
            .order_by(&vec![order])
            .limit(limit)
            .execute()?;
        Ok(())
    }

    fn delete_objects_by_order_limit_offset(
        &self,
        table_name: &str,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()> {
        self.prepare_delete()
            .from_table(table_name)
            .order_by(&vec![order])
            .limit(limit)
            .offset(offset)
            .execute()?;
        Ok(())
    }

    fn update_object_by_field<T>(
        &self,
        object: T,
        field: &Field<T>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.prepare_update::<T>()
            .table(table_name)
            .set(vec![field])
            .to_object(object)
            .execute()?;
        Ok(())
    }

    fn update_object_by_field_expression<T>(
        &self,
        object: T,
        field: &Field<T>,
        table_name: &str,
        expression: Expression,
    ) -> WCDBResult<()> {
        self.prepare_update::<T>()
            .table(table_name)
            .set(vec![field])
            .to_object(object)
            .where_expression(expression)
            .execute()?;
        Ok(())
    }

    fn update_object_by_field_expression_order_limit<T>(
        &self,
        object: T,
        field: &Field<T>,
        table_name: &str,
        expression: Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()> {
        self.prepare_update::<T>()
            .table(table_name)
            .set(vec![field])
            .to_object(object)
            .where_expression(expression)
            .order_by(&vec![order])
            .limit(limit)
            .execute()?;
        Ok(())
    }

    fn update_object_by_field_expression_order_limit_offset<T>(
        &self,
        object: T,
        field: &Field<T>,
        table_name: &str,
        expression: Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()> {
        self.prepare_update::<T>()
            .table(table_name)
            .set(vec![field])
            .to_object(object)
            .where_expression(expression)
            .order_by(&vec![order])
            .limit(limit)
            .offset(offset)
            .execute()?;
        Ok(())
    }

    fn update_object_by_field_order_limit<T>(
        &self,
        object: T,
        field: &Field<T>,
        table_name: &str,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()> {
        self.prepare_update::<T>()
            .table(table_name)
            .set(vec![field])
            .to_object(object)
            .order_by(&vec![order])
            .limit(limit)
            .execute()?;
        Ok(())
    }

    fn update_object_by_field_order_limit_offset<T>(
        &self,
        object: T,
        field: &Field<T>,
        table_name: &str,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()> {
        self.prepare_update::<T>()
            .table(table_name)
            .set(vec![field])
            .to_object(object)
            .order_by(&vec![order])
            .limit(limit)
            .offset(offset)
            .execute()?;
        Ok(())
    }

    fn update_object_by_fields<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.prepare_update::<T>()
            .table(table_name)
            .set(fields)
            .to_object(object)
            .execute()?;
        Ok(())
    }

    fn update_object_by_fields_expression<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
        expression: Expression,
    ) -> WCDBResult<()> {
        self.prepare_update::<T>()
            .table(table_name)
            .set(fields)
            .to_object(object)
            .where_expression(expression)
            .execute()?;
        Ok(())
    }

    fn update_object_by_fields_expression_order_limit<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
        expression: Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()> {
        self.prepare_update::<T>()
            .table(table_name)
            .set(fields)
            .to_object(object)
            .where_expression(expression)
            .order_by(&vec![order])
            .limit(limit)
            .execute()?;
        Ok(())
    }

    fn update_object_by_fields_expression_order_limit_offset<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
        expression: Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()> {
        self.prepare_update::<T>()
            .table(table_name)
            .set(fields)
            .to_object(object)
            .where_expression(expression)
            .order_by(&vec![order])
            .limit(limit)
            .offset(offset)
            .execute()?;
        Ok(())
    }

    fn update_object_by_fields_order_limit<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()> {
        self.prepare_update::<T>()
            .table(table_name)
            .set(fields)
            .to_object(object)
            .order_by(&vec![order])
            .limit(limit)
            .execute()?;
        Ok(())
    }

    fn update_object_by_fields_order_limit_offset<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()> {
        self.prepare_update::<T>()
            .table(table_name)
            .set(fields)
            .to_object(object)
            .order_by(&vec![order])
            .limit(limit)
            .offset(offset)
            .execute()?;
        Ok(())
    }

    fn get_first_object<T>(&self, fields: Vec<&Field<T>>, table_name: &str) -> WCDBResult<T> {
        self.prepare_select()
            .select(fields)
            .from(table_name)
            .first_object()
    }

    fn get_first_object_by_expression<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        expression: Expression,
    ) -> WCDBResult<T> {
        self.prepare_select()
            .select(fields)
            .from(table_name)
            .where_expression(expression)
            .first_object()
    }

    fn get_first_object_by_table_name_expression<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition: Expression,
    ) -> WCDBResult<T> {
        self.prepare_select()
            .select(fields)
            .from(table_name)
            .where_expression(condition)
            .first_object()
    }

    fn get_first_object_by_table_name_expression_order<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition: Expression,
        order: OrderingTerm,
    ) -> WCDBResult<T> {
        self.prepare_select()
            .select(fields)
            .from(table_name)
            .where_expression(condition)
            .order_by(order)
            .first_object()
    }

    fn get_first_object_by_table_name_expression_order_offset<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition: Expression,
        order: OrderingTerm,
        offset: i64,
    ) -> WCDBResult<T> {
        self.prepare_select()
            .select(fields)
            .from(table_name)
            .where_expression(condition)
            .order_by(order)
            .limit(1)
            .offset(offset)
            .first_object()
    }

    fn get_first_object_by_table_name_order<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        order: OrderingTerm,
    ) -> WCDBResult<T> {
        self.prepare_select()
            .select(fields)
            .from(table_name)
            .order_by(order)
            .first_object()
    }

    fn get_first_object_by_table_name_order_offset<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        order: OrderingTerm,
        offset: i64,
    ) -> WCDBResult<T> {
        self.prepare_select()
            .select(fields)
            .from(table_name)
            .order_by(order)
            .limit(1)
            .offset(offset)
            .first_object()
    }

    fn get_all_objects<T>(&self, fields: Vec<&Field<T>>, table_name: &str) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(fields)
            .from(table_name)
            .all_objects()
    }

    fn get_all_objects_by_table_name_expression<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition: Expression,
    ) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(fields)
            .from(table_name)
            .where_expression(condition)
            .all_objects()
    }

    fn get_all_objects_by_table_name_expression_order<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition: Expression,
        order: OrderingTerm,
    ) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(fields)
            .from(table_name)
            .where_expression(condition)
            .order_by(order)
            .all_objects()
    }

    fn get_all_objects_by_table_name_expression_order_limit<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition: Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(fields)
            .from(table_name)
            .where_expression(condition)
            .order_by(order)
            .limit(limit)
            .all_objects()
    }

    fn get_all_objects_by_table_name_expression_order_limit_offset<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition: Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(fields)
            .from(table_name)
            .where_expression(condition)
            .order_by(order)
            .limit(limit)
            .offset(offset)
            .all_objects()
    }

    fn get_all_objects_by_table_name_order<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        order: OrderingTerm,
    ) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(fields)
            .from(table_name)
            .order_by(order)
            .all_objects()
    }

    fn get_all_objects_by_table_name_order_limit<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(fields)
            .from(table_name)
            .order_by(order)
            .limit(limit)
            .all_objects()
    }

    fn get_all_objects_by_table_name_order_limit_offset<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(fields)
            .from(table_name)
            .order_by(order)
            .limit(limit)
            .offset(offset)
            .all_objects()
    }
}

impl Database {
    pub fn new(path: &str) -> Self {
        let c_path = CString::new(path).unwrap_or_default();
        let cpp_obj = unsafe { WCDBRustCore_createDatabase(c_path.as_ptr()) };
        Database {
            handle_orm_operation: HandleORMOperation::new_with_obj(cpp_obj),
            close_callback: Arc::new(Mutex::new(None)),
        }
    }

    pub fn from(cpp_obj: *mut c_void) -> Self {
        Database {
            handle_orm_operation: HandleORMOperation::new_with_obj(cpp_obj),
            close_callback: Arc::new(Mutex::new(None)),
        }
    }

    pub fn get_path(&self) -> String {
        let path = unsafe { WCDBRustDatabase_getPath(self.get_cpp_obj()) };
        path.to_cow().to_string()
    }

    pub fn remove_files(&self) -> WCDBResult<()> {
        let ret: bool = unsafe { WCDBRustDatabase_removeFiles(self.get_cpp_obj()) };
        if ret {
            Ok(())
        } else {
            Err(self.create_exception())
        }
    }

    pub fn can_open(&self) -> bool {
        unsafe { WCDBRustDatabase_canOpen(self.get_cpp_obj()) }
    }

    pub fn is_opened(&self) -> bool {
        unsafe { WCDBRustDatabase_isOpened(self.get_cpp_obj()) }
    }

    pub fn get_table<'a, T, R: TableBinding<T>>(
        &'a self,
        table_name: &str,
        binding: &'a R,
    ) -> Arc<Table<'a, T, R>> {
        assert!(!table_name.is_empty());
        Arc::new(Table::new(table_name, binding, self))
    }

    pub fn close<CB>(&self, cb_opt: Option<CB>)
    where
        CB: FnOnce() + Send + 'static,
    {
        match cb_opt {
            None => unsafe {
                WCDBRustDatabase_close(self.get_cpp_obj(), null_mut(), close_callback_wrapper)
            },
            Some(cb) => {
                let boxed_cb: Box<Box<dyn FnOnce()>> = Box::new(Box::new(cb));
                let context = Box::into_raw(boxed_cb) as *mut c_void;
                unsafe {
                    WCDBRustDatabase_close(self.get_cpp_obj(), context, close_callback_wrapper)
                }
            }
        }
    }

    pub fn blockade(&self) {
        unsafe { WCDBRustDatabase_blockade(self.get_cpp_obj()) }
    }

    pub fn un_blockade(&self) {
        unsafe { WCDBRustDatabase_unblockade(self.get_cpp_obj()) }
    }

    pub fn is_blockaded(&self) -> bool {
        unsafe { WCDBRustDatabase_isBlockaded(self.get_cpp_obj()) }
    }

    pub(crate) fn get_handle_raw(cpp_obj: *mut c_void, write_hint: bool) -> *mut c_void {
        unsafe { WCDBRustDatabase_getHandle(cpp_obj, write_hint) }
    }

    pub fn vacuum<CB>(&self, monitor: Option<CB>) -> WCDBResult<()>
    where
        CB: ProgressMonitorTrait + 'static,
    {
        let mut ret: bool = false;
        match monitor {
            None => {
                *GLOBAL_VACUUM_PROGRESS_MONITOR_TRAIT_CALLBACK
                    .lock()
                    .unwrap() = None;
                ret = unsafe { WCDBRustDatabase_vacuum(self.get_cpp_obj(), null_mut()) };
            }
            Some(cb) => {
                let callback_box = Box::new(cb) as ProgressMonitorTraitCallback;
                *GLOBAL_VACUUM_PROGRESS_MONITOR_TRAIT_CALLBACK
                    .lock()
                    .unwrap() = Some(callback_box);
                ret = unsafe {
                    WCDBRustDatabase_vacuum(
                        self.get_cpp_obj(),
                        vacuum_progress_monitor_trait_wrapper as *mut c_void,
                    )
                };
            }
        }
        if ret {
            Ok(())
        } else {
            Err(self.create_exception())
        }
    }

    pub fn enable_auto_vacuum(&self, incremental: bool) {
        unsafe { WCDBRustDatabase_enableAutoVacuum(self.get_cpp_obj(), incremental) }
    }

    pub fn incremental_vacuum(&self, pages: i32) -> WCDBResult<()> {
        let ret: bool = unsafe { WCDBRustDatabase_incrementalVacuum(self.get_cpp_obj(), pages) };
        if ret {
            Ok(())
        } else {
            Err(self.create_exception())
        }
    }

    pub(crate) fn create_exception(&self) -> WCDBException {
        WCDBException::create_exception(unsafe { WCDBRustDatabase_getError(self.get_cpp_obj()) })
    }

    pub fn global_trace_performance<CB>(cb_opt: Option<CB>)
    where
        CB: TracePerformanceCallbackTrait + 'static,
    {
        match cb_opt {
            None => {
                *GLOBAL_TRACE_PERFORMANCE_CALLBACK.lock().unwrap() = None;
                unsafe {
                    WCDBRustDatabase_globalTracePerformance(global_trace_performance_callback);
                }
            }
            Some(cb) => {
                let callback_box = Box::new(cb) as TracePerformanceCallback;
                *GLOBAL_TRACE_PERFORMANCE_CALLBACK.lock().unwrap() = Some(callback_box);
                unsafe {
                    WCDBRustDatabase_globalTracePerformance(global_trace_performance_callback);
                }
            }
        }
    }

    pub fn global_trace_sql<CB>(cb_opt: Option<CB>)
    where
        CB: TraceSqlCallbackTrait + 'static,
    {
        match cb_opt {
            None => unsafe {
                *GLOBAL_TRACE_SQL_CALLBACK.lock().unwrap() = None;
                WCDBRustDatabase_globalTraceSQL(global_trace_sql_callback);
            },
            Some(cb) => {
                let callback_box = Box::new(cb) as TraceSqlCallback;
                *GLOBAL_TRACE_SQL_CALLBACK.lock().unwrap() = Some(callback_box);
                unsafe {
                    WCDBRustDatabase_globalTraceSQL(global_trace_sql_callback);
                }
            }
        }
    }

    pub fn trace_sql<CB>(&self, cb_opt: Option<CB>)
    where
        CB: TraceSqlCallbackTrait + 'static,
    {
        match cb_opt {
            None => unsafe {
                *DATABASE_TRACE_SQL_CALLBACK.lock().unwrap() = None;
                WCDBRustDatabase_traceSQL(self.get_cpp_obj(), trace_sql_callback);
            },
            Some(cb) => {
                let callback_box = Box::new(cb) as TraceSqlCallback;
                *DATABASE_TRACE_SQL_CALLBACK.lock().unwrap() = Some(callback_box);
                unsafe {
                    WCDBRustDatabase_traceSQL(self.get_cpp_obj(), trace_sql_callback);
                }
            }
        }
    }

    pub fn global_trace_exception<CB>(cb_opt: Option<CB>)
    where
        CB: TraceExceptionCallbackTrait + 'static,
    {
        match cb_opt {
            None => {
                *GLOBAL_TRACE_EXCEPTION_CALLBACK.lock().unwrap() = None;
                unsafe {
                    WCDBRustDatabase_globalTraceException(global_trace_exception_callback);
                }
            }
            Some(cb) => {
                let callback_box = Box::new(cb) as TraceExceptionCallback;
                *GLOBAL_TRACE_EXCEPTION_CALLBACK.lock().unwrap() = Some(callback_box);
                unsafe {
                    WCDBRustDatabase_globalTraceException(global_trace_exception_callback);
                }
            }
        }
    }

    pub fn trace_exception<CB>(&self, cb_opt: Option<CB>)
    where
        CB: TraceExceptionCallbackTrait + 'static,
    {
        match cb_opt {
            None => unsafe {
                *DATABASE_TRACE_EXCEPTION_CALLBACK.lock().unwrap() = None;
                WCDBRustDatabase_traceException(self.get_cpp_obj(), trace_exception_callback);
            },
            Some(cb) => {
                let callback_box = Box::new(cb) as TraceExceptionCallback;
                *GLOBAL_TRACE_EXCEPTION_CALLBACK.lock().unwrap() = Some(callback_box);
                unsafe {
                    WCDBRustDatabase_traceException(self.get_cpp_obj(), trace_exception_callback);
                }
            }
        }
    }

    pub fn set_tag(&self, tag: i64) {
        unsafe { WCDBRustDatabase_setTag(self.get_cpp_obj(), tag) }
    }

    /// Get the tag of the database. Tag is 0 by default.
    pub fn get_tag(&self) -> i64 {
        unsafe { WCDBRustDatabase_getTag(self.get_cpp_obj()) as i64 }
    }

    pub fn execute<T: StatementTrait>(&self, statement: &T) -> WCDBResult<()> {
        let handle = self.get_handle(statement.is_write_statement());
        let mut exception_opt = None;
        if !Handle::execute_inner(handle.get_cpp_handle()?, statement) {
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

    pub fn set_notification_when_corrupted<CB>(&self, monitor: Option<CB>)
    where
        CB: CorruptionNotificationTrait + 'static,
    {
        match monitor {
            None => {
                *GLOBAL_CORRUPTION_NOTIFICATION_CALLBACK.lock().unwrap() = None;
                unsafe {
                    WCDBRustDatabase_setNotificationWhenCorrupted(self.get_cpp_obj(), null_mut())
                }
            }
            Some(cb) => {
                let callback_box = Box::new(cb) as CorruptionNotificationCallback;
                *GLOBAL_CORRUPTION_NOTIFICATION_CALLBACK.lock().unwrap() = Some(callback_box);
                unsafe {
                    WCDBRustDatabase_setNotificationWhenCorrupted(
                        self.get_cpp_obj(),
                        global_corruption_notification_callback_wrapper as *mut c_void,
                    )
                }
            }
        }
    }

    pub fn check_if_corrupted(&self) -> bool {
        unsafe { WCDBRustDatabase_checkIfCorrupted(self.get_cpp_obj()) }
    }

    pub fn check_if_is_already_corrupted(&self) -> bool {
        unsafe { WCDBRustDatabase_checkIfIsAlreadyCorrupted(self.get_cpp_obj()) }
    }

    pub fn enable_auto_backup(&self, enable: bool) {
        unsafe { WCDBRustDatabase_enableAutoBackup(self.get_cpp_obj(), enable) }
    }

    pub fn backup(&self) -> WCDBResult<()> {
        let ret = unsafe { WCDBRustDatabase_backup(self.get_cpp_obj()) };
        if ret {
            Ok(())
        } else {
            Err(self.create_exception())
        }
    }

    pub fn check_table_should_be_backup<T: BackupFilterTrait>(
        &self,
        filter: &T,
        table_name: &str,
    ) -> bool {
        filter.table_should_be_backup(table_name)
    }

    pub fn filter_backup<CB>(&self, filter: Option<CB>)
    where
        CB: BackupFilterCallbackTrait + 'static,
    {
        match filter {
            None => {
                *GLOBAL_BACKUP_FILTER_CALLBACK.lock().unwrap() = None;
                unsafe { WCDBRustDatabase_filterBackup(self.get_cpp_obj(), null_mut()) }
            }
            Some(cb) => {
                let callback_box = Box::new(cb) as BackupFilterCallback;
                *GLOBAL_BACKUP_FILTER_CALLBACK.lock().unwrap() = Some(callback_box);
                unsafe {
                    WCDBRustDatabase_filterBackup(
                        self.get_cpp_obj(),
                        backup_filter_callback_wrapper as *const c_void,
                    )
                }
            }
        }
    }

    pub fn retrieve<CB>(&self, monitor: Option<CB>) -> WCDBResult<f64>
    where
        CB: ProgressMonitorTrait + 'static,
    {
        let mut score: f64 = 0f64;
        match monitor {
            None => {
                *GLOBAL_PROGRESS_MONITOR_TRAIT_CALLBACK.lock().unwrap() = None;
                score = unsafe { WCDBRustDatabase_retrieve(self.get_cpp_obj(), null_mut()) as f64 };
            }
            Some(cb) => {
                let callback_box = Box::new(cb) as ProgressMonitorTraitCallback;
                *GLOBAL_PROGRESS_MONITOR_TRAIT_CALLBACK.lock().unwrap() = Some(callback_box);
                score = unsafe {
                    WCDBRustDatabase_retrieve(
                        self.get_cpp_obj(),
                        retrieve_progress_monitor_trait_wrapper as *mut c_void,
                    ) as f64
                };
            }
        }
        if score < 0f64 {
            Err(self.create_exception())
        } else {
            Ok(score)
        }
    }

    pub fn deposit(&self) -> WCDBResult<()> {
        let ret: bool = unsafe { WCDBRustDatabase_deposit(self.get_cpp_obj()) };
        if ret {
            Ok(())
        } else {
            Err(self.create_exception())
        }
    }

    pub fn remove_deposited_files(&self) -> WCDBResult<()> {
        let ret: bool = unsafe { WCDBRustDatabase_removeDepositedFiles(self.get_cpp_obj()) };
        if ret {
            Ok(())
        } else {
            Err(self.create_exception())
        }
    }

    pub fn contain_deposited_files(&self) -> bool {
        unsafe { WCDBRustDatabase_containDepositedFiles(self.get_cpp_obj()) }
    }

    pub fn truncate_check_point(&self) -> WCDBResult<()> {
        let ret: bool = unsafe { WCDBRustDatabase_truncateCheckpoint(self.get_cpp_obj()) };
        if ret {
            Ok(())
        } else {
            Err(self.create_exception())
        }
    }

    pub fn set_auto_check_point_enable(&self, enable: bool) {
        unsafe { WCDBRustDatabase_setAutoCheckpointEnable(self.get_cpp_obj(), enable) }
    }
}

#[derive(Debug, Default)]
#[repr(C)]
pub struct PerformanceInfo {
    pub table_page_read_count: i32,
    pub table_page_write_count: i32,
    pub index_page_read_count: i32,
    pub index_page_write_count: i32,
    pub overflow_page_read_count: i32,
    pub overflow_page_write_count: i32,
    pub cost_in_nanoseconds: u64,
}
