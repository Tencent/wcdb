use crate::db_corrupted::corrupted_base_test_case::CorruptedBaseTestCase;

struct DeleteDbFileTest {
    test_case: CorruptedBaseTestCase,
}

impl DeleteDbFileTest {
    pub fn new(db_name: &str, auto_backup: bool) -> Self {
        let test_case = CorruptedBaseTestCase::new(db_name, auto_backup);
        DeleteDbFileTest { test_case }
    }

    pub fn test_case(&self) -> &CorruptedBaseTestCase {
        &self.test_case
    }

    pub fn delete_db_file(&self) {
        let path = format!("./target/tmp/{}", self.test_case().db_name());
        std::fs::remove_file(path).expect("delete db file failed");
    }
}

#[cfg(test)]
pub mod delete_db_file_test_exception {
    use crate::db_corrupted::delete_db_file_test::DeleteDbFileTest;
    use wcdb::core::handle_operation::HandleOperationTrait;

    pub fn delete_db_file_when_write_operation(db_name: &str) {
        let delete_db_file_test = DeleteDbFileTest::new(db_name, true);
        delete_db_file_test.test_case().setup();

        let data_num = 10;
        delete_db_file_test.test_case().insert_objects(data_num);

        delete_db_file_test.delete_db_file();
    }

    // #[test] //todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_delete_db_file_then_write_exception() {
        let db_name = "delete_db_file_when_write_operation.db";
        let delete_db_file_test = DeleteDbFileTest::new(db_name, true);
        delete_db_file_test.test_case().setup();

        delete_db_file_test
            .test_case()
            .trace_exception("file unlinked while open");

        let data_num = 10;
        delete_db_file_test.test_case().insert_objects(data_num);

        delete_db_file_test.delete_db_file();

        // trace_exception: file unlinked while open: path/to/target/tmp/delete_db_file_when_write_operation.db
        delete_db_file_test.test_case().insert_objects(data_num);
    }

    // #[test] //todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_delete_db_file_then_write_back_exception() {
        let db_name = "delete_db_file_when_write_operation.db";
        let delete_db_file_test = DeleteDbFileTest::new(db_name, true);
        delete_db_file_test.test_case().setup();

        delete_db_file_test
            .test_case()
            .trace_exception("file unlinked while open");

        let data_num = 10;
        delete_db_file_test.test_case().insert_objects(data_num);

        delete_db_file_test.delete_db_file();

        // trace_exception: file unlinked while open: path/to/target/tmp/delete_db_file_when_write_operation.db
        let sql = "PRAGMA wal_checkpoint(FULL);";
        delete_db_file_test
            .test_case()
            .database()
            .execute_sql(sql)
            .unwrap();
    }

    // #[test] //todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_delete_db_file_then_retrieve_exception() {
        let db_name = "delete_db_file_when_write_operation.db";
        let delete_db_file_test = DeleteDbFileTest::new(db_name, true);
        delete_db_file_test.test_case().setup();

        delete_db_file_test
            .test_case()
            .trace_exception("file unlinked while open");

        let data_num = 10;
        delete_db_file_test.test_case().insert_objects(data_num);

        delete_db_file_test.delete_db_file();

        // // trace_exception: file unlinked while open: path/to/target/tmp/delete_db_file_when_write_operation.db
        let _ = delete_db_file_test.test_case().database().retrieve(Some(
            move |percentage: f64, increment: f64| {
                println!(
                    "Database retrieve percentage:{} , increment:{}",
                    percentage, increment
                );
                if percentage >= 1.0 {
                    println!("Database retrieve complete");
                }
                true
            },
        ));
    }

    // #[test] //todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_delete_db_file_when_backup_then_write_back_exception() {
        let db_name = "delete_db_file_when_write_success.db";
        let delete_db_file_test = DeleteDbFileTest::new(db_name, true);
        delete_db_file_test.test_case().setup();

        let data_num = 10;
        delete_db_file_test.test_case().insert_objects(data_num);

        let sql = "PRAGMA wal_checkpoint(FULL);";
        delete_db_file_test
            .test_case()
            .database()
            .execute_sql(sql)
            .unwrap();

        delete_db_file_test.test_case().database().backup().unwrap();

        delete_db_file_test.delete_db_file();

        let _ = delete_db_file_test.test_case().database().retrieve(Some(
            move |percentage: f64, increment: f64| {
                println!(
                    "Database retrieve percentage:{} , increment:{}",
                    percentage, increment
                );
                if percentage >= 1.0 {
                    println!("Database retrieve complete");
                }
                true
            },
        ));

        // 无法恢复，表结构没有，数据也没有
        let ret = delete_db_file_test.test_case().database().execute_sql(sql);
        match ret {
            Ok(_) => {
                assert!(true);
            }
            Err(ex) => {
                assert_eq!(
                    ex.message(),
                    "no such table: table_goods_object".to_string()
                );
            }
        }
    }
}
