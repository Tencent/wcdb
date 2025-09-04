use crate::base::random_tool::RandomTool;
use crate::base::table_test_case::TableTestCase;
use crate::base::test_object::{DbTestObject, TestObject};
use crate::base::wrapped_value::WrappedValue;
use std::sync::{Arc, Mutex};
use wcdb::base::wcdb_exception::{ExceptionCode, ExceptionLevel, WCDBException};
use wcdb::core::database::{
    Database, PerformanceInfo, TraceExceptionCallback, TracePerformanceCallback, TraceSqlCallback,
};
use wcdb::core::handle_operation::HandleOperationTrait;
use wcdb::core::handle_orm_operation::HandleORMOperationTrait;
use wcdb::winq::identifier::IdentifierTrait;
use wcdb::winq::ordering_term::Order;
use wcdb::winq::pragma::Pragma;
use wcdb::winq::statement_create_index::StatementCreateIndex;
use wcdb::winq::statement_pragma::StatementPragma;
use wcdb::winq::statement_select::StatementSelect;

static TABLE_NAME: &'static str = "trace_test_case";
pub struct TraceTest {
    table_test_case: TableTestCase,
}

impl TraceTest {
    pub fn new() -> Self {
        TraceTest {
            table_test_case: TableTestCase::new_with_table_name(TABLE_NAME),
        }
    }
}

impl TraceTest {
    pub fn test_trace_sql(&self) {
        let statement = StatementPragma::new();
        let statement = statement.pragma(Pragma::user_version());
        let desc = statement.get_description();
        let mut tested = Arc::new(Mutex::new(WrappedValue::new()));
        let tested_clone = tested.clone();

        let database_arc = self.table_test_case.get_database().clone();
        let database = database_arc.read().unwrap();
        let db_tag = database.get_tag();
        let db_path = database.get_path();

        database
            .trace_sql(Some(
                move |tag: i64, path: String, handle_id: i64, sql: String, info: String| {
                    assert_eq!(tag, db_tag);
                    assert_eq!(path, db_path);
                    if sql.eq(desc.as_str()) {
                        tested.lock().unwrap().bool_value = true;
                    }
                },
            ))
            .unwrap();

        database.execute(statement).unwrap();
        assert!(tested_clone.lock().unwrap().bool_value);
        database.trace_sql::<TraceSqlCallback>(None).unwrap();
    }

    pub fn test_global_trace_sql(&self) {
        let database_arc = self.table_test_case.get_database().clone();
        let database = database_arc.read().unwrap();
        database.remove_files().unwrap();

        let statement = StatementPragma::new();
        let statement = statement.pragma(Pragma::user_version());
        let desc = statement.get_description();
        let mut tested = Arc::new(Mutex::new(WrappedValue::new()));
        let tested_clone = tested.clone();
        let db_tag = database.get_tag();
        let db_path = database.get_path();

        Database::global_trace_sql::<TraceSqlCallback>(None).unwrap();
        Database::global_trace_sql(Some(
            move |tag: i64, path: String, handle_id: i64, sql: String, info: String| {
                if !path.eq(db_path.as_str()) {
                    return;
                }
                assert_eq!(tag, db_tag);
                if sql.eq(desc.as_str()) {
                    tested.lock().unwrap().bool_value = true;
                }
            },
        ))
        .unwrap();
        database.execute(statement).unwrap();
        assert!(tested_clone.lock().unwrap().bool_value);
        Database::global_trace_sql::<TraceSqlCallback>(None).unwrap();
    }

    pub fn test_trace_performance(&self) {
        let database_arc = self.table_test_case.get_database().clone();
        let database = database_arc.read().unwrap();
        database.remove_files().unwrap();

        self.table_test_case.create_table().unwrap();
        let mut objects = vec![];
        let obj_size = 1000;
        for _ in 0..obj_size {
            let content = RandomTool::string_by_length(4096);
            let object = TestObject::new(content);
            objects.push(object);
        }

        let mut test_count = Arc::new(Mutex::new(WrappedValue::new()));
        let test_count_clone = test_count.clone();
        let db_tag = database.get_tag();
        let db_path = database.get_path();

        database
            .trace_performance::<TracePerformanceCallback>(None)
            .unwrap();
        database
            .trace_performance(Some(
                move |tag: i64,
                      path: String,
                      handle_id: i64,
                      sql: String,
                      info: PerformanceInfo| {
                    assert_eq!(tag, db_tag);
                    assert_eq!(path, db_path);
                    if sql.starts_with("COMMIT") {
                        assert!(info.cost_in_nanoseconds > 0);
                        assert!(info.table_page_write_count > 0);
                        assert_eq!(0, info.index_page_write_count);
                        assert!(info.overflow_page_write_count > 0);
                        assert_eq!(0, info.table_page_read_count);
                        assert_eq!(0, info.index_page_read_count);
                        assert_eq!(0, info.overflow_page_read_count);
                        test_count.lock().unwrap().int_value += 1;
                    } else if sql.starts_with("CREATE INDEX") {
                        assert!(info.cost_in_nanoseconds > 0);
                        assert_eq!(1, info.table_page_write_count);
                        assert!(info.index_page_write_count > 0);
                        assert_eq!(info.overflow_page_write_count, obj_size);
                        assert!(info.table_page_read_count > 0);
                        assert!(info.index_page_read_count >= 0);
                        assert!(info.overflow_page_read_count > obj_size / 2);
                        test_count.lock().unwrap().int_value += 1;
                    } else if sql.starts_with("SELECT") {
                        assert!(info.cost_in_nanoseconds > 0);
                        assert_eq!(0, info.table_page_write_count);
                        assert_eq!(0, info.index_page_write_count);
                        assert_eq!(0, info.overflow_page_write_count);
                        test_count.lock().unwrap().int_value += 1;
                        if sql.ends_with("ORDER BY content DESC") {
                            assert_eq!(0, info.table_page_read_count);
                            assert!(info.index_page_read_count > 0);
                            assert_eq!(info.overflow_page_read_count, obj_size);
                        } else {
                            assert!(info.table_page_read_count > 0);
                            assert_eq!(0, info.index_page_read_count);
                            assert_eq!(info.overflow_page_read_count, obj_size);
                        }
                    }
                },
            ))
            .unwrap();

        database
            .insert_objects(objects, DbTestObject::all_fields(), TABLE_NAME)
            .unwrap();

        database
            .execute(
                StatementCreateIndex::new()
                    .create_index("testIndex")
                    .on(TABLE_NAME)
                    .indexed_by(vec![DbTestObject::content().get_column()]),
            )
            .unwrap();

        assert_eq!(
            database
                .get_all_objects(
                    DbTestObject::all_fields(),
                    TABLE_NAME,
                    None,
                    None,
                    None,
                    None
                )
                .unwrap()
                .len(),
            obj_size as usize
        );

        assert_eq!(
            database
                .get_all_objects(
                    DbTestObject::all_fields(),
                    TABLE_NAME,
                    None,
                    Some(DbTestObject::content().get_column().order(Order::Desc)),
                    None,
                    None
                )
                .unwrap()
                .len(),
            obj_size as usize
        );

        assert_eq!(test_count_clone.lock().unwrap().int_value, 4);
        database
            .trace_performance::<TracePerformanceCallback>(None)
            .unwrap();
    }

    pub fn test_global_trace_performance(&self) {
        let database_arc = self.table_test_case.get_database().clone();
        let database = database_arc.read().unwrap();
        database.remove_files().unwrap();
        let mut objects = vec![];
        let obj_size = 1000;
        for _ in 0..obj_size {
            let content = RandomTool::string_by_length(4096);
            let object = TestObject::new(content);
            objects.push(object);
        }

        let mut test_count = Arc::new(Mutex::new(WrappedValue::new()));
        let test_count_clone = test_count.clone();
        let mut last_sql_is_insert = Arc::new(Mutex::new(WrappedValue::new()));

        let db_tag = database.get_tag();
        let db_path = database.get_path();

        Database::global_trace_performance::<TracePerformanceCallback>(None).unwrap();
        Database::global_trace_performance(Some(
            move |tag: i64, path: String, handle_id: i64, sql: String, info: PerformanceInfo| {
                if !path.eq(&db_path) {
                    return;
                }
                assert_eq!(tag, db_tag);
                if sql.starts_with("COMMIT") && last_sql_is_insert.lock().unwrap().bool_value {
                    assert!(info.cost_in_nanoseconds > 0);
                    assert!(info.table_page_write_count >= 0);
                    assert!(info.index_page_write_count >= 0);
                    assert!(info.overflow_page_write_count >= 0);
                    assert_eq!(0, info.table_page_read_count);
                    assert_eq!(0, info.index_page_read_count);
                    assert_eq!(0, info.overflow_page_read_count);
                    test_count.lock().unwrap().int_value += 1;
                } else if sql.starts_with("CREATE INDEX") {
                    assert!(info.cost_in_nanoseconds > 0);
                    assert_eq!(1, info.table_page_write_count);
                    assert!(info.index_page_write_count > 0);
                    assert_eq!(info.overflow_page_write_count, obj_size);
                    assert!(info.table_page_read_count > 0);
                    assert!(info.index_page_read_count >= 0);
                    assert!(info.overflow_page_read_count > obj_size / 2);
                    test_count.lock().unwrap().int_value += 1;
                } else if sql.starts_with("SELECT") {
                    assert!(info.cost_in_nanoseconds > 0);
                    assert_eq!(0, info.table_page_write_count);
                    assert_eq!(0, info.index_page_write_count);
                    assert_eq!(0, info.overflow_page_write_count);
                    test_count.lock().unwrap().int_value += 1;
                    if sql.ends_with("ORDER BY content DESC") {
                        assert_eq!(0, info.table_page_read_count);
                        assert!(info.index_page_read_count > 0);
                        assert_eq!(info.overflow_page_read_count, obj_size);
                    } else {
                        assert!(info.table_page_read_count > 0);
                        assert_eq!(0, info.index_page_read_count);
                        assert_eq!(info.overflow_page_read_count, obj_size);
                    }
                }
                last_sql_is_insert.lock().unwrap().bool_value = sql.starts_with("INSERT");
            },
        ))
        .unwrap();

        self.table_test_case.create_table().unwrap();
        database
            .insert_objects(objects, DbTestObject::all_fields(), TABLE_NAME)
            .unwrap();

        database
            .execute(
                StatementCreateIndex::new()
                    .create_index("testIndex")
                    .on(TABLE_NAME)
                    .indexed_by(vec![DbTestObject::content().get_column()]),
            )
            .unwrap();

        assert_eq!(
            database
                .get_all_objects(
                    DbTestObject::all_fields(),
                    TABLE_NAME,
                    None,
                    None,
                    None,
                    None
                )
                .unwrap()
                .len(),
            obj_size as usize
        );

        assert_eq!(
            database
                .get_all_objects(
                    DbTestObject::all_fields(),
                    TABLE_NAME,
                    None,
                    Some(DbTestObject::content().get_column().order(Order::Desc)),
                    None,
                    None
                )
                .unwrap()
                .len(),
            obj_size as usize
        );

        assert_eq!(test_count_clone.lock().unwrap().int_value, 4);
        Database::global_trace_performance::<TracePerformanceCallback>(None).unwrap();
    }

    pub fn test_trace_error(&self) {
        let mut tested = Arc::new(Mutex::new(WrappedValue::new()));
        let tested_clone = tested.clone();
        let database_arc = self.table_test_case.get_database().clone();
        let database = database_arc.read().unwrap();
        let db_tag = database.get_tag();
        let db_path = database.get_path();
        database
            .trace_exception(Some(move |exception: WCDBException| {
                if exception.level == ExceptionLevel::Error
                    && exception.path().eq(db_path.as_str())
                    && exception.tag() == db_tag
                    && exception.code == ExceptionCode::Error
                    && exception.sql().eq("SELECT 1 FROM dummy")
                    && exception.message().eq("no such table: dummy")
                {
                    tested.lock().unwrap().bool_value = true;
                }
            }))
            .unwrap();

        assert!(database.can_open());
        let ret = database.execute(
            StatementSelect::new()
                .select(vec!["1"])
                .from("dummy" /* table_subquery_obj_vec */),
        );
        match ret {
            Ok(_) => {
                assert!(tested_clone.lock().unwrap().bool_value);
            }
            Err(_) => {
                assert!(tested_clone.lock().unwrap().bool_value);
            }
        }
        database
            .trace_exception::<TraceExceptionCallback>(None)
            .unwrap();
    }

    pub fn test_global_trace_error(&self) {
        let mut tested = Arc::new(Mutex::new(WrappedValue::new()));
        let tested_clone = tested.clone();
        let database_arc = self.table_test_case.get_database().clone();
        let database = database_arc.read().unwrap();
        let db_tag = database.get_tag();
        let db_path = database.get_path();

        Database::global_trace_exception::<TraceExceptionCallback>(None).unwrap();

        Database::global_trace_exception(Some(move |exception: WCDBException| {
            if exception.level == ExceptionLevel::Error
                && exception.path().eq(db_path.as_str())
                && exception.tag() == db_tag
                && exception.code == ExceptionCode::Error
                && exception.sql().eq("SELECT 1 FROM dummy")
                && exception.message().eq("no such table: dummy")
            {
                tested.lock().unwrap().bool_value = true;
            }
        }))
        .unwrap();

        assert!(database.can_open());
        let ret = database.execute(
            StatementSelect::new()
                .select(vec!["1".to_string()])
                .from("dummy"),
        );
        match ret {
            Ok(_) => {
                assert!(tested_clone.lock().unwrap().bool_value);
            }
            Err(_) => {
                assert!(tested_clone.lock().unwrap().bool_value);
            }
        }
        Database::global_trace_exception::<TraceExceptionCallback>(None).unwrap();
    }
}

#[cfg(test)]
pub mod trace_test {
    use crate::database::trace_test::TraceTest;

    // #[test]
    // todo qixinbing 单独运行正常，但是集成测试会失败
    pub fn test_trace_sql() {
        let trace_test = TraceTest::new();
        trace_test.test_trace_sql();
    }

    #[test]
    pub fn test_global_trace_sql() {
        let trace_test = TraceTest::new();
        trace_test.test_global_trace_sql();
    }

    // #[test]
    // todo qixinbing 单独运行正常，但是集成测试会失败
    pub fn test_trace_performance() {
        let trace_test = TraceTest::new();
        trace_test.test_trace_performance();
    }

    // #[test]
    // todo qixinbing 本地单独、集测运行正常，但是 ci 集成测试会失败
    pub fn test_global_trace_performance() {
        let trace_test = TraceTest::new();
        trace_test.test_global_trace_performance();
    }

    #[test]
    pub fn test_trace_error() {
        let trace_test = TraceTest::new();
        trace_test.test_trace_error();
    }

    #[test]
    pub fn test_global_trace_error() {
        let trace_test = TraceTest::new();
        trace_test.test_global_trace_error();
    }
}
