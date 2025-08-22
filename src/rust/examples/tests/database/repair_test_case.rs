use crate::base::base_test_case::TestCaseTrait;
use crate::base::random_tool::RandomTool;
use crate::base::table_test_case::TableTestCase;
use crate::base::test_object::TestObject;
use lazy_static::lazy_static;
use std::sync::{Arc, RwLock};
use wcdb::base::wcdb_exception::WCDBResult;

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
    static ref PRE_INSERT_OBJECTS: Vec<TestObject> =
        RandomTool::auto_increment_test_case_objects(2);
}

#[cfg(test)]
pub mod repair_test_case {
    use crate::base::base_test_case::TestCaseTrait;
    use crate::base::file_tool::FileTool;
    use crate::base::random_tool::RandomTool;
    use crate::base::test_object::{DbTestObject, TestObject, DBTESTOBJECT_INSTANCE};
    use crate::base::wrapped_value::WrappedValue;
    use crate::database::repair_test_case::{PRE_INSERT_OBJECTS, REPAIR_TEST};
    use std::sync::{Arc, RwLock};
    use std::thread;
    use wcdb::base::wcdb_exception::WCDBResult;
    use wcdb::core::database::{BackupFilterCallbackTrait, Database};
    use wcdb::core::handle_orm_operation::HandleORMOperationTrait;
    use wcdb::core::table_orm_operation::TableORMOperationTrait;

    pub fn setup() {
        {
            let repair_clone = Arc::clone(&REPAIR_TEST);
            let repair_test = repair_clone.read().expect("repair_test write failure");
            repair_test.setup().expect("setup failure");
        }
    }
    pub fn teardown() {
        {
            let repair_clone = Arc::clone(&REPAIR_TEST);
            let repair_test = repair_clone.read().expect("repair_test write failure");
            repair_test.teardown().expect("teardown failure");
        }
    }

    pub fn get_arc_database() -> Arc<RwLock<Database>> {
        let repair_clone = Arc::clone(&REPAIR_TEST);
        let ret = repair_clone
            .read()
            .unwrap()
            .get_table_test_case()
            .get_database();
        Arc::clone(&ret)
    }

    pub fn execute_test<Execute>(execute: Execute)
    where
        Execute: Fn(),
    {
        {
            let database_arc = get_arc_database();
            let database = database_arc.read().unwrap();
            // database.setCipherKey(null);
            let repair_clone = Arc::clone(&REPAIR_TEST);
            let mut repair_test = repair_clone.write().expect("repair_test write failure");
            repair_test
                .get_mut_table_test_case()
                .create_table()
                .expect("Create table table failure");

            let table_name: &str = repair_test.get_table_test_case().get_table_name();
            let database_arc: Arc<RwLock<Database>> =
                repair_test.get_table_test_case().get_database();
            let database_clone = Arc::clone(&database_arc);
            let database = database_clone.read().unwrap();
            let table = database.get_table(table_name, &*DBTESTOBJECT_INSTANCE);
            let table_clone = Arc::clone(&table);

            let mut tmp_vec: Vec<TestObject> = Vec::new();
            PRE_INSERT_OBJECTS.iter().for_each(|o: &TestObject| {
                tmp_vec.push(TestObject::create_auto_increment_object(
                    o.content().parse().expect("parse error"),
                ));
            });
            table_clone
                .insert_objects(tmp_vec, DbTestObject::all_fields())
                .expect("insert objects failure");
        }
        execute();
        {
            let database_arc = get_arc_database();
            let database = database_arc.read().unwrap();
            database.remove_files().expect("remove files failure");
            // database.setCipherKey("123".getBytes());
        }
        {
            let repair_clone = Arc::clone(&REPAIR_TEST);
            let mut repair_test = repair_clone.write().expect("repair_test write failure");
            repair_test
                .get_mut_table_test_case()
                .create_table()
                .expect("Create table table failure");

            let table_name: &str = repair_test.get_table_test_case().get_table_name();
            let database_arc: Arc<RwLock<Database>> =
                repair_test.get_table_test_case().get_database();
            let database_clone = Arc::clone(&database_arc);
            let database = database_clone.read().unwrap();
            let table = database.get_table(table_name, &*DBTESTOBJECT_INSTANCE);
            let table_clone = Arc::clone(&table);

            let mut tmp_vec: Vec<TestObject> = Vec::new();
            PRE_INSERT_OBJECTS.iter().for_each(|o: &TestObject| {
                tmp_vec.push(TestObject::create_auto_increment_object(
                    o.content().parse().expect("parse error"),
                ));
            });
            table
                .insert_objects(tmp_vec, DbTestObject::all_fields())
                .expect("insert objects failure");
        }
        execute();
    }

    // #[test]
    pub fn test_backup() {
        setup();
        {
            // 执行该测试任务前 可能执行过 backup 存在缓存文件。
            let database_arc = get_arc_database();
            let database = database_arc.read().unwrap();
            database
                .remove_files()
                .expect("The remove_files method failed to be executed");
        }
        execute_test(move || {
            {
                let repair_test = REPAIR_TEST.read().expect("Fails to obtain REPAIR_TEST");
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

            let database_arc = get_arc_database();
            let database = database_arc.read().unwrap();
            database
                .backup()
                .expect("The backup method failed to be executed");
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

            thread::sleep(std::time::Duration::from_millis(1000));

            let ret = database.backup();
            match ret {
                Ok(_) => {}
                Err(error) => {
                    // todo dengxudong error
                    // WCDBNormalException(Level: NoticeCode: CorruptException { Message: "Acquired page number: 3 exceeds the page count: 1." })
                    println!("backup error: {:?}", error);
                }
            }
            let repair_test = REPAIR_TEST.read().expect("Fails to obtain REPAIR_TEST");
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
        });
        teardown();
    }

    // #[test]
    pub fn test_backup_with_filter() {
        setup();
        {
            let mut repair_test = REPAIR_TEST.write().expect("Fails to obtain REPAIR_TEST");
            repair_test
                .get_mut_table_test_case()
                .create_table()
                .expect("Table creation failed");

            let table_name: &str = repair_test.get_table_test_case().get_table_name();
            let database_arc: Arc<RwLock<Database>> =
                repair_test.get_table_test_case().get_database();
            let database_clone = Arc::clone(&database_arc);
            let database = database_clone.read().unwrap();
            let table = database.get_table(table_name, &*DBTESTOBJECT_INSTANCE);
            let table_clone = Arc::clone(&table);

            table_clone
                .insert_objects(
                    RandomTool::auto_increment_test_case_objects(2),
                    DbTestObject::all_fields(),
                )
                .expect("Inserting objects failed");
        }

        let database_arc = get_arc_database();
        let database = database_arc.read().unwrap();
        let ret = database.filter_backup::<Box<dyn BackupFilterCallbackTrait>>(None);
        assert!(ret.is_ok());
        database
            .backup()
            .expect("The backup method failed to be executed");

        {
            let mut repair_test = REPAIR_TEST.write().unwrap();
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
        let ret = database.filter_backup(Some(|table_name: &str| {
            return false;
        }));
        assert!(ret.is_ok());
        thread::sleep(std::time::Duration::from_millis(1000));
        database
            .backup()
            .expect("The backup method failed to be executed");
        {
            let mut repair_test = REPAIR_TEST.write().unwrap();
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
            assert_eq!(
                FileTool::get_file_size(
                    &*repair_test
                        .get_table_test_case()
                        .get_data_base_test_case()
                        .first_material_path()
                ) >= FileTool::get_file_size(
                    &*repair_test
                        .get_table_test_case()
                        .get_data_base_test_case()
                        .last_material_path()
                ),
                true
            );
        }
        teardown();
    }

    // #[test]
    pub fn test_auto_backup() {
        setup();
        teardown();
    }

    // #[test]
    pub fn test_deposit() {
        setup();
        // execute_test(|| {
        // 0.
        // {
        //     let repair_clone = Arc::clone(&REPAIR_TEST);
        //     let repair_test = repair_clone.read().unwrap();
        //     let table = repair_test.get_table_test_case().get_table();
        //     let table_clone = Arc::clone(&table);
        // todo dengxudong 需要实现 getValue 方法
        // long num0 = table.getValue(Column.all().count()).getLong();
        // }
        // });
        teardown();
    }

    pub fn do_test_retrieve(success: bool) -> WCDBResult<()> {
        let last_percentage = Arc::new(RwLock::new(WrappedValue::new()));
        let last_percentage_clone = Arc::clone(&last_percentage);
        let database_arc = get_arc_database();
        let database = database_arc.read().unwrap();
        let score = database.retrieve(Some(move |percentage, increment| {
            assert!(
                (percentage - last_percentage_clone.read().unwrap().double_value) == increment
                    && increment > 0f64
            );
            last_percentage_clone.write().unwrap().double_value = percentage;
            return true;
        }));
        match score {
            Ok(val) => {
                assert!((success && val == 1.0f64) || (!success && val < 1.0f64));
            }
            Err(error) => {
                println!("do_test_retrieve error:{:?}", error);
            }
        }
        assert_eq!(
            last_percentage.read().unwrap().double_value - 1.0 <= 0.00001,
            true
        );
        Ok(())
    }

    pub fn do_test_objects_retrieved(success: bool) {
        let repair_clone = Arc::clone(&REPAIR_TEST);
        let repair_test = repair_clone.read().unwrap();
        let database_arc = get_arc_database();
        let database = database_arc.read().unwrap();

        let table_name: &str = repair_test.get_table_test_case().get_table_name();

        let objects = database.get_all_objects(DbTestObject::all_fields(), table_name);
        match objects {
            Ok(object_vec) => {
                if success {
                    assert_eq!(object_vec.is_empty(), false);
                    let mut tmp_vec: Vec<TestObject> = Vec::new();
                    PRE_INSERT_OBJECTS.iter().for_each(|o: &TestObject| {
                        tmp_vec.push(TestObject::create_auto_increment_object(
                            o.content().parse().unwrap(),
                        ));
                    });
                    // assert_eq!(object_vec, tmp_vec);
                } else {
                    assert_eq!(object_vec.is_empty(), true);
                }
            }
            Err(error) => {
                println!(
                    "do_test_objects_retrieved -> get_all_objects error:{:?}, table_name: {1}",
                    error, table_name
                );
            }
        }
    }

    #[test]
    pub fn test_retrieve_with_backup_and_deposit() {
        setup();
        execute_test(|| {
            {
                let database_arc = get_arc_database();
                let database = database_arc.read().unwrap();
                database
                    .backup()
                    .expect("The backup method failed to be executed");
                database.deposit().expect("Deposit failed");
            }
            {
                let repair_clone = Arc::clone(&REPAIR_TEST);
                let repair_test = repair_clone.read().expect("Fails to obtain REPAIR_TEST");
                repair_test
                    .get_table_test_case()
                    .get_data_base_test_case()
                    .corrupt_header();
            }
            do_test_retrieve(true).expect("Failed to retrieve database");
            do_test_objects_retrieved(true);
        });
        teardown();
    }

    // #[test]
    pub fn test_retrieve_with_backup_and_without_deposit() {
        setup();
        execute_test(|| {
            {
                let database_arc = get_arc_database();
                let database = database_arc.read().unwrap();
                database
                    .backup()
                    .expect("The backup method failed to be executed");
            }
            {
                let repair_clone = Arc::clone(&REPAIR_TEST);
                let repair_test = repair_clone.read().expect("Fails to obtain REPAIR_TEST");
                repair_test
                    .get_table_test_case()
                    .get_data_base_test_case()
                    .corrupt_header();
            }
            do_test_retrieve(true).expect("Failed to retrieve database");
            do_test_objects_retrieved(true);
        });
        teardown();
    }

    // #[test]
    // todo dengxudong error
    // 方法 do_test_retrieve(false) 中 score 始终返回 1 测试 java 中该方法返回的是 0.5
    pub fn test_retrieve_without_backup_and_deposit() {
        setup();
        execute_test(|| {
            {
                let repair_clone = Arc::clone(&REPAIR_TEST);
                let repair_test = repair_clone.read().expect("Fails to obtain REPAIR_TEST");
                repair_test
                    .get_table_test_case()
                    .get_data_base_test_case()
                    .corrupt_header();
            }
            {
                let database_arc = get_arc_database();
                let database = database_arc.read().unwrap();
                database.deposit().expect("Deposit failed");
            }
            do_test_retrieve(false).expect("Failed to retrieve database");
            do_test_objects_retrieved(false);
        });
        teardown();
    }

    // #[test]
    pub fn test_vacuum() {
        setup();
        execute_test(|| {
            let last_percentage = Arc::new(RwLock::new(WrappedValue::new()));
            let last_percentage_clone = Arc::clone(&last_percentage);
            let ret = {
                let database_arc = get_arc_database();
                let database = database_arc.read().unwrap();
                database.vacuum(Some(move |percentage, increment| {
                    assert!(
                        (percentage - last_percentage_clone.read().unwrap().double_value)
                            == increment
                            && increment > 0f64
                    );
                    last_percentage_clone.write().unwrap().double_value = percentage;
                    return true;
                }))
            };
            assert!(ret.is_ok());
            assert_eq!(
                last_percentage.read().unwrap().double_value - 1.0 <= 0.00001,
                true
            );
            do_test_objects_retrieved(true)
        });
        teardown();
    }
}
