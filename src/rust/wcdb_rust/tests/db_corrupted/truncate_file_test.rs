use crate::db_corrupted::corrupted_base_test_case::CorruptedBaseTestCase;
use crate::db_corrupted::utils::run_cmd;
use wcdb_core::core::handle_orm_operation::HandleORMOperationTrait;
use wcdb_core::core::table_orm_operation::TableORMOperationTrait;

struct TruncateFileTest {
    test_case: CorruptedBaseTestCase,
}

impl TruncateFileTest {
    pub fn new(db_name: &str, auto_backup: bool) -> Self {
        let test_case = CorruptedBaseTestCase::new(db_name, auto_backup);
        TruncateFileTest { test_case }
    }

    pub fn test_case(&self) -> &CorruptedBaseTestCase {
        &self.test_case
    }

    pub fn truncate_file(&self) {
        // # 将文件截断为 1024 字节
        // truncate -s 1024 test.db
        let db_path = format!("target/tmp/{}", self.test_case.db_name());
        let cmd = format!("truncate -s 1 {}", db_path);
        let _ = run_cmd(cmd.as_str());
    }
}

#[cfg(test)]
pub mod truncate_file_exception_test_case {
    use crate::db_corrupted::truncate_file_test::TruncateFileTest;
    use wcdb_core::core::handle_orm_operation::HandleORMOperationTrait;
    use wcdb_core::core::table_orm_operation::TableORMOperationTrait;

    // #[test] // todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_truncate_file_then_backup_exception() {
        let db_name = "truncate_file_then_backup_exception.db";
        let truncate_file_test = TruncateFileTest::new(db_name, true);
        truncate_file_test.test_case().setup();

        truncate_file_test
            .test_case()
            .trace_exception("file is not a database");

        let data_num = 10;
        truncate_file_test.test_case().insert_objects(data_num);

        // 必须强制回写，否则 db 文件内容为空，无法被损坏
        let sql = "PRAGMA wal_checkpoint(FULL);";
        truncate_file_test
            .test_case()
            .database()
            .execute_sql(sql)
            .unwrap();

        truncate_file_test.truncate_file();

        // WCDBCorruptOrIOException(Level: ErrorCode: NotADatabaseException { Message: "file is not a database" })
        // truncate_file_test.test_case().database().backup().unwrap();
        let _ = truncate_file_test.test_case().database().backup();

        assert_eq!(
            truncate_file_test.test_case().get_all_objects().len(),
            data_num as usize
        );

        truncate_file_test.test_case().teardown(true);
    }

    // #[test] // todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_truncate_file_then_write_back_exception() {
        let db_name = "truncate_file_then_write_back_exception.db";
        let truncate_file_test = TruncateFileTest::new(db_name, true);
        truncate_file_test.test_case().setup();

        truncate_file_test
            .test_case()
            .trace_exception("file is not a database");

        let data_num = 10;
        truncate_file_test.test_case().insert_objects(data_num);

        // 必须强制回写，否则 db 文件内容为空，无法被损坏
        let sql = "PRAGMA wal_checkpoint(FULL);";
        truncate_file_test
            .test_case()
            .database()
            .execute_sql(sql)
            .unwrap();

        truncate_file_test.truncate_file();

        // 此处的强制回写竟然不报错？？？神奇
        let sql = "PRAGMA wal_checkpoint(FULL);";
        truncate_file_test
            .test_case()
            .database()
            .execute_sql(sql)
            .unwrap();

        assert_eq!(
            truncate_file_test.test_case().get_all_objects().len(),
            data_num as usize
        );

        truncate_file_test.test_case().teardown(true);
    }

    // #[test] // todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_truncate_file_then_retrieve_exception() {
        let db_name = "truncate_file_then_retrieve_exception.db";
        let truncate_file_test = TruncateFileTest::new(db_name, true);
        truncate_file_test.test_case().setup();

        let data_num = 10;
        truncate_file_test.test_case().insert_objects(10);

        // 必须强制回写，否则 db 文件内容为空，无法被损坏
        let sql = "PRAGMA wal_checkpoint(FULL);";
        truncate_file_test
            .test_case()
            .database()
            .execute_sql(sql)
            .unwrap();

        truncate_file_test.truncate_file();

        // 损坏之后再次回写。为什么不报错？？？神奇
        let sql = "PRAGMA wal_checkpoint(FULL);";
        truncate_file_test
            .test_case()
            .database()
            .execute_sql(sql)
            .unwrap();

        assert_eq!(
            truncate_file_test.test_case().get_all_objects().len(),
            data_num as usize
        );

        truncate_file_test.test_case().teardown(true);
    }

    // #[test] // todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_backup_truncate_retrieve_exception() {
        let db_name = "truncate_file_success.db";
        let truncate_file_test = TruncateFileTest::new(db_name, true);
        truncate_file_test.test_case().setup();

        let data_num = 10;
        let has_back_up = truncate_file_test.test_case().has_back_up();

        if !has_back_up {
            truncate_file_test.test_case().insert_objects(data_num);

            // 必须强制回写，确保备份成功
            let sql = "PRAGMA wal_checkpoint(FULL);";
            truncate_file_test
                .test_case()
                .database()
                .execute_sql(sql)
                .unwrap();

            truncate_file_test.test_case().database().backup().unwrap();
        } else {
            truncate_file_test.truncate_file();

            let _ = truncate_file_test.test_case().database().retrieve(Some(
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

            let database = truncate_file_test.test_case().database();
            let table_name = truncate_file_test.test_case().table_name();
            assert!(!database.table_exist(table_name.as_str()).unwrap());

            // WCDBNormalException(Level: ErrorCode: ErrorException { Message: "no such table: table_goods_object" }
            // truncate_file_test.test_case().insert_objects(data_num);
        }
    }
}
