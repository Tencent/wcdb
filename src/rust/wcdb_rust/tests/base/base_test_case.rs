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
            println!(
                "global_trace_performance tag:{} path:{} handle_id:{} sql:{} info:{:?}",
                tag, path, handle_id, sql, info
            );
        }));

        Database::global_trace_sql(Some(|tag, path, handle_id, sql, info| {
            println!(
                "global_trace_sql tag:{} path:{} handle_id:{} sql:{} info:{:?}",
                tag, path, handle_id, sql, info
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
        println!("Current directory: {}", self.current_directory);
        eprintln!("Current directory: {}", self.current_directory);
        Ok(())
    }

    fn teardown(&self) -> WCDBResult<()> {
        Ok(())
    }
}
