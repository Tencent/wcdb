use crate::base::base_test_case::TestCaseTrait;
use crate::base::table_test_case::TableTestCase;
use lazy_static::lazy_static;
use std::sync::{Arc, Mutex};
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
    static ref DATABASE: Arc<Mutex<Database>> = {
        DATABASE_TEST.setup().unwrap();
        DATABASE_TEST.get_table_test_case().get_database()
    };
}

#[cfg(test)]
pub mod data_base_test {
    use crate::base::base_test_case::TestCaseTrait;
    use crate::database::data_base_test_case::{DatabaseTest, DATABASE, DATABASE_TEST};
    use std::time::{SystemTime, UNIX_EPOCH};
    use wcdb_core::core::database::Database;

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
        let database = DATABASE.lock().unwrap();
        assert_ne!(database.get_tag(), 0);
        let new_database = Database::new(database.get_path().as_str());
        assert_eq!(database.get_tag(), new_database.get_tag());
    }

    #[test]
    pub fn test_path() {
        let database = DATABASE.lock().unwrap();
        assert_eq!(database.can_open(), true);
        assert_eq!(
            database.get_path(),
            DATABASE_TEST.get_table_test_case().get_path()
        );
    }

    #[test]
    pub fn test_open_and_close() {
        let database_test = DatabaseTest::new();
        // setup(&database_test);
        let database = database_test.get_table_test_case().get_database_lock();
        assert_eq!(database.is_opened(), false);

        let database = DATABASE.lock().unwrap();
        // assert_eq!(database.is_opened(), false);
        assert_eq!(database.can_open(), true);
        assert_eq!(database.is_opened(), true);
        database.close(Some(|| {}));
        assert_eq!(database.is_opened(), false);
    }

    #[test]
    pub fn test_blockade() {
        // {
        //     let database = DATABASE.lock().unwrap();
        //     database.blockade();
        // }
        //
        // let time_arc = Arc::new(Mutex::new(WrappedValue::new()));
        // let clone_database = Arc::clone(&DATABASE);
        // let pair: Arc<(Mutex<bool>, Condvar)> = Arc::new((Mutex::new(false), Condvar::new()));
        // let clone_pair = Arc::clone(&pair);
        // let mut clone_time = Arc::clone(&time_arc);
        // thread::spawn(move || {
        //     let is_blockaded = {
        //         let database = clone_database.lock().unwrap();
        //         database.is_blockaded()
        //     };
        //     if is_blockaded {
        //         let (lock, cvar) = &*clone_pair;
        //         let mut started: MutexGuard<bool> = lock.lock().unwrap();
        //         while !*started {
        //             started = cvar.wait(started).unwrap();
        //         }
        //     }
        //
        //     let database = clone_database.lock().unwrap();
        //     assert_eq!(database.can_open(), true);
        //     let mut time = clone_time.lock().unwrap();
        //     time.int_value = current_time_millis() as i64;
        // });
        //
        // thread::sleep(Duration::from_millis(1000));
        // let new_time = current_time_millis() as i64;
        // {
        //     let database = DATABASE.lock().unwrap();
        //     database.un_blockade();
        // }
        //
        // {
        //     let (lock, cvar) = &*pair;
        //     let mut started: MutexGuard<bool> = lock.lock().unwrap();
        //     *started = true;
        //     cvar.notify_one();
        // }
        //
        // thread::sleep(Duration::from_millis(1000));
        //
        // let clone_time = Arc::clone(&time_arc);
        // let time = clone_time.lock().unwrap().int_value;
        // assert_eq!(new_time < time, true);
    }
}
