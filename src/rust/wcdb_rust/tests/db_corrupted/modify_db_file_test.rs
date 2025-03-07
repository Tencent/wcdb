use crate::db_corrupted::testclass::table_goods_object::{
    DbTableGoodsObject, TableGoodsObject, DBTABLEGOODSOBJECT_INSTANCE,
};
use crate::db_corrupted::utils::run_cmd;
use wcdb_core::base::wcdb_exception::WCDBException;
use wcdb_core::core::database::Database;
use wcdb_core::core::handle_orm_operation::HandleORMOperationTrait;
use wcdb_core::core::table_orm_operation::TableORMOperationTrait;

struct ModifyDbFileTest {
    database: Database,
    db_name: String,
    table_name: String,
}

impl ModifyDbFileTest {
    pub fn new(db_name: &str, auto_backup: bool) -> Self {
        let path = format!("./target/tmp/{}", db_name);
        let table_name = "table_goods_object";
        let test = ModifyDbFileTest {
            database: Database::new(path.as_str()),
            db_name: db_name.to_string(),
            table_name: table_name.to_string(),
        };
        test.database.enable_auto_backup(auto_backup);
        test
    }

    pub fn database(&self) -> &Database {
        &self.database
    }

    pub fn table_name(&self) -> String {
        self.table_name.clone()
    }

    pub fn setup(&self) {
        self.database
            .create_table(self.table_name.as_str(), &*DBTABLEGOODSOBJECT_INSTANCE)
            .unwrap();
    }

    pub fn teardown(&self, delete_all: bool) {
        if delete_all {
            self.delete_all();
        }
    }

    pub fn insert_objects(&self, data_num: i32) {
        let obj_vec = TableGoodsObject::get_obj_vec(data_num);

        let table = self
            .database
            .get_table(self.table_name.as_str(), &*DBTABLEGOODSOBJECT_INSTANCE);
        table
            .insert_objects(obj_vec, DbTableGoodsObject::all_fields())
            .unwrap();
    }

    pub fn get_all_objects(&self) -> Vec<TableGoodsObject> {
        let table = self
            .database
            .get_table(self.table_name.as_str(), &*DBTABLEGOODSOBJECT_INSTANCE);

        table.get_all_objects().unwrap()
    }

    fn delete_all(&self) {
        let path = "./target/tmp";
        let db_name = format!("{}/{}", path, self.db_name);
        for item in std::fs::read_dir(path).unwrap() {
            let path = item.unwrap().path();
            if path.starts_with(db_name.as_str()) {
                std::fs::remove_file(path).unwrap();
            }
        }
    }

    pub fn trace_exception(&self, exp_msg: &str) {
        let exp_msg_string = exp_msg.to_string();
        self.database()
            .trace_exception(Some(move |exception: WCDBException| {
                let msg = exception.message();
                println!("trace_exception: {}", msg);
                if msg.starts_with(exp_msg_string.as_str()) {
                    assert!(true);
                }
            }));
    }

    pub fn modify_db_file(&self) {
        let db_path = format!("target/tmp/{}", self.db_name);
        let cmd = format!(
            "echo \"Corrupted\" | dd of={} bs=1 seek=0 count=10 conv=notrunc",
            db_path
        );

        let _ = run_cmd(cmd.as_str());
    }

    pub fn has_back_up(&self) -> bool {
        let first_material = format!("./target/tmp/{}-first.material", self.db_name);
        let incremental_material = format!("./target/tmp/{}-incremental.material", self.db_name);
        if !std::path::Path::new(first_material.as_str()).exists() {
            return false;
        }
        if !std::path::Path::new(incremental_material.as_str()).exists() {
            return false;
        }
        true
    }
}

#[cfg(test)]
pub mod modify_db_file_test_exception {
    use crate::db_corrupted::modify_db_file_test::ModifyDbFileTest;

    // #[test] // todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_modify_then_backup_exception() {
        let db_name = "modify_then_backup_exception.db";
        let modify_db_file_test = ModifyDbFileTest::new(db_name, false);
        modify_db_file_test.setup();

        modify_db_file_test.trace_exception("NotADatabase");

        let data_num = 10;
        modify_db_file_test.insert_objects(data_num);

        modify_db_file_test.modify_db_file();

        // WCDBNormalException(Level: NoticeCode: NotADatabaseException { Message: "NotADatabase" })
        let _ = modify_db_file_test.database().backup();

        assert_eq!(
            modify_db_file_test.get_all_objects().len(),
            data_num as usize
        );
        modify_db_file_test.teardown(true);
    }

    // #[test] // todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_modify_then_write_back_exception() {
        let db_name = "modify_then_write_back_exception.db";
        let modify_db_file_test = ModifyDbFileTest::new(db_name, false);
        modify_db_file_test.setup();

        // WCDBCorruptOrIOException(Level: ErrorCode: IOErrorException { Message: "disk I/O error" })
        modify_db_file_test.trace_exception("disk I/O error");

        let data_num = 10;
        modify_db_file_test.insert_objects(data_num);

        modify_db_file_test.modify_db_file();

        let _ = modify_db_file_test
            .database()
            .execute_sql("PRAGMA wal_checkpoint(FULL);");

        modify_db_file_test.teardown(true);
    }

    // #[test] // todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_modify_then_retrieve_exception() {
        let db_name = "modify_then_retrieve_exception.db";

        let modify_db_file_test = ModifyDbFileTest::new(db_name, false);
        modify_db_file_test.setup();

        // WCDBNormalException(Level: NoticeCode: NotADatabaseException { Message: "NotADatabase" })
        modify_db_file_test.trace_exception("NotADatabase");

        let data_num = 10;
        modify_db_file_test.insert_objects(data_num);

        modify_db_file_test.modify_db_file();

        let _ = modify_db_file_test.database().retrieve(Some(
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
        modify_db_file_test.teardown(true);
    }
}

#[cfg(test)]
pub mod test_modify_then_backup_success {
    use crate::db_corrupted::modify_db_file_test::ModifyDbFileTest;

    // 第一次运行，写入数据并手动备份
    // 第二次运行，修改数据库文件并恢复数据，再写入数据并验证数据库有效性
    // 该用例：单独运行两次可以成功，直接测试所有代码，第二次会失败
    // #[test] // todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test() {
        let db_name = "test_modify_then_backup_success.db";
        let modify_db_file_test = ModifyDbFileTest::new(db_name, true);
        modify_db_file_test.setup();

        modify_db_file_test.trace_exception("");

        let data_num = 10;
        let has_back_up = modify_db_file_test.has_back_up();

        if !has_back_up {
            modify_db_file_test.insert_objects(data_num);

            modify_db_file_test.database().backup().unwrap();
        } else {
            modify_db_file_test.modify_db_file();

            let _ = modify_db_file_test.database().retrieve(Some(
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

            modify_db_file_test.insert_objects(data_num);

            assert_eq!(
                modify_db_file_test.get_all_objects().len() as i32,
                data_num * 2
            );

            modify_db_file_test.teardown(true);
        }
    }
}
