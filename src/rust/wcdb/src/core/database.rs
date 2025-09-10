use crate::base::cpp_object::{CppObject, CppObjectTrait, WCDBRustBase_releaseObject};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::value::Value;
use crate::base::wcdb_exception::{ExceptionCode, ExceptionLevel, WCDBException, WCDBResult};
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
use crate::utils::{ToCString, ToCow};
use crate::winq::expression::Expression;
use crate::winq::ordering_term::OrderingTerm;
use crate::winq::statement::StatementTrait;
use lazy_static::lazy_static;
use std::cell::RefCell;
use std::ffi::{c_char, c_double, c_int, c_void, CStr, CString};
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
pub trait ProgressMonitorTrait: Fn(/*percentage*/f64, /*increment*/f64) -> bool + Send {}
impl<T> ProgressMonitorTrait for T where T: Fn(/*percentage*/ f64, /*increment*/ f64) -> bool + Send {}
pub type ProgressMonitorTraitCallback = Box<dyn ProgressMonitorTrait>;

pub trait SetDatabaseConfigTrait: Fn(Handle) -> bool + Send + Sync {}
pub type SetDatabaseConfigCallback = Box<dyn SetDatabaseConfigTrait>;
impl<T> SetDatabaseConfigTrait for T where T: Fn(Handle) -> bool + Send + Sync {}

// 定义一个全局静态变量来存储闭包
lazy_static! {
    static ref GLOBAL_TRACE_PERFORMANCE_CALLBACK: Arc<Mutex<Option<TracePerformanceCallback>>> =
        Arc::new(Mutex::new(None));
    static ref GLOBAL_TRACE_SQL_CALLBACK: Arc<Mutex<Option<TraceSqlCallback>>> =
        Arc::new(Mutex::new(None));
    static ref GLOBAL_TRACE_EXCEPTION_CALLBACK: Arc<Mutex<Option<TraceExceptionCallback>>> =
        Arc::new(Mutex::new(None));
    static ref GLOBAL_CORRUPTION_NOTIFICATION_CALLBACK: Arc<Mutex<Option<CorruptionNotificationCallback>>> =
        Arc::new(Mutex::new(None));
    static ref GLOBAL_BACKUP_FILTER_CALLBACK: Arc<Mutex<Option<BackupFilterCallback>>> =
        Arc::new(Mutex::new(None));
    static ref GLOBAL_PROGRESS_MONITOR_TRAIT_CALLBACK: Arc<Mutex<Option<ProgressMonitorTraitCallback>>> =
        Arc::new(Mutex::new(None));
    static ref GLOBAL_VACUUM_PROGRESS_MONITOR_TRAIT_CALLBACK: Arc<Mutex<Option<ProgressMonitorTraitCallback>>> =
        Arc::new(Mutex::new(None));
    static ref GLOBAL_INVOCATION_CONFIG_CALLBACK: Arc<Mutex<Option<SetDatabaseConfigCallback>>> =
        Arc::new(Mutex::new(None));
    static ref GLOBAL_UN_INVOCATION_CONFIG_CALLBACK: Arc<Mutex<Option<SetDatabaseConfigCallback>>> =
        Arc::new(Mutex::new(None));
}

pub type DatabaseCloseCallback = extern "C" fn(context: *mut c_void);

extern "C" {
    fn WCDBRustCore_createDatabase(
        path: *const c_char,
        readonly: bool,
        in_memory: bool,
    ) -> *mut c_void;
    fn WCDBRustDatabase_getPath(cpp_obj: *mut c_void) -> *const c_char;

    fn WCDBRustDatabase_removeFiles(cpp_obj: *mut c_void) -> bool;
    fn WCDBRustDatabase_configCipher(
        cpp_obj: *mut c_void,
        key: *const u8,
        key_len: usize,
        page_size: c_int,
        version: c_int,
    );
    fn WCDBRustCore_setDefaultCipherConfig(version: c_int);

    fn WCDBRustDatabase_close(
        cpp_obj: *mut c_void,
        context: *mut c_void,
        cb: DatabaseCloseCallback,
    );

    pub(crate) fn WCDBRustDatabase_config(
        cpp_obj: *mut c_void,
        config_name: *const c_char,
        invocation: *const c_void,
        un_invocation: *const c_void,
        priority: c_int,
    );

    fn WCDBRustDatabase_blockade(cpp_obj: *mut c_void);

    fn WCDBRustDatabase_unblockade(cpp_obj: *mut c_void);

    fn WCDBRustDatabase_isBlockaded(cpp_obj: *mut c_void) -> bool;
    fn WCDBRustDatabase_canOpen(cpp_obj: *mut c_void) -> bool;

    fn WCDBRustDatabase_isOpened(cpp_obj: *mut c_void) -> bool;

    fn WCDBRustDatabase_getHandle(cpp_obj: *mut c_void, write_hint: bool) -> *mut c_void;

    fn WCDBRustDatabase_vacuum(cpp_obj: *mut c_void, monitor: *const c_void) -> bool;

    fn WCDBRustDatabase_enableAutoVacuum(cpp_obj: *mut c_void, incremental: bool);

    fn WCDBRustDatabase_incrementalVacuum(cpp_obj: *mut c_void, pages: i32) -> bool;

    fn WCDBRustDatabase_getError(cpp_obj: *mut c_void) -> *mut c_void;

    fn WCDBRustDatabase_globalTracePerformance(global_trace_performance_callback: *mut c_void);

    fn WCDBRustDatabase_tracePerformance(
        cpp_obj: *mut c_void,
        trace_performance_callback: extern "C" fn(
            cb_raw: *mut c_void,
            tag: i64,
            path: *const c_char,
            handle_id: i64,
            sql: *const c_char,
            info: PerformanceInfo,
        ),
        cb_ptr: *mut c_void,
    );

    fn WCDBRustDatabase_globalTraceSQL(global_trace_sql_callback: *mut c_void);

    fn WCDBRustDatabase_traceSQL(
        cpp_obj: *mut c_void,
        trace_sql_callback: extern "C" fn(
            cb_raw: *mut c_void,
            tag: i64,
            path: *const c_char,
            handle_id: i64,
            sql: *const c_char,
            info: *const c_char,
        ),
        cb_ptr: *mut c_void,
    );

    fn WCDBRustDatabase_globalTraceException(global_trace_exception_callback: *mut c_void);

    fn WCDBRustDatabase_traceException(
        cpp_obj: *mut c_void,
        trace_exception_callback: extern "C" fn(cb_raw: *mut c_void, exp_cpp_obj: *mut c_void),
        cb_ptr: *mut c_void,
    );

    fn WCDBRustDatabase_getTag(cpp_obj: *mut c_void) -> *mut c_void;

    fn WCDBRustDatabase_setTag(cpp_obj: *mut c_void, tag: i64);

    fn WCDBRustDatabase_setNotificationWhenCorrupted(
        cpp_obj: *mut c_void,
        global_corruption_notification: *mut c_void,
    );

    fn WCDBRustDatabase_checkIfCorrupted(cpp_obj: *mut c_void) -> bool;

    fn WCDBRustDatabase_checkIfIsAlreadyCorrupted(cpp_obj: *mut c_void) -> bool;

    fn WCDBRustDatabase_enableAutoBackup(cpp_obj: *mut c_void, enable: bool);

    fn WCDBRustDatabase_backup(cpp_obj: *mut c_void) -> bool;

    fn WCDBRustDatabase_filterBackup(cpp_obj: *mut c_void, filter: *const c_void);

    fn WCDBRustDatabase_retrieve(cpp_obj: *mut c_void, monitor: *const c_void) -> c_double;

    fn WCDBRustDatabase_deposit(cpp_obj: *mut c_void) -> bool;

    fn WCDBRustDatabase_removeDepositedFiles(cpp_obj: *mut c_void) -> bool;

    fn WCDBRustDatabase_containDepositedFiles(cpp_obj: *mut c_void) -> bool;
    fn WCDBRustDatabase_truncateCheckpoint(cpp_obj: *mut c_void) -> bool;

    fn WCDBRustDatabase_setAutoCheckpointEnable(cpp_obj: *mut c_void, enable: bool);
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
    let global_callback = GLOBAL_TRACE_PERFORMANCE_CALLBACK.lock();
    match global_callback {
        Ok(callback) => {
            if let Some(cb) = &*callback {
                cb(
                    tag,
                    path.to_cow().to_string(),
                    handle_id,
                    sql.to_cow().to_string(),
                    info,
                );
            } else {
                eprintln!("Method: global_trace_performance_callback, No callback found.");
            }
        }
        Err(error) => {
            eprintln!(
                "Method: global_trace_performance_callback, Failed to acquire lock: {:?}",
                error
            );
        }
    }
}

extern "C" fn trace_performance_callback(
    cb_raw: *mut c_void,
    tag: i64,
    path: *const c_char,
    handle_id: i64,
    sql: *const c_char,
    info: PerformanceInfo,
) {
    if cb_raw.is_null() {
        return;
    }
    let closure = unsafe { &*(cb_raw as *mut Box<dyn TracePerformanceCallbackTrait>) };
    closure(
        tag,
        path.to_cow().to_string(),
        handle_id,
        sql.to_cow().to_string(),
        info,
    );
}

extern "C" fn global_trace_sql_callback(
    tag: i64,
    path: *const c_char,
    handle_id: i64,
    sql: *const c_char,
    info: *const c_char,
) {
    let global_callback = GLOBAL_TRACE_SQL_CALLBACK.lock();
    match global_callback {
        Ok(callback) => {
            if let Some(cb) = &*callback {
                cb(
                    tag,
                    path.to_cow().to_string(),
                    handle_id,
                    sql.to_cow().to_string(),
                    info.to_cow().to_string(),
                );
            } else {
                eprintln!("Method: global_trace_sql_callback, No callback found.");
            }
        }
        Err(error) => {
            eprintln!(
                "Method: global_trace_sql_callback, Failed to acquire lock: {:?}",
                error
            );
        }
    }
}

extern "C" fn trace_sql_callback(
    cb_raw: *mut c_void,
    tag: i64,
    path: *const c_char,
    handle_id: i64,
    sql: *const c_char,
    info: *const c_char,
) {
    if cb_raw.is_null() {
        return;
    }
    let closure = unsafe { &*(cb_raw as *mut Box<dyn TraceSqlCallbackTrait>) };
    closure(
        tag,
        path.to_cow().to_string(),
        handle_id,
        sql.to_cow().to_string(),
        info.to_cow().to_string(),
    );
}

extern "C" fn global_trace_exception_callback(exp_cpp_obj: *mut c_void) {
    if exp_cpp_obj.is_null() {
        return;
    }
    let global_callback = GLOBAL_TRACE_EXCEPTION_CALLBACK.lock();
    match global_callback {
        Ok(callback) => {
            if let Some(cb) = &*callback {
                let ex = WCDBException::create_exception(exp_cpp_obj);
                cb(ex);
            } else {
                eprintln!("Method: global_trace_exception_callback, No callback found.");
            }
        }
        Err(error) => {
            eprintln!(
                "Method: global_trace_exception_callback, Failed to acquire lock: {:?}",
                error
            );
        }
    }
}

extern "C" fn trace_exception_callback(cb_raw: *mut c_void, exp_cpp_obj: *mut c_void) {
    if cb_raw.is_null() || exp_cpp_obj.is_null() {
        return;
    }
    let closure = unsafe { &*(cb_raw as *mut Box<dyn TraceExceptionCallbackTrait>) };
    let ex = WCDBException::create_exception(exp_cpp_obj);
    closure(ex);
    unsafe { WCDBRustBase_releaseObject(exp_cpp_obj) };
}

extern "C" fn global_corruption_notification_callback_wrapper(cpp_obj: *mut c_void) {
    match GLOBAL_CORRUPTION_NOTIFICATION_CALLBACK.lock() {
        Ok(callback) => {
            if let Some(cb) = &*callback {
                let database = Database::from(cpp_obj);
                cb(database);
            } else {
                eprintln!("Method: retrieve_progress_monitor_trait_wrapper, No callback found.");
            }
        }
        Err(error) => {
            eprintln!(
                "Method: global_corruption_notification_callback_wrapper, Failed to acquire lock: {:?}",
                error
            );
        }
    }
}

extern "C" fn backup_filter_callback_wrapper(table_name: *const c_char) -> bool {
    let global_callback = GLOBAL_BACKUP_FILTER_CALLBACK.lock();
    match global_callback {
        Ok(callback) => {
            if let Some(cb) = &*callback {
                let cstr = unsafe { CStr::from_ptr(table_name) };
                match cstr.to_str() {
                    Ok(str) => cb(str),
                    Err(error) => {
                        eprintln!(
                            "Method: backup_filter_callback_wrapper, CStr parsing error: {:?}",
                            error
                        );
                        false
                    }
                }
            } else {
                eprintln!("Method: backup_filter_callback_wrapper, No callback found.");
                false
            }
        }
        Err(error) => {
            eprintln!(
                "Method: backup_filter_callback_wrapper, Failed to acquire lock: {:?}",
                error
            );
            false
        }
    }
}

// True to continue current operation.
extern "C" fn retrieve_progress_monitor_trait_wrapper(
    percentage: c_double,
    increment: c_double,
) -> bool {
    let global_callback = GLOBAL_PROGRESS_MONITOR_TRAIT_CALLBACK.lock();
    match global_callback {
        Ok(callback) => {
            if let Some(cb) = &*callback {
                cb(percentage as f64, increment as f64)
            } else {
                eprintln!("Method: retrieve_progress_monitor_trait_wrapper, No callback found.");
                false
            }
        }
        Err(error) => {
            eprintln!(
                "Method: retrieve_progress_monitor_trait_wrapper, Failed to acquire lock: {:?}",
                error
            );
            false
        }
    }
}

// True to continue current operation.
extern "C" fn vacuum_progress_monitor_trait_wrapper(
    percentage: c_double,
    increment: c_double,
) -> bool {
    let global_callback = GLOBAL_VACUUM_PROGRESS_MONITOR_TRAIT_CALLBACK.lock();
    match global_callback {
        Ok(callback) => {
            if let Some(cb) = &*callback {
                cb(percentage as f64, increment as f64)
            } else {
                eprintln!("Method: vacuum_progress_monitor_trait_wrapper, No callback found.");
                false
            }
        }
        Err(error) => {
            eprintln!(
                "Method: vacuum_progress_monitor_trait_wrapper, Failed to acquire lock: {:?}",
                error
            );
            false
        }
    }
}

extern "C" fn set_config_invocation_callback(cpp_handle: *mut c_void) -> bool {
    let global_callback = GLOBAL_INVOCATION_CONFIG_CALLBACK.lock();
    match global_callback {
        Ok(callback) => match &*callback {
            None => true,
            Some(cb) => {
                let db = Database::create_invalid_database();
                let handle = Handle::new_with_obj(cpp_handle, &db);
                cb(handle)
            }
        },
        Err(error) => {
            eprintln!(
                "Method: set_config_invocation_callback, Failed to acquire lock: {:?}",
                error
            );
            false
        }
    }
}

extern "C" fn set_config_un_invocation_callback(cpp_handle: *mut c_void) -> bool {
    let global_callback = GLOBAL_UN_INVOCATION_CONFIG_CALLBACK.lock();
    match global_callback {
        Ok(callback) => match &*callback {
            None => true,
            Some(cb) => {
                let db = Database::create_invalid_database();
                let handle = Handle::new_with_obj(cpp_handle, &db);
                cb(handle)
            }
        },
        Err(error) => {
            eprintln!(
                "Method: set_config_un_invocation_callback, Failed to acquire lock: {:?}",
                error
            );
            false
        }
    }
}

/// Priority of config.
/// The higher the priority, the earlier it will be executed.
/// Note that the highest priority is only for cipher config.
#[repr(i32)]
pub enum ConfigPriority {
    Low,
    Default,
    High,
    Highest,
}

pub struct Database {
    handle_orm_operation: HandleORMOperation,
    close_callback: Arc<Mutex<Option<Box<dyn FnOnce() + Send>>>>,
    trace_callback_ref: Arc<RefCell<*mut c_void>>,
    trace_sql_ref: Arc<RefCell<*mut c_void>>,
    trace_exception_ref: Arc<RefCell<*mut c_void>>,
}

unsafe impl Send for Database {}

unsafe impl Sync for Database {}

impl Drop for Database {
    fn drop(&mut self) {
        let raw_ptr = *self.trace_callback_ref.borrow_mut();
        if !raw_ptr.is_null() {
            unsafe {
                let _ = Box::from_raw(raw_ptr);
            }
        }
    }
}

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

impl CppObjectConvertibleTrait for Database {
    fn as_cpp_object(&self) -> &CppObject {
        self.handle_orm_operation.as_cpp_object()
    }
}

impl HandleOperationTrait for Database {
    fn get_handle(&self, write_hint: bool) -> Handle {
        Handle::new(self, write_hint)
    }

    fn auto_invalidate_handle(&self) -> bool {
        true
    }

    fn run_transaction<F: FnOnce(&Handle) -> bool>(&self, callback: F) -> WCDBResult<()> {
        self.handle_orm_operation.run_transaction(
            self.get_handle(true),
            self.auto_invalidate_handle(),
            callback,
        )
    }

    fn execute<T: StatementTrait>(&self, statement: &T) -> WCDBResult<()> {
        self.handle_orm_operation.execute(
            self.get_handle(true),
            self.auto_invalidate_handle(),
            statement,
        )
    }

    fn execute_sql(&self, sql: &str) -> WCDBResult<()> {
        self.handle_orm_operation.execute_sql(
            self.get_handle(true),
            self.auto_invalidate_handle(),
            sql,
        )
    }
}

impl HandleORMOperationTrait for Database {
    fn create_table<T, R: TableBinding<T>>(
        &self,
        table_name: &str,
        binding: &R,
    ) -> WCDBResult<bool> {
        self.handle_orm_operation
            .create_table(self.get_handle(true), table_name, binding)
    }

    fn table_exist(&self, table_name: &str) -> WCDBResult<bool> {
        self.handle_orm_operation.table_exist(
            self.get_handle(true),
            self.auto_invalidate_handle(),
            table_name,
        )
    }

    fn drop_table(&self, table_name: &str) -> WCDBResult<()> {
        self.handle_orm_operation.drop_table(
            self.get_handle(true),
            self.auto_invalidate_handle(),
            table_name,
        )
    }

    fn prepare_insert<T>(&self) -> Insert<T> {
        self.handle_orm_operation
            .prepare_insert(self.get_handle(true), self.auto_invalidate_handle())
    }

    fn prepare_update<T>(&self) -> Update<T> {
        self.handle_orm_operation
            .prepare_update(self.get_handle(true), self.auto_invalidate_handle())
    }

    fn prepare_select<T>(&self) -> Select<T> {
        self.handle_orm_operation
            .prepare_select(self.get_handle(true), self.auto_invalidate_handle())
    }

    fn prepare_delete(&self) -> Delete {
        self.handle_orm_operation
            .prepare_delete(self.get_handle(true), self.auto_invalidate_handle())
    }

    fn insert_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.handle_orm_operation.insert_object(
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
        self.handle_orm_operation.insert_or_replace_object(
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
        self.handle_orm_operation.insert_or_ignore_object(
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
        self.handle_orm_operation.insert_objects(
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
        self.handle_orm_operation.insert_or_replace_objects(
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
        self.handle_orm_operation.insert_or_ignore_objects(
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
        self.handle_orm_operation.delete_objects(
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
        self.handle_orm_operation.update_object(
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
        self.handle_orm_operation.get_first_object(
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
        self.handle_orm_operation.get_all_objects(
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

impl Database {
    pub(crate) fn create_invalid_database() -> Self {
        Database {
            handle_orm_operation: HandleORMOperation::new(None),
            close_callback: Arc::new(Mutex::new(None)),
            trace_callback_ref: Arc::new(RefCell::new(null_mut())),
            trace_sql_ref: Arc::new(RefCell::new(null_mut())),
            trace_exception_ref: Arc::new(RefCell::new(null_mut())),
        }
    }

    pub fn new(path: &str, readonly_opt: Option<bool>) -> Self {
        let c_path = CString::new(path).unwrap_or_default();
        let readonly = readonly_opt.unwrap_or(false);
        let cpp_obj = unsafe { WCDBRustCore_createDatabase(c_path.as_ptr(), readonly, false) };
        Database {
            handle_orm_operation: HandleORMOperation::new(Some(cpp_obj)),
            close_callback: Arc::new(Mutex::new(None)),
            trace_callback_ref: Arc::new(RefCell::new(null_mut())),
            trace_sql_ref: Arc::new(RefCell::new(null_mut())),
            trace_exception_ref: Arc::new(RefCell::new(null_mut())),
        }
    }

    pub fn create_in_memory_database() -> Self {
        let c_path = CString::new("").unwrap_or_default();
        let cpp_obj = unsafe { WCDBRustCore_createDatabase(c_path.as_ptr(), false, true) };
        Database {
            handle_orm_operation: HandleORMOperation::new(Some(cpp_obj)),
            close_callback: Arc::new(Mutex::new(None)),
            trace_callback_ref: Arc::new(RefCell::new(null_mut())),
            trace_sql_ref: Arc::new(RefCell::new(null_mut())),
            trace_exception_ref: Arc::new(RefCell::new(null_mut())),
        }
    }

    // todo qixinbing : Java 没有该方法，考虑删除
    pub fn from(cpp_obj: *mut c_void) -> Self {
        Database {
            handle_orm_operation: HandleORMOperation::new(Some(cpp_obj)),
            close_callback: Arc::new(Mutex::new(None)),
            trace_callback_ref: Arc::new(RefCell::new(null_mut())),
            trace_sql_ref: Arc::new(RefCell::new(null_mut())),
            trace_exception_ref: Arc::new(RefCell::new(null_mut())),
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

    pub fn set_cipher_key(
        &self,
        key: &Vec<u8>,
        page_size: Option<i32>,
        version: Option<CipherVersion>,
    ) {
        let key_ptr = key.as_ptr();
        let key_len = key.len();
        let page_size = page_size.unwrap_or(4096);
        let version = version.unwrap_or(CipherVersion::DefaultVersion);
        unsafe {
            WCDBRustDatabase_configCipher(
                self.get_cpp_obj(),
                key_ptr,
                key_len,
                page_size,
                version as i32,
            );
        }
    }

    pub fn set_default_cipher_version(version: CipherVersion) {
        unsafe {
            WCDBRustCore_setDefaultCipherConfig(version as i32);
        }
    }

    pub fn set_config<I, U>(
        &self,
        config_name: &str,
        invocation: Option<I>,
        un_invocation: Option<U>,
        priority: ConfigPriority,
    ) -> WCDBResult<()>
    where
        I: SetDatabaseConfigTrait + 'static,
        U: SetDatabaseConfigTrait + 'static,
    {
        let mut cpp_priority: i32 = 0;
        match priority {
            ConfigPriority::Low => {
                cpp_priority = 100;
            }
            ConfigPriority::Default => {}
            ConfigPriority::High => {
                cpp_priority = -100;
            }
            ConfigPriority::Highest => {
                cpp_priority = -2147483648;
            }
        }

        let c_config_name = config_name.to_cstring();

        let mut invocation_raw: *const c_void = set_config_invocation_callback as *mut c_void;
        let mut un_invocation_raw: *const c_void = set_config_un_invocation_callback as *mut c_void;
        {
            match GLOBAL_INVOCATION_CONFIG_CALLBACK.lock() {
                Ok(mut global_callback) => match invocation {
                    None => {
                        *global_callback = None;
                        invocation_raw = null_mut();
                    }
                    Some(cb) => {
                        let callback_box = Box::new(cb) as SetDatabaseConfigCallback;
                        *global_callback = Some(callback_box);
                    }
                },
                Err(error) => {
                    return Err(WCDBException::new_with_message(
                        ExceptionLevel::Error,
                        ExceptionCode::Error,
                        error.to_string(),
                    ));
                }
            }
        }
        {
            match GLOBAL_UN_INVOCATION_CONFIG_CALLBACK.lock() {
                Ok(mut global_callback) => match un_invocation {
                    None => {
                        *global_callback = None;
                        un_invocation_raw = null_mut();
                    }
                    Some(cb) => {
                        let callback_box = Box::new(cb) as SetDatabaseConfigCallback;
                        *global_callback = Some(callback_box);
                    }
                },
                Err(error) => {
                    return Err(WCDBException::new_with_message(
                        ExceptionLevel::Error,
                        ExceptionCode::Error,
                        error.to_string(),
                    ));
                }
            }
        }

        unsafe {
            WCDBRustDatabase_config(
                self.get_cpp_obj(),
                c_config_name.as_ptr(),
                invocation_raw,
                un_invocation_raw,
                cpp_priority as c_int,
            );
        }
        Ok(())
    }

    pub fn set_config_with_invocation<I, U>(
        &self,
        config_name: &str,
        invocation: Option<I>,
        priority: ConfigPriority,
    ) -> WCDBResult<()>
    where
        I: SetDatabaseConfigTrait + 'static,
        U: SetDatabaseConfigTrait + 'static,
    {
        self.set_config::<I, U>(config_name, invocation, None, priority)
    }

    pub fn set_config_with_default_priority<I, U>(
        &self,
        config_name: &str,
        invocation: Option<I>,
    ) -> WCDBResult<()>
    where
        I: SetDatabaseConfigTrait + 'static,
        U: SetDatabaseConfigTrait + 'static,
    {
        self.set_config::<I, U>(config_name, invocation, None, ConfigPriority::Default)
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
        let mut cb_raw: *mut c_void = null_mut();
        {
            match GLOBAL_VACUUM_PROGRESS_MONITOR_TRAIT_CALLBACK.lock() {
                Ok(mut global_callback) => match monitor {
                    None => {
                        *global_callback = None;
                        cb_raw = null_mut();
                    }
                    Some(cb) => {
                        let callback_box = Box::new(cb) as ProgressMonitorTraitCallback;
                        *global_callback = Some(callback_box);
                        cb_raw = vacuum_progress_monitor_trait_wrapper as *mut c_void;
                    }
                },
                Err(error) => {
                    return Err(WCDBException::new_with_message(
                        ExceptionLevel::Error,
                        ExceptionCode::Error,
                        error.to_string(),
                    ));
                }
            }
        }
        ret = unsafe { WCDBRustDatabase_vacuum(self.get_cpp_obj(), cb_raw) };
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

    pub fn global_trace_performance<CB>(cb_opt: Option<CB>) -> WCDBResult<()>
    where
        CB: TracePerformanceCallbackTrait + 'static,
    {
        let mut cb_raw: *mut c_void = null_mut();
        {
            match GLOBAL_TRACE_PERFORMANCE_CALLBACK.lock() {
                Ok(mut global_callback) => match cb_opt {
                    None => {
                        *global_callback = None;
                        cb_raw = global_trace_performance_callback as *mut c_void;
                    }
                    Some(cb) => {
                        let callback_box = Box::new(cb) as TracePerformanceCallback;
                        *global_callback = Some(callback_box);
                        cb_raw = global_trace_performance_callback as *mut c_void;
                    }
                },
                Err(error) => {
                    return Err(WCDBException::new_with_message(
                        ExceptionLevel::Error,
                        ExceptionCode::Error,
                        error.to_string(),
                    ));
                }
            }
        }
        unsafe {
            WCDBRustDatabase_globalTracePerformance(cb_raw);
        }
        Ok(())
    }

    pub fn trace_performance<CB>(&self, cb_opt: Option<CB>) -> WCDBResult<()>
    where
        CB: TracePerformanceCallbackTrait + 'static,
    {
        let mut closure_raw = null_mut();
        if let Some(cb) = cb_opt {
            let closure_box = Box::new(Box::new(cb) as Box<dyn TracePerformanceCallbackTrait>);
            closure_raw = Box::into_raw(closure_box) as *mut c_void;
            let mut value = self.trace_callback_ref.borrow_mut();
            *value = closure_raw;
        }
        unsafe {
            WCDBRustDatabase_tracePerformance(
                self.get_cpp_obj(),
                trace_performance_callback,
                closure_raw,
            );
        }
        Ok(())
    }

    pub fn global_trace_sql<CB>(cb_opt: Option<CB>) -> WCDBResult<()>
    where
        CB: TraceSqlCallbackTrait + 'static,
    {
        let mut cb_raw: *mut c_void = null_mut();
        {
            match GLOBAL_TRACE_SQL_CALLBACK.lock() {
                Ok(mut global_callback) => match cb_opt {
                    None => {
                        *global_callback = None;
                        cb_raw = global_trace_sql_callback as *mut c_void;
                    }
                    Some(cb) => {
                        let callback_box = Box::new(cb) as TraceSqlCallback;
                        *global_callback = Some(callback_box);
                        cb_raw = global_trace_sql_callback as *mut c_void;
                    }
                },
                Err(error) => {
                    return Err(WCDBException::new_with_message(
                        ExceptionLevel::Error,
                        ExceptionCode::Error,
                        error.to_string(),
                    ));
                }
            }
        }
        unsafe {
            WCDBRustDatabase_globalTraceSQL(cb_raw);
        }
        Ok(())
    }

    pub fn trace_sql<CB>(&self, cb_opt: Option<CB>) -> WCDBResult<()>
    where
        CB: TraceSqlCallbackTrait + 'static,
    {
        let mut closure_raw = null_mut();
        if let Some(cb) = cb_opt {
            let closure_box = Box::new(Box::new(cb) as Box<dyn TraceSqlCallbackTrait>);
            closure_raw = Box::into_raw(closure_box) as *mut c_void;
            let mut value = self.trace_sql_ref.borrow_mut();
            *value = closure_raw;
        }
        unsafe {
            WCDBRustDatabase_traceSQL(self.get_cpp_obj(), trace_sql_callback, closure_raw);
        }
        Ok(())
    }

    pub fn global_trace_exception<CB>(cb_opt: Option<CB>) -> WCDBResult<()>
    where
        CB: TraceExceptionCallbackTrait + 'static,
    {
        let mut cb_raw: *mut c_void = null_mut();
        {
            match GLOBAL_TRACE_EXCEPTION_CALLBACK.lock() {
                Ok(mut global_callback) => match cb_opt {
                    None => {
                        *global_callback = None;
                        cb_raw = global_trace_exception_callback as *mut c_void;
                    }
                    Some(cb) => {
                        let callback_box = Box::new(cb) as TraceExceptionCallback;
                        *global_callback = Some(callback_box);
                        cb_raw = global_trace_exception_callback as *mut c_void;
                    }
                },
                Err(error) => {
                    return Err(WCDBException::new_with_message(
                        ExceptionLevel::Error,
                        ExceptionCode::Error,
                        error.to_string(),
                    ));
                }
            }
        }
        unsafe {
            WCDBRustDatabase_globalTraceException(cb_raw);
        }
        Ok(())
    }

    pub fn trace_exception<CB>(&self, cb_opt: Option<CB>) -> WCDBResult<()>
    where
        CB: TraceExceptionCallbackTrait + 'static,
    {
        let mut closure_raw = null_mut();
        if let Some(cb) = cb_opt {
            let closure_box = Box::new(Box::new(cb) as Box<dyn TraceExceptionCallbackTrait>);
            closure_raw = Box::into_raw(closure_box) as *mut c_void;
            let mut value = self.trace_exception_ref.borrow_mut();
            *value = closure_raw;
        }
        unsafe {
            WCDBRustDatabase_traceException(
                self.get_cpp_obj(),
                trace_exception_callback,
                closure_raw,
            );
        };
        Ok(())
    }

    pub fn set_tag(&self, tag: i64) {
        unsafe { WCDBRustDatabase_setTag(self.get_cpp_obj(), tag) }
    }

    /// Get the tag of the database. Tag is 0 by default.
    pub fn get_tag(&self) -> i64 {
        unsafe { WCDBRustDatabase_getTag(self.get_cpp_obj()) as i64 }
    }

    pub fn get_all_rows_from_statement<T: StatementTrait>(
        &self,
        statement: &T,
    ) -> WCDBResult<Vec<Vec<Value>>> {
        let mut handle = self.get_handle(false);
        let result = handle.prepared_with_main_statement(statement);
        match result {
            Ok(val) => {
                let prepared_statement = Arc::clone(&val);
                let result = prepared_statement.get_multi_rows();
                prepared_statement.finalize_statement();
                if self.auto_invalidate_handle() {
                    handle.invalidate();
                }
                match result {
                    Ok(values) => {
                        let mut rows: Vec<Vec<Value>> = Vec::with_capacity(values.len());
                        for x in values {
                            let mut item: Vec<Value> = Vec::with_capacity(x.len());
                            for v in x {
                                item.push(v.clone());
                            }
                            rows.push(item)
                        }
                        Ok(rows)
                    }
                    Err(error) => Err(error),
                }
            }
            Err(error) => Err(error),
        }
    }

    pub fn get_value_from_statement<T: StatementTrait>(&self, statement: &T) -> WCDBResult<Value> {
        let mut handle = self.get_handle(false);
        let result = handle.prepared_with_main_statement(statement);
        match result {
            Ok(val) => {
                let prepared_statement = Arc::clone(&val);
                prepared_statement.step()?;
                if !prepared_statement.is_done() {
                    let ret = prepared_statement.get_value(0);
                    prepared_statement.finalize_statement();
                    if self.auto_invalidate_handle() {
                        handle.invalidate();
                    }
                    Ok(ret)
                } else {
                    Ok(Value::default())
                }
            }
            Err(error) => Err(error),
        }
    }

    pub fn get_value_from_sql(&self, sql: &str) -> WCDBResult<Value> {
        let mut handle = self.get_handle(false);
        let result = handle.prepared_with_main_statement_and_sql(sql);
        match result {
            Ok(val) => {
                let prepared_statement = Arc::clone(&val);
                prepared_statement.step()?;
                if !prepared_statement.is_done() {
                    let ret = prepared_statement.get_value(0);
                    prepared_statement.finalize_statement();
                    if self.auto_invalidate_handle() {
                        handle.invalidate();
                    }
                    Ok(ret)
                } else {
                    Ok(Value::default())
                }
            }
            Err(error) => Err(error),
        }
    }

    pub fn get_values_from_sql(&self, sql: &str) -> WCDBResult<Vec<Vec<Value>>> {
        let mut handle = self.get_handle(false);
        let result = handle.prepared_with_main_statement_and_sql(sql);
        match result {
            Ok(val) => {
                let mut ret_vec = Vec::new();
                let prepared_statement = Arc::clone(&val);
                if !prepared_statement.is_done() {
                    let ret = prepared_statement.get_all_values()?;
                    prepared_statement.finalize_statement();
                    if self.auto_invalidate_handle() {
                        handle.invalidate();
                    }
                    ret_vec = ret;
                }
                Ok(ret_vec)
            }
            Err(error) => Err(error),
        }
    }

    pub fn get_objects_from_sql<T>(&self, fields: Vec<&Field<T>>, sql: &str) -> WCDBResult<Vec<T>> {
        let mut handle = self.get_handle(false);
        let result = handle.prepared_with_main_statement_and_sql(sql);
        match result {
            Ok(val) => {
                let mut ret_vec: Vec<T> = Vec::new();
                let prepared_statement = Arc::clone(&val);
                if prepared_statement.step().is_ok() {
                    while !prepared_statement.is_done() {
                        let ret = prepared_statement.get_one_object(&fields);
                        prepared_statement.finalize_statement();
                        if self.auto_invalidate_handle() {
                            handle.invalidate();
                        }
                        if let Ok(Some(val)) = ret {
                            ret_vec.push(val);
                        }
                        if prepared_statement.step().is_err() {
                            break;
                        }
                    }
                }
                Ok(ret_vec)
            }
            Err(error) => Err(error),
        }
    }

    pub fn set_notification_when_corrupted<CB>(&self, monitor: Option<CB>) -> WCDBResult<()>
    where
        CB: CorruptionNotificationTrait + 'static,
    {
        let mut cb_raw: *mut c_void = null_mut();
        {
            match GLOBAL_CORRUPTION_NOTIFICATION_CALLBACK.lock() {
                Ok(mut global_callback) => match monitor {
                    None => {
                        *global_callback = None;
                        cb_raw = null_mut();
                    }
                    Some(cb) => {
                        let callback_box = Box::new(cb) as CorruptionNotificationCallback;
                        *global_callback = Some(callback_box);
                        cb_raw = global_corruption_notification_callback_wrapper as *mut c_void;
                    }
                },
                Err(error) => {
                    return Err(WCDBException::new_with_message(
                        ExceptionLevel::Error,
                        ExceptionCode::Error,
                        error.to_string(),
                    ));
                }
            }
        }
        unsafe {
            WCDBRustDatabase_setNotificationWhenCorrupted(self.get_cpp_obj(), cb_raw);
        }
        Ok(())
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

    pub fn filter_backup<CB>(&self, filter: Option<CB>) -> WCDBResult<()>
    where
        CB: BackupFilterCallbackTrait + 'static,
    {
        let mut cb_raw: *const c_void = null_mut();
        {
            match GLOBAL_BACKUP_FILTER_CALLBACK.lock() {
                Ok(mut global_callback) => match filter {
                    None => {
                        *global_callback = None;
                        cb_raw = null_mut();
                    }
                    Some(cb) => {
                        let callback_box = Box::new(cb) as BackupFilterCallback;
                        *global_callback = Some(callback_box);
                        cb_raw = backup_filter_callback_wrapper as *const c_void;
                    }
                },
                Err(error) => {
                    return Err(WCDBException::new_with_message(
                        ExceptionLevel::Error,
                        ExceptionCode::Error,
                        error.to_string(),
                    ));
                }
            }
        }
        unsafe { WCDBRustDatabase_filterBackup(self.get_cpp_obj(), cb_raw) }
        Ok(())
    }

    pub fn retrieve<CB>(&self, monitor: Option<CB>) -> WCDBResult<f64>
    where
        CB: ProgressMonitorTrait + 'static,
    {
        let mut score: f64 = 0f64;
        let mut cb_raw: *const c_void = null_mut();
        {
            match GLOBAL_PROGRESS_MONITOR_TRAIT_CALLBACK.lock() {
                Ok(mut global_callback) => match monitor {
                    None => {
                        *global_callback = None;
                        cb_raw = null_mut();
                    }
                    Some(cb) => {
                        let callback_box = Box::new(cb) as ProgressMonitorTraitCallback;
                        *global_callback = Some(callback_box);
                        cb_raw = retrieve_progress_monitor_trait_wrapper as *mut c_void;
                    }
                },
                Err(error) => {
                    return Err(WCDBException::new_with_message(
                        ExceptionLevel::Error,
                        ExceptionCode::Error,
                        error.to_string(),
                    ));
                }
            }
        }

        score = unsafe { WCDBRustDatabase_retrieve(self.get_cpp_obj(), cb_raw) as f64 };

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

pub enum CipherVersion {
    DefaultVersion = 0,
    Version1 = 1,
    Version2 = 2,
    Version3 = 3,
    Version4 = 4,
}
