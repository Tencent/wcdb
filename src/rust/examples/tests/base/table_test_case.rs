use crate::base::base_test_case::TestCaseTrait;
use crate::base::database_test_case::DatabaseTestCase;
use crate::base::test_object::{TestObject, DBTESTOBJECT_INSTANCE};
use lazy_static::lazy_static;
use std::sync::{Arc, RwLock};
use wcdb::base::wcdb_exception::WCDBResult;
use wcdb::core::database::Database;
use wcdb::core::handle_orm_operation::HandleORMOperationTrait;

pub trait SelectingObjectOperationTrait {
    fn execute() -> Vec<TestObject>;
}

pub struct TableTestCase {
    table_name: String,
    pub(crate) data_base_test_case: DatabaseTestCase,
    is_virtual_table: bool,
}

impl TestCaseTrait for TableTestCase {
    fn setup(&self) -> WCDBResult<()> {
        self.data_base_test_case.setup()
    }

    fn teardown(&self) -> WCDBResult<()> {
        self.data_base_test_case.teardown()
    }
}

lazy_static! {
    static ref TABLE_TEST_CASE: TableTestCase = {
        let table_test = TableTestCase::new();
        table_test
    };
}

impl TableTestCase {
    pub fn new() -> Self {
        TableTestCase {
            table_name: "testTable".to_string(),
            data_base_test_case: DatabaseTestCase::new(),
            is_virtual_table: false,
        }
    }

    pub fn new_with_table_name(table_name: &str) -> Self {
        TableTestCase {
            table_name: table_name.to_string(),
            data_base_test_case: DatabaseTestCase::new(),
            is_virtual_table: false,
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

    pub fn create_table(&mut self) -> WCDBResult<()> {
        let database_clone = Arc::clone(&self.data_base_test_case.get_database());
        let database = database_clone.read().unwrap();
        if !self.is_virtual_table {
            database.create_table(&*self.table_name, &*DBTESTOBJECT_INSTANCE)?;
        } else {
            // todo dengxudong
            // database.createVirtualTable(tableName, tableBinding);
        }
        Ok(())
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

    // pub fn get_table(&self) -> Arc<Table<'static, TestObject, DbTestObject>> {
    //     match &self.table {
    //         None => {
    //             panic!("Table is None");
    //         }
    //         Some(table) => Arc::clone(table),
    //     }
    // }

    pub fn get_data_base_test_case(&self) -> &DatabaseTestCase {
        &self.data_base_test_case
    }

    pub fn get_table_name(&self) -> &str {
        &self.table_name
    }
}
