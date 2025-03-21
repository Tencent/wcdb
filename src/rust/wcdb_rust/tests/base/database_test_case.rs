use crate::base::base_test_case::{BaseTestCase, TestCaseTrait};
use crate::base::wrapped_value::WrappedValue;
use std::cmp::PartialEq;
use std::fs::OpenOptions;
use std::io::{Seek, SeekFrom, Write};
use std::path::{Path, MAIN_SEPARATOR};
use std::sync::{Arc, Mutex, RwLock};
use std::thread;
use std::thread::ThreadId;
use wcdb_core::base::wcdb_exception::WCDBResult;
use wcdb_core::core::database::{Database, TraceExceptionCallbackTrait};
use wcdb_core::core::handle_orm_operation::HandleORMOperationTrait;
use wcdb_core::orm::field::Field;
use wcdb_core::orm::table_binding::TableBinding;
use wcdb_core::winq::statement::StatementTrait;

#[derive(Clone)]
pub struct DatabaseTestCase {
    base_test_case: BaseTestCase,
    path: Arc<Mutex<String>>,
    file_name: Arc<Mutex<String>>,
    database: Arc<RwLock<Database>>,
    expect_mode: Arc<Mutex<Expect>>,
}

impl DatabaseTestCase {
    pub fn new() -> Self {
        DatabaseTestCase {
            base_test_case: BaseTestCase::new(),
            path: Arc::new(Mutex::new("".to_string())),
            file_name: Arc::new(Mutex::new("".to_string())),
            database: Arc::new(RwLock::new(Database::new("./target/tmp/test.db"))),
            expect_mode: Arc::new(Mutex::new(Expect::AllSQLs)),
        }
    }

    pub fn create_table<T, R: TableBinding<T>>(
        &self,
        table_name: &str,
        binding: &R,
    ) -> WCDBResult<()> {
        self.get_database()
            .write()
            .unwrap()
            .create_table(table_name, binding)?;
        Ok(())
    }

    pub fn execute<T: StatementTrait>(&self, statement: &T) -> WCDBResult<()> {
        self.get_database().read().unwrap().execute(statement)
    }

    pub fn trace_exception<CB>(&self, cb_opt: Option<CB>)
    where
        CB: TraceExceptionCallbackTrait + 'static,
    {
        self.get_database().read().unwrap().trace_exception(cb_opt);
    }

    pub fn drop_table(&self, table_name: &str) -> WCDBResult<()> {
        self.get_database().read().unwrap().drop_table(table_name)
    }

    pub fn insert_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.get_database()
            .read()
            .unwrap()
            .insert_object(object, fields, table_name)
    }

    pub fn do_test_sql<CB>(&self, sql: &str, operation: CB)
    where
        CB: FnOnce() -> WCDBResult<()>,
    {
        let vec = vec![sql.to_string()];
        let _ = self.do_test_sql_vec(vec, operation);
    }

    pub fn do_test_sql_vec<CB>(&self, sql_vec: Vec<String>, operation: CB) -> WCDBResult<()>
    where
        CB: FnOnce() -> WCDBResult<()>,
    {
        assert!(sql_vec.len() > 0);
        loop {
            let mut trace = Arc::new(Mutex::new(WrappedValue::new()));
            {
                trace.lock().unwrap().bool_value = false;
            }
            let expected_sql_vec_mutex = Arc::new(Mutex::new(sql_vec.clone()));
            let expected_sql_vec_mutex_clone = expected_sql_vec_mutex.clone();
            let mode_ref = self.get_expect_mode().clone();
            let current_id: ThreadId = thread::current().id();
            let current_thread = Arc::new(format!("{:?}", current_id));
            let trace_clone = Arc::clone(&trace);
            let current_thread_clone = Arc::clone(&current_thread);
            self.get_database().read().unwrap().trace_sql(Some(
                move |tag: i64, path: String, handle_id: i64, sql: String, info: String| {
                    let current_id_trace = format!("{:?}", thread::current().id());
                    if current_thread_clone.as_str() != current_id_trace {
                        return;
                    }

                    if !trace_clone.lock().unwrap().bool_value {
                        return;
                    }
                    DatabaseTestCase::do_test_sql_as_expected(
                        &mode_ref,
                        &expected_sql_vec_mutex_clone,
                        sql,
                    );
                },
            ));

            let mode_ref = self.get_expect_mode();
            if mode_ref != Expect::SomeSQLs {
                {
                    if !self.get_database().read().unwrap().can_open() {
                        assert!(false, "database can not open");
                        break;
                    }
                }
            }
            let trace_clone = Arc::clone(&trace);
            {
                trace_clone.lock().unwrap().bool_value = true;
            }
            operation()?;
            let expected_sql_vec_mutex_clone2 = expected_sql_vec_mutex.clone();
            let expected_sql_vec_mutex_clone_lock = expected_sql_vec_mutex_clone2.lock().unwrap();
            if expected_sql_vec_mutex_clone_lock.len() != 0 {
                eprintln!("Reminding: {:?}", expected_sql_vec_mutex_clone_lock);
                assert!(false, "expectedSQLs not empty");
                break;
            }
            {
                trace_clone.lock().unwrap().bool_value = false;
            }
            break;
        }
        {
            self.get_database().read().unwrap().trace_sql(Some(
                move |tag: i64, path: String, handle_id: i64, sql: String, info: String| {},
            ));
        }
        Ok(())
    }

    fn do_test_sql_as_expected(
        mode_ref: &Expect,
        expected_sql_vec_mutex: &Arc<Mutex<Vec<String>>>,
        sql: String,
    ) {
        match mode_ref {
            Expect::AllSQLs => {
                let mut expected_sql_vec = expected_sql_vec_mutex.lock().unwrap();
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
                let mut expected_sql_vec = expected_sql_vec_mutex.lock().unwrap();
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
                let vec_lock = expected_sql_vec_mutex.lock();
                match vec_lock {
                    Ok(mut expected_sql_vec) => {
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
                    Err(error) => {
                        println!("expected_sql_vec_mutex error : {}", error);
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
        let file_name = "testDatabase";
        self.set_file_name(file_name.to_string());
        // "/Users/xxx/Rust/wcdb_rust/src/rust/wcdb_rust/BaseTestCase/target/tmp/testDatabase"
        let path = format!(
            "{}{}{}{}{}{}{}",
            self.base_test_case.get_current_directory(),
            MAIN_SEPARATOR,
            "target",
            MAIN_SEPARATOR,
            "tmp",
            MAIN_SEPARATOR,
            file_name
        );
        if Path::new(&path).exists() {
            let _ = std::fs::remove_file(path.as_str());
        }
        self.set_path(path.clone());
        self.set_database(Database::new(path.as_str()));
        let binding = self.get_database();
        let database = binding.read().unwrap();
        database.set_tag(10001);
        Ok(())
    }

    fn teardown(&self) -> WCDBResult<()> {
        self.base_test_case.teardown()?;
        let binding = self.get_database();
        let database = binding.read().unwrap();
        database.close(Some(|| {}));
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
        let mut data = self.database.write().expect("TODO: panic message");
        *data = database;
    }

    pub fn set_expect_mode(&self, expect_mode: Expect) {
        let mut data = self.expect_mode.lock().unwrap();
        *data = expect_mode;
    }

    pub fn get_expect_mode(&self) -> Expect {
        let data = self.expect_mode.lock().unwrap();
        data.clone()
    }

    pub fn get_database(&self) -> Arc<RwLock<Database>> {
        Arc::clone(&self.database)
    }

    pub fn first_material_path(&self) -> String {
        let path_clone = Arc::clone(&self.path);
        let path = path_clone.lock().unwrap();
        format!("{}{}", path, "-first.material")
    }

    pub fn last_material_path(&self) -> String {
        let path_clone = Arc::clone(&self.path);
        let path = path_clone.lock().unwrap();
        format!("{}{}", path, "-last.material")
    }

    pub fn factory_path(&self) -> String {
        let path_clone = Arc::clone(&self.path);
        let path = path_clone.lock().unwrap();
        format!("{}{}", path, ".factory")
    }

    pub fn header_size(&self) -> i32 {
        100
    }

    pub fn page_size(&self) -> i32 {
        4096
    }

    pub fn wal_header_size(&self) -> i32 {
        32
    }

    pub fn wal_frame_header_size(&self) -> i32 {
        24
    }

    pub fn wal_frame_size(&self) -> i32 {
        self.wal_frame_header_size() + self.page_size()
    }

    pub fn wal_path(&self) -> String {
        let path_clone = Arc::clone(&self.path);
        let path = path_clone.lock().unwrap();
        format!("{}{}", path, "-wal")
    }

    pub fn corrupt_header(&self) {
        let binding = self.get_database();
        let database = binding.read().unwrap();
        let database_clone = Arc::clone(&self.get_database());
        let header_size = self.header_size();
        database.close(Some(move || {
            database_clone
                .read()
                .unwrap()
                .truncate_check_point()
                .unwrap();
            let path_str = database_clone.read().unwrap().get_path();
            if !Path::new(&path_str).exists() {
                println!("File not founded");
                return;
            }
            let mut file = OpenOptions::new()
                .read(true)
                .write(true)
                .open(path_str)
                .unwrap();
            file.seek(SeekFrom::Start(0)).unwrap();
            let mut header = Vec::<u8>::new();
            header.push(header_size as u8);
            file.write_all(&header).unwrap();
        }));
    }
}

#[derive(PartialEq, Clone)]
pub enum Expect {
    AllSQLs,
    FirstFewSQLs,
    SomeSQLs,
}

pub type TestOperation = Box<dyn FnOnce() -> WCDBResult<()> + Send + 'static>;
