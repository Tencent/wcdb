use crate::base::base_test_case::TestCaseTrait;
use crate::base::database_test_case::DatabaseTestCase;
use crate::base::test_object::TestObject;
use std::sync::{Arc, RwLock};
use wcdb_core::base::wcdb_exception::WCDBResult;
use wcdb_core::core::database::Database;

pub trait SelectingObjectOperationTrait {
    fn execute() -> Vec<TestObject>;
}
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

    pub fn get_database(&self) -> Arc<RwLock<Database>> {
        self.data_base_test_case.get_database()
    }

    pub fn get_path(&self) -> String {
        self.data_base_test_case.get_path()
    }

    pub fn do_test_object_by_selecting<T: SelectingObjectOperationTrait>(
        &self,
        object: TestObject,
        sql: String,
        operation: &T,
    ) {
        self.do_test_objects_by_selecting(vec![object], vec![sql], operation);
    }

    pub fn do_test_objects_by_selecting<T: SelectingObjectOperationTrait>(
        &self,
        objects: Vec<TestObject>,
        sqls: Vec<String>,
        operation: &T,
    ) {
        self.data_base_test_case
            .do_test_sql_vec(sqls, move || Ok(()))
            .unwrap();
    }
}
