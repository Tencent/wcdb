use crate::db_corrupted::corrupted_base_test_case::CorruptedBaseTestCase;
use crate::db_corrupted::testclass::table_goods_object::{DbTableGoodsObject, TableGoodsObject};
use crate::db_corrupted::utils::run_cmd;
use wcdb::core::handle_orm_operation::HandleORMOperationTrait;

struct TerminatedWhenWriteTest {
    test_case: CorruptedBaseTestCase,
}

impl TerminatedWhenWriteTest {
    pub fn new(db_name: &str, auto_backup: bool) -> Self {
        let test_case = CorruptedBaseTestCase::new(db_name, auto_backup);
        TerminatedWhenWriteTest { test_case }
    }

    pub fn test_case(&self) -> &CorruptedBaseTestCase {
        &self.test_case
    }

    // 强制终止写操作，todo 是否可以恢复？
    pub fn terminated_when_write(&self) {
        let table_name = self.test_case().table_name();
        let sql = "BEGIN IMMEDIATE";
        self.test_case().database().execute_sql(sql).unwrap();
        let obj = TableGoodsObject::new();
        self.test_case()
            .database()
            .insert_object(obj, DbTableGoodsObject::all_fields(), table_name.as_str())
            .unwrap();
        let obj = TableGoodsObject::new();
        self.test_case()
            .database()
            .insert_object(obj, DbTableGoodsObject::all_fields(), table_name.as_str())
            .unwrap();

        let sql = "PRAGMA journal_mode=DELETE;";
        self.test_case().database().execute_sql(sql).unwrap();
        let sql = "PRAGMA synchronous=OFF;";
        self.test_case().database().execute_sql(sql).unwrap();

        let kill_cmd = "os.kill(os.getpid(), 9)";
        let _ = run_cmd(kill_cmd);
    }
}

pub mod terminated_when_write_test_exception {
    use super::*;

    // 该用例比较特殊，执行完之后会强制终止进程，所以该用例无法恢复
    // 执行该用例，明确终止写操作并终止进程，再执行其他用户验证与恢复
    // #[test] // todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_terminated_when_write_operation() {
        let db_name = "terminated_when_write_then_backup_exception.db";
        let terminated_when_write_test = TerminatedWhenWriteTest::new(db_name, false);
        terminated_when_write_test.test_case().setup();

        let data_num = 10;
        terminated_when_write_test
            .test_case()
            .insert_objects(data_num);

        terminated_when_write_test.terminated_when_write();
    }

    // #[test] // todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_terminated_when_write_operation_check() {
        let db_name = "terminated_when_write_then_backup_exception.db";
        let terminated_when_write_test = TerminatedWhenWriteTest::new(db_name, false);
        terminated_when_write_test.test_case().setup();

        let ret = terminated_when_write_test.test_case().get_all_objects();
        assert!(ret.len() > 0);
    }

    // #[test] // todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_terminated_when_write_then_write_back_exception() {
        let db_name = "terminated_when_write_then_backup_exception.db";
        let terminated_when_write_test = TerminatedWhenWriteTest::new(db_name, false);
        terminated_when_write_test.test_case().setup();

        // WCDBCorruptOrIOException(Level: ErrorCode: IOErrorException { Message: "disk I/O error" })
        let sql = "PRAGMA wal_checkpoint(FULL);";
        terminated_when_write_test
            .test_case()
            .database()
            .execute_sql(sql)
            .unwrap();
    }
}

pub mod terminated_when_write_test_success {
    use crate::db_corrupted::terminated_when_write_test::TerminatedWhenWriteTest;

    // #[test] // todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_terminated_when_write_then_backup_success() {
        let db_name = "terminated_when_write_then_backup_exception.db";
        let terminated_when_write_test = TerminatedWhenWriteTest::new(db_name, false);
        terminated_when_write_test.test_case().setup();

        terminated_when_write_test
            .test_case()
            .database()
            .backup()
            .unwrap();

        let ret = terminated_when_write_test.test_case().get_all_objects();
        assert!(ret.len() > 0);
    }

    // #[test] // todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_terminated_when_write_then_write_back_success() {
        let db_name = "terminated_when_write_then_backup_exception.db";
        let terminated_when_write_test = TerminatedWhenWriteTest::new(db_name, false);
        terminated_when_write_test.test_case().setup();

        let _ = terminated_when_write_test
            .test_case()
            .database()
            .retrieve(Some(move |percentage: f64, increment: f64| {
                println!(
                    "Database retrieve percentage:{} , increment:{}",
                    percentage, increment
                );
                if percentage >= 1.0 {
                    println!("Database retrieve complete");
                }
                true
            }));

        let sql = "PRAGMA wal_checkpoint(FULL);";
        terminated_when_write_test
            .test_case()
            .database()
            .execute_sql(sql)
            .unwrap();

        let ret = terminated_when_write_test.test_case().get_all_objects();
        assert_eq!(ret.len(), 10);
    }
}
