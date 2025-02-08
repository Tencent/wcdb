use crate::base::base_test_case::TestCaseTrait;
use crate::base::table_test_case::TableTestCase;
use lazy_static::lazy_static;
use std::sync::{Arc, Mutex, RwLock};
use wcdb_core::base::wcdb_exception::WCDBResult;
use wcdb_core::core::database::Database;

pub struct DatabaseTest {
    table_test_case: TableTestCase,
}

unsafe impl Sync for DatabaseTest {}
unsafe impl Send for DatabaseTest {}

impl TestCaseTrait for DatabaseTest {
    fn setup(&self) -> WCDBResult<()> {
        self.table_test_case.setup()
    }

    fn teardown(&self) -> WCDBResult<()> {
        Ok(())
    }
}

impl DatabaseTest {
    pub fn new() -> Self {
        DatabaseTest {
            table_test_case: TableTestCase::new(),
        }
    }

    pub fn get_table_test_case(&self) -> &TableTestCase {
        &self.table_test_case
    }
}

lazy_static! {
    static ref DATABASE_TEST: DatabaseTest = {
        let database_test = DatabaseTest::new();
        database_test
    };
    static ref DATABASE: Arc<RwLock<Database>> = {
        DATABASE_TEST.setup().unwrap();
        DATABASE_TEST.get_table_test_case().get_database()
    };
}

#[cfg(test)]
pub mod data_base_test {
    use crate::base::base_test_case::TestCaseTrait;
    use crate::base::wrapped_value::WrappedValue;
    use crate::database::data_base_test_case::{DatabaseTest, DATABASE, DATABASE_TEST};
    use std::sync::{Arc, Mutex};
    use std::thread;
    use std::thread::JoinHandle;
    use std::time::{Duration, SystemTime, UNIX_EPOCH};
    use wcdb_core::core::database::Database;
    use wcdb_core::winq::pragma::Pragma;
    use wcdb_core::winq::statement_pragma::StatementPragma;

    fn setup(database_test: &DatabaseTest) {
        database_test.setup().unwrap();
    }

    fn current_time_millis() -> u128 {
        let now = SystemTime::now();
        now.duration_since(UNIX_EPOCH)
            .expect("Time went backwards")
            .as_millis()
    }

    #[test]
    pub fn test_tag() {
        let database = DATABASE.read().unwrap();
        assert_ne!(database.get_tag(), 0);
        let new_database = Database::new(database.get_path().as_str());
        assert_eq!(database.get_tag(), new_database.get_tag());
    }

    #[test]
    pub fn test_path() {
        let database = DATABASE.read().unwrap();
        assert_eq!(database.can_open(), true);
        assert_eq!(
            database.get_path(),
            DATABASE_TEST.get_table_test_case().get_path()
        );
    }

    #[test]
    pub fn test_open_and_close() {
        let database_test = DatabaseTest::new();
        let binding = database_test.get_table_test_case().get_database();
        let database = binding.read().unwrap();
        assert_eq!(database.is_opened(), false);

        let database = DATABASE.read().unwrap();
        assert_eq!(database.can_open(), true);
        assert_eq!(database.is_opened(), true);
        database.close(Some(|| {}));
        assert_eq!(database.is_opened(), false);
    }

    #[test]
    pub fn test_blockade() {
        {
            let database = DATABASE.write().unwrap();
            database.blockade();
        }
        let time = Arc::new(Mutex::new(WrappedValue::new()));
        let thread_handle = {
            let database_clone = Arc::clone(&DATABASE);
            let time_clone = Arc::clone(&time);
            thread::spawn(move || {
                let database = database_clone.read().unwrap();
                assert!(database.can_open());
                let mut time = time_clone.lock().unwrap();
                time.int_value = current_time_millis() as i64;
            })
        };
        thread::sleep(Duration::from_millis(1000));
        let new_time = current_time_millis() as i64;
        {
            let database2 = DATABASE.read().unwrap();
            database2.un_blockade();
        }
        thread_handle.join().unwrap();
        let time = time.lock().unwrap();
        assert!(new_time < time.int_value);
    }

    #[test]
    pub fn test_blockade_and_close() {
        let main = Arc::new(Mutex::new(WrappedValue::current_time()));
        let sub_thread = Arc::new(Mutex::new(WrappedValue::current_time()));

        let thread_handle: JoinHandle<()> = {
            let database = Arc::clone(&DATABASE);
            let sub_thread = Arc::clone(&sub_thread);
            thread::spawn(move || {
                let db = database.read().unwrap();
                assert!(db.can_open());
                thread::sleep(Duration::from_millis(100));
                let mut sub_thread_value = sub_thread.lock().unwrap();
                sub_thread_value.int_value = current_time_millis() as i64;
            })
        };

        let main_clone = Arc::clone(&main);
        let database = DATABASE.read().unwrap();
        database.close(Some(move || {
            let mut main_value = main_clone.lock().unwrap();
            thread::sleep(Duration::from_secs(1));
            main_value.int_value = current_time_millis() as i64;
        }));

        thread_handle.join().unwrap();

        let main_value = main.lock().unwrap();
        let sub_thread_value = sub_thread.lock().unwrap();
        assert!(main_value.int_value < sub_thread_value.int_value);
    }

    #[test]
    pub fn test_readonly() {}

    #[test]
    pub fn test_run_while_close() {
        let database = DATABASE.read().unwrap();
        assert_eq!(database.can_open(), true);
        assert_eq!(database.is_opened(), true);
        let database_clone = Arc::clone(&DATABASE);
        database.close(Some(move || {
            let database = database_clone.read().unwrap();
            let statement_pragma = StatementPragma::new();
            let statement_pragma = statement_pragma
                .pragma(Pragma::user_version())
                .to_value(123);
            let ret = database.execute(statement_pragma);
            assert!(ret.is_ok());
        }));
        assert_eq!(database.is_opened(), false);
    }
}
