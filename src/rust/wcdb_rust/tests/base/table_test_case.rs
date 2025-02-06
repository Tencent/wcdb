use crate::base::base_test_case::TestCaseTrait;
use crate::base::database_test_case::DatabaseTestCase;
use std::sync::{Arc, Mutex, MutexGuard};
use wcdb_core::base::wcdb_exception::WCDBResult;
use wcdb_core::core::database::Database;

pub struct TableTestCase {
    data_base_test_case: DatabaseTestCase,
}

impl TestCaseTrait for TableTestCase {
    fn setup(&self) -> WCDBResult<()> {
        self.data_base_test_case.setup()
    }

    fn teardown(&self) -> WCDBResult<()> {
        Ok(())
    }
}

impl TableTestCase {
    pub fn new() -> Self {
        TableTestCase {
            data_base_test_case: DatabaseTestCase::new(),
        }
    }

    pub fn get_database_lock(&self) -> MutexGuard<Database> {
        self.data_base_test_case.get_database_lock()
    }

    pub fn get_database(&self) -> Arc<Mutex<Database>> {
        self.data_base_test_case.get_database()
    }

    pub fn get_path(&self) -> String {
        self.data_base_test_case.get_path()
    }
}
