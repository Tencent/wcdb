use crate::db_corrupted::corrupted_base_test_case::CorruptedBaseTestCase;
use crate::db_corrupted::utils::run_cmd;
use wcdb::core::handle_orm_operation::HandleORMOperationTrait;
use wcdb::core::table_orm_operation::TableORMOperationTrait;

struct ModifyDbFileTest {
    test_case: CorruptedBaseTestCase,
}

impl ModifyDbFileTest {
    pub fn new(db_name: &str, auto_backup: bool) -> Self {
        let test_case = CorruptedBaseTestCase::new(db_name, auto_backup);
        ModifyDbFileTest { test_case }
    }

    pub fn test_case(&self) -> &CorruptedBaseTestCase {
        &self.test_case
    }

    pub fn modify_db_file(&self) {
        // # 示例：破坏文件头（高风险操作）
        // echo "Corrupted" | dd of=test.db bs=1 seek=0 count=10 conv=notrunc
        let db_path = format!("target/tmp/{}", self.test_case.db_name());
        let cmd = format!(
            "echo \"Corrupted\" | dd of={} bs=1 seek=0 count=10 conv=notrunc",
            db_path
        );

        let _ = run_cmd(cmd.as_str());
    }
}

#[cfg(test)]
pub mod modify_db_file_exception_test_case {
    use crate::db_corrupted::modify_db_file_test::ModifyDbFileTest;

    // #[test] // todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_modify_then_backup_exception() {
        let db_name = "modify_then_backup_exception.db";
        let modify_db_file_test = ModifyDbFileTest::new(db_name, false);
        modify_db_file_test.test_case().setup();

        modify_db_file_test
            .test_case()
            .trace_exception("NotADatabase");

        let data_num = 10;
        modify_db_file_test.test_case().insert_objects(data_num);

        modify_db_file_test.modify_db_file();

        // WCDBNormalException(Level: NoticeCode: NotADatabaseException { Message: "NotADatabase" })
        let _ = modify_db_file_test.test_case().database().backup();

        assert_eq!(
            modify_db_file_test.test_case().get_all_objects().len(),
            data_num as usize
        );
        modify_db_file_test.test_case().teardown(true);
    }

    // #[test] // todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_modify_then_write_back_exception() {
        let db_name = "modify_then_write_back_exception.db";
        let modify_db_file_test = ModifyDbFileTest::new(db_name, false);
        modify_db_file_test.test_case().setup();

        // WCDBCorruptOrIOException(Level: ErrorCode: IOErrorException { Message: "disk I/O error" })
        modify_db_file_test
            .test_case()
            .trace_exception("disk I/O error");

        let data_num = 10;
        modify_db_file_test.test_case().insert_objects(data_num);

        modify_db_file_test.modify_db_file();

        let _ = modify_db_file_test
            .test_case()
            .database()
            .execute_sql("PRAGMA wal_checkpoint(FULL);");

        modify_db_file_test.test_case().teardown(true);
    }

    // #[test] // todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_modify_then_retrieve_exception() {
        let db_name = "modify_then_retrieve_exception.db";

        let modify_db_file_test = ModifyDbFileTest::new(db_name, false);
        modify_db_file_test.test_case().setup();

        // WCDBNormalException(Level: NoticeCode: NotADatabaseException { Message: "NotADatabase" })
        modify_db_file_test
            .test_case()
            .trace_exception("NotADatabase");

        let data_num = 10;
        modify_db_file_test.test_case().insert_objects(data_num);

        modify_db_file_test.modify_db_file();

        let _ = modify_db_file_test.test_case().database().retrieve(Some(
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
        modify_db_file_test.test_case().teardown(true);
    }
}

#[cfg(test)]
pub mod modify_then_backup_success_test_case {
    use crate::db_corrupted::modify_db_file_test::ModifyDbFileTest;

    // 第一次运行，写入数据并手动备份
    // 第二次运行，修改数据库文件并恢复数据，再写入数据并验证数据库有效性
    // 该用例：单独运行两次可以成功，直接测试所有代码，第二次会失败
    // #[test] // todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test() {
        let db_name = "test_modify_then_backup_success.db";
        let modify_db_file_test = ModifyDbFileTest::new(db_name, true);
        modify_db_file_test.test_case().setup();

        modify_db_file_test.test_case().trace_exception("");

        let data_num = 10;
        let has_back_up = modify_db_file_test.test_case().has_back_up();

        if !has_back_up {
            modify_db_file_test.test_case().insert_objects(data_num);

            modify_db_file_test.test_case().database().backup().unwrap();
        } else {
            modify_db_file_test.modify_db_file();

            let _ = modify_db_file_test.test_case().database().retrieve(Some(
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

            modify_db_file_test.test_case().insert_objects(data_num);

            assert_eq!(
                modify_db_file_test.test_case().get_all_objects().len() as i32,
                data_num * 2
            );

            modify_db_file_test.test_case().teardown(true);
        }
    }
}
