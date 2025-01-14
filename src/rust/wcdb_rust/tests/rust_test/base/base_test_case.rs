use lazy_static::lazy_static;
use std::fmt::Display;
use std::thread;
use std::time::Duration;
use wcdb_core::base::wcdb_exception::WCDBResult;
use wcdb_core::core::database::Database;

lazy_static! {
    static ref GLOBAL_SETUP: () = {
        BaseTestCase::global_set_up();
    };
}

#[macro_export]
macro_rules! log_error {
    // 如果没有额外的参数，直接输出 msg
    ($msg:expr) => {
        eprintln!("{}", $msg);
    };

    // 如果有参数，格式化 msg 和 args
    ($msg:expr, $($arg:expr),*) => {
        eprintln!($msg, $($arg),*);
    };
}

#[macro_export]
macro_rules! log_info {
    // 如果没有额外的参数，直接输出 msg
    ($msg:expr) => {
        println!("{}", $msg);
    };

    // 如果有参数，格式化 msg 和 args
    ($msg:expr, $($arg:expr),*) => {
        println!($msg, $($arg),*);
    };
}

pub trait TestCaseTrait {
    fn setup(&self) -> WCDBResult<()>;

    fn teardown(&self) -> WCDBResult<()>;
}

#[derive(Clone)]
pub struct BaseTestCase {
    current_directory: String,
}

impl BaseTestCase {
    pub fn new() -> BaseTestCase {
        let _ = &*GLOBAL_SETUP;
        BaseTestCase {
            current_directory: "./target/tmp".to_string(),
        }
    }

    pub fn get_current_directory(&self) -> &str {
        self.current_directory.as_str()
    }

    pub fn global_set_up() {
        Database::global_trace_performance(Some(|tag, path, handle_id, sql, info| {
            log_info!(
                "global_trace_performance tag:{} path:{} handle_id:{} sql:{} info:{:?}",
                tag,
                path,
                handle_id,
                sql,
                info
            );
        }));

        Database::global_trace_sql(Some(|tag, path, handle_id, sql, info| {
            log_info!(
                "global_trace_sql tag:{} path:{} handle_id:{} sql:{} info:{:?}",
                tag,
                path,
                handle_id,
                sql,
                info
            );
        }));

        // todo qixinbing 有崩溃
        // Database::global_trace_exception(Some(|exception| {
        //     log_error!("global_trace_exception exception:{:?}", exception);
        // }));
    }

    pub fn sleep(millisecond: i64) {
        let duration = Duration::from_millis(millisecond as u64);
        thread::sleep(duration);
    }
}

impl TestCaseTrait for BaseTestCase {
    fn setup(&self) -> WCDBResult<()> {
        log_info!("Current directory: {}", self.current_directory);
        log_error!("Current directory: {}", self.current_directory);
        Ok(())
    }

    fn teardown(&self) -> WCDBResult<()> {
        Ok(())
    }
}
