use crate::base::base_test_case::{BaseTestCase, TestCaseTrait};
use crate::base::wrapped_value::WrappedValue;
use std::cmp::PartialEq;
use std::path::{Path, MAIN_SEPARATOR};
use std::sync::{Arc, Mutex, MutexGuard};
use wcdb_core::base::wcdb_exception::WCDBResult;
use wcdb_core::core::database::Database;
use wcdb_core::core::handle_orm_operation::HandleORMOperationTrait;
use wcdb_core::orm::table_binding::TableBinding;

#[derive(Clone)]
pub struct DatabaseTestCase {
    base_test_case: BaseTestCase,
    path: Arc<Mutex<String>>,
    file_name: Arc<Mutex<String>>,
    database: Arc<Mutex<Database>>,
    expect_mode: Arc<Mutex<Expect>>,
}

impl DatabaseTestCase {
    pub fn new() -> Self {
        DatabaseTestCase {
            base_test_case: BaseTestCase::new(),
            path: Arc::new(Mutex::new("".to_string())),
            file_name: Arc::new(Mutex::new("".to_string())),
            database: Arc::new(Mutex::new(Database::new("./target/tmp/test.db"))),
            expect_mode: Arc::new(Mutex::new(Expect::AllSQLs)),
        }
    }

    pub fn create_table<T, R: TableBinding<T>>(
        &self,
        table_name: &str,
        binding: &R,
    ) -> WCDBResult<()> {
        self.get_database_lock().create_table(table_name, binding)?;
        Ok(())
    }

    fn do_test_sql<CB>(&self, sql: String, operation: CB)
    where
        CB: FnOnce() -> WCDBResult<()>,
    {
        let vec = vec![sql];
        let _ = self.do_test_sql_vec(vec, operation);
    }

    pub fn do_test_sql_vec<CB>(&self, sql_vec: Vec<String>, operation: CB) -> WCDBResult<()>
    where
        CB: FnOnce() -> WCDBResult<()>,
    {
        assert!(sql_vec.len() > 0);
        loop {
            let mut trace = WrappedValue::new();
            trace.bool_value = true;
            let mut expected_sql_vec = sql_vec.clone();
            {
                self.get_database_lock().trace_sql(Some(
                    move |tag: i64, path: String, handle_id: i64, sql: String, info: String| {
                        if !trace.bool_value {
                            return;
                        }
                        // todo qixinbing
                        // DatabaseTestCase::do_test_sql_as_expected(&self_clone,expected_sql_vec, sql);
                    },
                ));
                expected_sql_vec.clear();
            }

            let mode_ref = self.get_expect_mode();
            if mode_ref != Expect::SomeSQLs {
                {
                    if !self.get_database_lock().can_open() {
                        assert!(false, "database can not open");
                        break;
                    }
                }
            }
            trace.bool_value = true;
            operation()?;
            if expected_sql_vec.len() != 0 {
                eprintln!("Reminding: {:?}", expected_sql_vec);
                assert!(false);
                break;
            }
            trace.bool_value = false;
            break;
        }
        {
            // self.get_database_lock().trace_sql::<TraceSqlCallback>(None);
        }
        Ok(())
    }

    fn do_test_sql_as_expected(
        self_clone: &DatabaseTestCase,
        mut expected_sql_vec: Vec<String>,
        sql: String,
    ) {
        let mode_ref = self_clone.get_expect_mode();
        match mode_ref {
            Expect::AllSQLs => {
                let first_sql = match expected_sql_vec.first() {
                    None => "".to_string(),
                    Some(str) => str.clone(),
                };
                if first_sql == sql {
                    expected_sql_vec.remove(0);
                } else {
                    assert!(first_sql.eq(&sql));
                }
            }
            Expect::FirstFewSQLs => {
                let first_sql = match expected_sql_vec.first() {
                    None => "".to_string(),
                    Some(str) => str.clone(),
                };
                if expected_sql_vec.len() > 0 && first_sql == sql {
                    expected_sql_vec.remove(0);
                } else {
                    assert!(first_sql.eq(&sql));
                }
            }
            Expect::SomeSQLs => {
                for i in 0..expected_sql_vec.len() {
                    let sql_ = match expected_sql_vec.get(i) {
                        None => "".to_string(),
                        Some(str) => str.clone(),
                    };
                    if sql_ == sql {
                        expected_sql_vec.remove(i);
                        break;
                    }
                }
            }
        }
    }

    pub fn trace_sql(&self) {}
}

impl TestCaseTrait for DatabaseTestCase {
    fn setup(&self) -> WCDBResult<()> {
        self.base_test_case.setup()?;
        self.set_expect_mode(Expect::AllSQLs);
        let file_name = "test_database";
        self.set_file_name(file_name.to_string());
        let path = format!(
            "../{}{}{}",
            self.base_test_case.get_current_directory(),
            MAIN_SEPARATOR,
            file_name
        );
        if Path::new(&path).exists() {
            // todo qixinbing : 此处会出现删除失败的情况，应该是多线程导致的，待处理
            std::fs::remove_file(path.as_str()).unwrap();
        }
        self.set_path(path.clone());
        self.set_database(Database::new(path.as_str()));
        Ok(())
    }

    fn teardown(&self) -> WCDBResult<()> {
        self.base_test_case.teardown()?;
        // self.database.close(None);
        Ok(())
    }
}

impl DatabaseTestCase {
    pub fn set_path(&self, path: String) {
        let mut data = self.path.lock().unwrap();
        *data = path;
    }

    pub fn get_path(&self) -> String {
        let data = self.path.lock().unwrap();
        data.clone()
    }

    pub fn set_file_name(&self, file_name: String) {
        let mut data = self.file_name.lock().unwrap();
        *data = file_name;
    }

    pub fn get_file_name(&self) -> String {
        let data = self.file_name.lock().unwrap();
        data.clone()
    }

    pub fn set_database(&self, database: Database) {
        let mut data = self.database.lock().unwrap();
        *data = database;
    }
    pub fn get_database_lock(&self) -> MutexGuard<Database> {
        let data = self.database.lock().unwrap();
        data
    }

    pub fn set_expect_mode(&self, expect_mode: Expect) {
        let mut data = self.expect_mode.lock().unwrap();
        *data = expect_mode;
    }

    pub fn get_expect_mode(&self) -> Expect {
        let data = self.expect_mode.lock().unwrap();
        // data.as_ref()
        // &(*data)
        data.clone()
    }
}

#[derive(PartialEq, Clone)]
pub enum Expect {
    AllSQLs,
    FirstFewSQLs,
    SomeSQLs,
}
