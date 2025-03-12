use crate::base::base_test_case::TestCaseTrait;
use crate::base::random_tool::RandomTool;
use crate::base::table_test_case::TableTestCase;
use crate::base::test_object::TestObject;
use lazy_static::lazy_static;
use std::sync::{Arc, RwLock};
use wcdb_core::base::wcdb_exception::WCDBResult;

pub struct ConfigTest {
    table_test_case: TableTestCase,
}

impl TestCaseTrait for ConfigTest {
    fn setup(&self) -> WCDBResult<()> {
        self.table_test_case.setup()
    }

    fn teardown(&self) -> WCDBResult<()> {
        // todo dengxudong
        // database.setConfig(configName, null);
        self.table_test_case.teardown()
    }
}

impl ConfigTest {
    pub fn new() -> Self {
        ConfigTest {
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
    static ref CONFIG_TEST: Arc<RwLock<ConfigTest>> = Arc::new(RwLock::new(ConfigTest::new()));
    static ref PRE_INSERT_OBJECTS: Vec<TestObject> =
        RandomTool::auto_increment_test_case_objects(2);
}

#[cfg(test)]
pub mod config_test_case {
    use crate::base::base_test_case::TestCaseTrait;
    use crate::base::random_tool::RandomTool;
    use crate::base::test_object::{DbTestObject, TestObject, DBTESTOBJECT_INSTANCE};
    use crate::database::config_test_case::CONFIG_TEST;
    use std::sync::{Arc, RwLock, RwLockReadGuard};
    use wcdb_core::core::database;
    use wcdb_core::core::database::{CipherVersion, Database};
    use wcdb_core::core::table_orm_operation::TableORMOperationTrait;
    use wcdb_core::winq::expression_operable::BinaryOperatorType::Match;

    pub fn setup() {
        {
            let config_clone = Arc::clone(&CONFIG_TEST);
            let config_test = config_clone.read().expect("config_clone write failure");
            config_test.setup().expect("teardown failure");

            let database_arc = config_test.get_table_test_case().get_database();
            database_arc.read().unwrap().remove_files().unwrap();
        }
    }

    pub fn teardown() {
        {
            let config_clone = Arc::clone(&CONFIG_TEST);
            let config_test = config_clone.read().expect("config_clone write failure");
            config_test.teardown().expect("teardown failure");
        }
    }

    pub fn get_arc_database() -> Arc<RwLock<Database>> {
        let config_clone = Arc::clone(&CONFIG_TEST);
        let ret = config_clone
            .read()
            .unwrap()
            .get_table_test_case()
            .get_database();
        Arc::clone(&ret)
    }

    #[test]
    pub fn test_cipher() {
        setup();
        let cipher = "123".as_bytes().to_vec();
        let wrong_cipher = "456".as_bytes().to_vec();

        let database_arc = get_arc_database();
        let database = database_arc.read().unwrap();
        database.set_cipher_key(&cipher, None, None);
        assert_eq!(database.can_open(), true);

        database.close(Some(|| {}));

        database.set_cipher_key(&wrong_cipher, None, None);
        assert_eq!(database.can_open(), false);
        teardown();
    }

    #[test]
    pub fn test_cipher_with_page_size() {
        setup();
        let cipher = "123".as_bytes().to_vec();
        let page_size = 8 * 1024;
        let wrong_page_size = 16 * 1024;

        let database_arc = get_arc_database();
        let database = database_arc.read().unwrap();
        database.set_cipher_key(&cipher, Some(page_size), None);
        assert_eq!(database.can_open(), true);

        database.close(Some(|| {}));
        database.set_cipher_key(&cipher, Some(wrong_page_size), None);
        assert_eq!(database.can_open(), false);
        teardown();
    }

    #[test]
    pub fn test_cipher_with_different_version() {
        setup();

        let cipher = "123".as_bytes().to_vec();
        let page_size = 4096;

        let database_arc = get_arc_database();
        let database = database_arc.read().unwrap();
        database.set_cipher_key(&cipher, Some(page_size), Some(CipherVersion::Version3));
        assert_eq!(database.can_open(), true);

        database.close(Some(|| {}));
        database.set_cipher_key(&cipher, Some(page_size), None);
        assert_eq!(database.can_open(), false);

        database.set_cipher_key(&cipher, Some(page_size), Some(CipherVersion::Version3));
        assert_eq!(database.can_open(), true);

        database.remove_files().unwrap();
        database.set_cipher_key(&cipher, Some(page_size), Some(CipherVersion::Version4));
        assert_eq!(database.can_open(), true);
        database.close(Some(|| {}));

        database.set_cipher_key(&cipher, Some(page_size), None);
        assert_eq!(database.can_open(), true);
        Database::set_default_cipher_version(CipherVersion::Version4);
        assert_eq!(database.can_open(), true);
        teardown();
    }

    #[test]
    pub fn test_incremental_vacuum() {
        setup();
        {
            let database_arc = get_arc_database();
            let database = database_arc.read().unwrap();
            database.enable_auto_backup(true);
        }
        {
            let config_test_clone = Arc::clone(&CONFIG_TEST);
            let mut config_test = config_test_clone.write().unwrap();
            config_test
                .get_mut_table_test_case()
                .create_table()
                .unwrap();

            let table_name: &str = config_test.get_table_test_case().get_table_name();
            let database_arc: Arc<RwLock<Database>> =
                config_test.get_table_test_case().get_database();
            let database_clone = Arc::clone(&database_arc);
            let database: RwLockReadGuard<Database> = database_clone.read().unwrap();
            let table = database.get_table(table_name, &*DBTESTOBJECT_INSTANCE);
            let table_clone = Arc::clone(&table);

            table_clone
                .insert_objects(
                    RandomTool::auto_increment_test_case_objects(2),
                    DbTestObject::all_fields(),
                )
                .unwrap();
        }
        {
            let database_arc = get_arc_database();
            let database = database_arc.read().unwrap();
            database.truncate_check_point().unwrap()
            // database.dropTable(tableName);
        }
        teardown();
    }
}
