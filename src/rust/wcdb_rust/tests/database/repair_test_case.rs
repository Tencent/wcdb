use crate::base::base_test_case::TestCaseTrait;
use crate::base::table_test_case::TableTestCase;
use crate::database::data_base_test_case::DatabaseTest;
use lazy_static::lazy_static;
use std::sync::{Arc, RwLock};
use wcdb_core::base::wcdb_exception::WCDBResult;
use wcdb_core::core::database::Database;

pub struct RepairTest {
    table_test_case: TableTestCase,
}

impl TestCaseTrait for RepairTest {
    fn setup(&self) -> WCDBResult<()> {
        self.table_test_case.setup()
    }

    fn teardown(&self) -> WCDBResult<()> {
        self.table_test_case.teardown()
    }
}

impl RepairTest {
    pub fn new() -> Self {
        RepairTest {
            table_test_case: TableTestCase::new(),
        }
    }

    pub fn get_table_test_case(&self) -> &TableTestCase {
        &self.table_test_case
    }

    pub fn get_mut_table_test_case(&mut self) -> &mut TableTestCase {
        &mut self.table_test_case
    }
}

lazy_static! {
    static ref REPAIR_TEST: Arc<RwLock<RepairTest>> = Arc::new(RwLock::new(RepairTest::new()));
    static ref DATABASE: Arc<RwLock<Database>> = {
        REPAIR_TEST.read().unwrap().setup().unwrap();
        REPAIR_TEST
            .write()
            .unwrap()
            .get_table_test_case()
            .get_database()
    };
}

#[cfg(test)]
pub mod repair_test_case {
    use crate::base::file_tool::FileTool;
    use crate::base::random_tool::RandomTool;
    use crate::base::test_object::DbTestObject;
    use crate::database::repair_test_case::{DATABASE, REPAIR_TEST};
    use std::sync::Arc;
    use std::thread;
    use wcdb_core::core::handle_orm_operation::HandleORMOperationTrait;
    use wcdb_core::core::table_orm_operation::TableORMOperationTrait;

    pub fn execute_test<Execute>(execute: Execute)
    where
        Execute: Fn(),
    {
        {
            let database = DATABASE.read().unwrap();
            // database.setCipherKey(null);
            let repair_clone = Arc::clone(&REPAIR_TEST);
            let mut repair_test = repair_clone.write().unwrap();
            repair_test
                .get_mut_table_test_case()
                .create_table()
                .unwrap();
            let table = Arc::clone(&repair_test.get_table_test_case().get_table());
            table
                .insert_objects(
                    RandomTool::auto_increment_test_case_objects(2),
                    DbTestObject::all_fields(),
                )
                .unwrap();
        }
        execute();
        {
            let database = DATABASE.read().unwrap();
            database.remove_files().unwrap();
            // database.setCipherKey("123".getBytes());
        }
        {
            let repair_clone = Arc::clone(&REPAIR_TEST);
            let mut repair_test = repair_clone.write().unwrap();
            repair_test
                .get_mut_table_test_case()
                .create_table()
                .unwrap();
            let table = Arc::clone(&repair_test.get_table_test_case().get_table());
            table
                .insert_objects(
                    RandomTool::auto_increment_test_case_objects(2),
                    DbTestObject::all_fields(),
                )
                .unwrap();
        }
        execute();
    }

    #[test]
    pub fn test_backup() {
        execute_test(|| {
            {
                let repair_test = REPAIR_TEST.read().unwrap();
                // /Users/xxx/wcdb_rust/src/rust/wcdb_rust/BaseTestCase/target/tmp/testDatabase-first.material
                assert_eq!(
                    FileTool::file_exist(
                        &*repair_test
                            .get_table_test_case()
                            .get_data_base_test_case()
                            .first_material_path()
                    ),
                    false
                );
                assert_eq!(
                    FileTool::file_exist(
                        &*repair_test
                            .get_table_test_case()
                            .get_data_base_test_case()
                            .last_material_path()
                    ),
                    false
                );
            }

            {
                let database = DATABASE.read().unwrap();
                database.backup().unwrap();
                let repair_test = REPAIR_TEST.read().unwrap();
                assert_eq!(
                    FileTool::file_exist(
                        &*repair_test
                            .get_table_test_case()
                            .get_data_base_test_case()
                            .first_material_path()
                    ),
                    true
                );
                assert_eq!(
                    FileTool::file_exist(
                        &*repair_test
                            .get_table_test_case()
                            .get_data_base_test_case()
                            .last_material_path()
                    ),
                    false
                );
            }

            thread::sleep(std::time::Duration::from_millis(1000));

            {
                let database = DATABASE.read().unwrap();
                database.backup().unwrap();
                let repair_test = REPAIR_TEST.read().unwrap();
                assert_eq!(
                    FileTool::file_exist(
                        &*repair_test
                            .get_table_test_case()
                            .get_data_base_test_case()
                            .first_material_path()
                    ),
                    true
                );
                assert_eq!(
                    FileTool::file_exist(
                        &*repair_test
                            .get_table_test_case()
                            .get_data_base_test_case()
                            .last_material_path()
                    ),
                    true
                );
            }

            {
                let database = DATABASE.read().unwrap();
                database.backup().unwrap();
                let repair_test = REPAIR_TEST.read().unwrap();
                if FileTool::file_exist(
                    &*repair_test
                        .get_table_test_case()
                        .get_data_base_test_case()
                        .last_material_path(),
                ) {
                    database.remove_files().unwrap()
                }
            }
        });
    }
}
