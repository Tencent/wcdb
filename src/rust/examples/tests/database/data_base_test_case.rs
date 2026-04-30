use crate::base::base_test_case::TestCaseTrait;
use crate::base::table_test_case::TableTestCase;
use lazy_static::lazy_static;
use std::sync::{Arc, RwLock};
use wcdb::base::wcdb_exception::WCDBResult;

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
        self.table_test_case.teardown()
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
    static ref DATABASE_TEST: Arc<RwLock<DatabaseTest>> = {
        let database_test = DatabaseTest::new();
        Arc::new(RwLock::new(database_test))
    };
}

#[cfg(test)]
pub mod data_base_test {
    use crate::base::base_test_case::TestCaseTrait;
    use crate::base::wrapped_value::WrappedValue;
    use crate::database::data_base_test_case::{DatabaseTest, DATABASE_TEST};
    use std::sync::{Arc, Mutex, RwLock};
    use std::thread;
    use std::thread::JoinHandle;
    use std::time::{Duration, SystemTime, UNIX_EPOCH};
    use wcdb::core::database::Database;
    use wcdb::core::handle_operation::HandleOperationTrait;
    use wcdb::winq::pragma::Pragma;
    use wcdb::winq::statement_pragma::StatementPragma;

    pub fn setup() {
        let tmp_clone = Arc::clone(&DATABASE_TEST);
        let current = tmp_clone.read().expect("repair_test write failure");
        current.setup().expect("setup failure");
    }
    pub fn teardown() {
        let tmp_clone = Arc::clone(&DATABASE_TEST);
        let current = tmp_clone.read().expect("repair_test write failure");
        current.teardown().expect("teardown failure");
    }

    pub fn get_arc_database() -> Arc<RwLock<Database>> {
        let tmp_clone = Arc::clone(&DATABASE_TEST);
        let ret = tmp_clone
            .read()
            .unwrap()
            .get_table_test_case()
            .get_database();
        Arc::clone(&ret)
    }

    fn current_time_millis() -> u128 {
        let now = SystemTime::now();
        now.duration_since(UNIX_EPOCH)
            .expect("Time went backwards")
            .as_millis()
    }

    #[test]
    pub fn test_tag() {
        setup();
        let database_arc = get_arc_database();
        let database = database_arc.read().unwrap();
        assert_ne!(database.get_tag(), 0);
        let new_database = Database::new(database.get_path().as_str(), None);
        assert_eq!(database.get_tag(), new_database.get_tag());
        teardown();
    }

    #[test]
    pub fn test_path() {
        setup();
        let database_arc = get_arc_database();
        let database = database_arc.read().unwrap();
        assert_eq!(database.can_open(), true);
        let binding = Arc::clone(&DATABASE_TEST);
        let database_test_clone = binding.read().unwrap();
        assert_eq!(
            database.get_path(),
            database_test_clone.get_table_test_case().get_path()
        );
        teardown();
    }

    #[test]
    pub fn test_open_and_close() {
        setup();
        let database_test = DatabaseTest::new();
        let binding = database_test.get_table_test_case().get_database();
        let database = binding.read().unwrap();
        assert_eq!(database.is_opened(), false);

        let database_arc = get_arc_database();
        let database = database_arc.read().unwrap();
        assert_eq!(database.can_open(), true);
        assert_eq!(database.is_opened(), true);
        database.close(Some(|| {}));
        assert_eq!(database.is_opened(), false);
        teardown();
    }

    #[test]
    pub fn test_blockade() {
        setup();
        {
            let database_arc = get_arc_database();
            let database = database_arc.read().unwrap();
            database.blockade();
        }
        let time = Arc::new(Mutex::new(WrappedValue::new()));
        let thread_handle = {
            let database_clone = get_arc_database();
            let time_clone = Arc::clone(&time);
            thread::spawn(move || {
                let database = database_clone.read().unwrap();
                assert!(database.can_open());
                thread::sleep(Duration::from_millis(100)); //todo dengxudong 怀疑 can_open 未阻塞
                let mut time = time_clone.lock().unwrap();
                time.int_value = current_time_millis() as i64;
            })
        };
        thread::sleep(Duration::from_millis(1000));
        let new_time = current_time_millis() as i64;
        {
            let database_arc = get_arc_database();
            let database = database_arc.read().unwrap();
            database.un_blockade();
        }
        thread_handle.join().unwrap();
        let time = time.lock().unwrap();
        assert!(new_time < time.int_value);
        teardown();
    }

    #[test]
    pub fn test_blockade_and_close() {
        setup();
        let main = Arc::new(Mutex::new(WrappedValue::current_time()));
        let sub_thread = Arc::new(Mutex::new(WrappedValue::current_time()));

        let thread_handle: JoinHandle<()> = {
            let database_clone = get_arc_database();
            let sub_thread = Arc::clone(&sub_thread);
            thread::spawn(move || {
                let db = database_clone.read().unwrap();
                assert!(db.can_open());
                thread::sleep(Duration::from_millis(100)); //todo dengxudong 怀疑 can_open 未阻塞
                let mut sub_thread_value = sub_thread.lock().unwrap();
                sub_thread_value.int_value = current_time_millis() as i64;
            })
        };

        let main_clone = Arc::clone(&main);
        let database_arc = get_arc_database();
        let database = database_arc.read().unwrap();
        database.close(Some(move || {
            let mut main_value = main_clone.lock().unwrap();
            thread::sleep(Duration::from_secs(1));
            main_value.int_value = current_time_millis() as i64;
        }));

        thread_handle.join().unwrap();

        let main_value = main.lock().unwrap();
        let sub_thread_value = sub_thread.lock().unwrap();
        assert!(main_value.int_value < sub_thread_value.int_value);
        teardown();
    }

    #[test]
    pub fn test_readonly() {
        setup();
        teardown();
    }

    #[test]
    pub fn test_run_while_close() {
        setup();
        let database_arc = get_arc_database();
        let database = database_arc.read().unwrap();
        assert_eq!(database.can_open(), true);
        assert_eq!(database.is_opened(), true);
        let database_clone = get_arc_database();
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
        teardown();
    }
}
