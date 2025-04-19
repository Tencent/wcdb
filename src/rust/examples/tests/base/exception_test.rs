use wcdb::base::basic_types::WCDBBasicTypes;
use wcdb::base::value::Value;
use wcdb::winq::column::Column;
use wcdb_derive::WCDBTableCoding;

#[derive(WCDBTableCoding, Debug)]
#[WCDBTable(
    multi_primaries(columns = ["category", "target_id", "channel_id"])
)]
pub struct ExceptionObject {
    #[WCDBField]
    pub category: i32,
    #[WCDBField]
    pub target_id: String,
    #[WCDBField]
    pub channel_id: String,
    #[WCDBField]
    pub value: String,
}

impl ExceptionObject {
    pub fn get_object() -> ExceptionObject {
        ExceptionObject {
            category: 1,
            target_id: "target_id".to_string(),
            channel_id: "channel_id".to_string(),
            value: "value".to_string(),
        }
    }

    pub fn get_values(&self) -> Vec<Value> {
        vec![
            Value::from(self.category as i64),
            Value::from(self.target_id.as_str()),
            Value::from(self.channel_id.as_str()),
            Value::from(self.value.as_str()),
        ]
    }

    pub fn get_all_columns() -> Vec<Column> {
        vec![
            Column::new("category"),
            Column::new("target_id"),
            Column::new("channel_id"),
            Column::new("value"),
        ]
    }
}

#[cfg(test)]
pub mod exception_test {
    use crate::base::exception_test::{ExceptionObject, DBEXCEPTIONOBJECT_INSTANCE};
    use wcdb::base::wcdb_exception::ExceptionExtendCode;
    use wcdb::core::database::Database;
    use wcdb::core::handle_orm_operation::HandleORMOperationTrait;
    use wcdb::core::table_operation::TableOperation;

    #[test]
    pub fn test() {
        let db_path = "./target/tmp/exception_test.db";
        let database = Database::new(db_path);

        let table_name = "test_table";
        // 需要删除表，验证没有表的情况。
        let _ = database.drop_table(table_name);

        /// 验证没有表的情况下，插入数据包错。
        let operation = TableOperation::new(table_name, &database);
        let obj = ExceptionObject::get_object();
        let ret = operation.insert_rows(vec![obj.get_values()], ExceptionObject::get_all_columns());
        assert!(ret.is_err());

        let error = ret.unwrap_err();
        assert_eq!(error.message(), "no such table: test_table");
        assert!(error.path().contains("exception_test.db"));
        assert_eq!(
            error.sql(),
            "INSERT INTO test_table(category, target_id, channel_id, value) VALUES(?1, ?2, ?3, ?4)"
        );
        assert_eq!(error.extend_code(), ExceptionExtendCode::Unknown);

        // 创建表
        let ret = database.create_table(table_name, &*DBEXCEPTIONOBJECT_INSTANCE);
        assert!(ret.is_ok());

        /// 验证重复插入数据。
        // 第一次插入数据。
        let values = obj.get_values();
        let ret = operation.insert_rows(vec![values], ExceptionObject::get_all_columns());
        assert!(ret.is_ok());

        // 第二次插入数据。
        let values = obj.get_values();
        let ret = operation.insert_rows(vec![values], ExceptionObject::get_all_columns());
        assert!(ret.is_err());

        let error = ret.unwrap_err();
        assert_eq!(error.message(), "UNIQUE constraint failed: test_table.category, test_table.target_id, test_table.channel_id");
        assert!(error.path().contains("exception_test.db"));
        assert_eq!(
            error.sql(),
            "INSERT INTO test_table(category, target_id, channel_id, value) VALUES(?1, ?2, ?3, ?4)"
        );
        assert_eq!(
            error.extend_code(),
            ExceptionExtendCode::ConstraintPrimaryKey
        );
    }
}
