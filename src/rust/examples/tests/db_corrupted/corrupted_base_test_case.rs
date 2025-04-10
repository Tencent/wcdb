use crate::db_corrupted::testclass::table_goods_object::{
    DbTableGoodsObject, TableGoodsObject, DBTABLEGOODSOBJECT_INSTANCE,
};
use wcdb::base::wcdb_exception::WCDBException;
use wcdb::core::database::Database;
use wcdb::core::handle_orm_operation::HandleORMOperationTrait;
use wcdb::core::table_orm_operation::TableORMOperationTrait;

pub struct CorruptedBaseTestCase {
    database: Database,
    db_name: String,
    table_name: String,
}

impl CorruptedBaseTestCase {
    pub fn new(db_name: &str, auto_backup: bool) -> Self {
        let path = format!("./target/tmp/{}", db_name);
        let table_name = "table_goods_object";
        let test = CorruptedBaseTestCase {
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

    pub fn db_name(&self) -> String {
        self.db_name.clone()
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
        let ret = self
            .database()
            .trace_exception(Some(move |exception: WCDBException| {
                let msg = exception.message();
                println!("trace_exception: {}", msg);
                if msg.starts_with(exp_msg_string.as_str()) {
                    assert!(true);
                }
            }));
        assert!(ret.is_ok());
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
