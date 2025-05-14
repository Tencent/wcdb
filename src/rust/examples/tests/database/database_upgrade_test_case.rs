use wcdb_derive::WCDBTableCoding;

#[derive(WCDBTableCoding)]
#[WCDBTable(
    multi_primaries(columns = ["target_id", "category_id", "channel_id"])
)]
pub struct ConversationTableV1 {
    #[WCDBField]
    pub target_id: String,
    #[WCDBField]
    pub category_id: String,
    #[WCDBField]
    pub channel_id: String,
    #[WCDBField]
    pub conversation_title: String,
    #[WCDBField]
    pub is_top: bool,
    #[WCDBField]
    pub last_time: i64,
}
impl ConversationTableV1 {
    pub fn insert(target_id: &str) -> Self {
        ConversationTableV1 {
            target_id: target_id.to_string(),
            category_id: "category_1".to_string(),
            channel_id: "channel_1".to_string(),
            conversation_title: "default".to_string(),
            is_top: true,
            last_time: 17900000000,
        }
    }
}

#[derive(WCDBTableCoding)]
#[WCDBTable(
    multi_primaries(columns = ["target_id", "category_id", "channel_id"])
)]
pub struct ConversationTableV1_1 {
    #[WCDBField]
    pub target_id: String,
    #[WCDBField]
    pub category_id: String,
    #[WCDBField]
    pub channel_id: String,
    #[WCDBField]
    pub conversation_title: String,
    #[WCDBField(default(i32_value = 100))]
    pub a1: i32,
    #[WCDBField]
    pub a2: i32,
    #[WCDBField(default(i32_value = 100))]
    pub a3: i32,
}

#[derive(WCDBTableCoding)]
#[WCDBTable(
    multi_primaries(columns = ["target_id", "category_id", "channel_id"])
)]
pub struct ConversationTableV1_2 {
    #[WCDBField]
    pub target_id: String,
    #[WCDBField]
    pub category_id: String,
    #[WCDBField]
    pub channel_id: String,
}
impl ConversationTableV1_2 {
    pub(crate) fn new() -> Self {
        ConversationTableV1_2 {
            target_id: "test3".to_string(),
            category_id: "category_1".to_string(),
            channel_id: "channel_1".to_string(),
        }
    }
}

#[derive(WCDBTableCoding)]
#[WCDBTable(
    multi_primaries(columns = ["target_id", "category_id", "channel_id"])
)]
pub struct ConversationTableV2 {
    #[WCDBField]
    id: i32,
    #[WCDBField]
    pub target_id: String,
    #[WCDBField]
    pub category_id: String,
    #[WCDBField]
    pub channel_id: String,
    #[WCDBField]
    pub conversation_title: String,
    #[WCDBField]
    pub is_top: bool,
    #[WCDBField]
    pub last_time: i64,
    #[WCDBField]
    pub channel_type: i32,
    #[WCDBField]
    pub draft_message: String,
}
impl ConversationTableV2 {
    pub fn insert() -> Self {
        ConversationTableV2 {
            id: 0,
            target_id: "t1".to_string(),
            category_id: "category_1".to_string(),
            channel_id: "channel_1".to_string(),
            conversation_title: "default".to_string(),
            is_top: true,
            last_time: 17900000000,
            channel_type: 0,
            draft_message: "".to_string(),
        }
    }
}

#[derive(WCDBTableCoding)]
#[WCDBTable(
    multi_primaries(columns = ["target_id", "category_id", "channel_id"]),
    multi_indexes(name = "index1", columns = ["target_id", "category_id", "channel_id"]),
)]
pub struct ConversationTableV2_1 {
    #[WCDBField(is_primary = true, is_auto_increment = true)]
    id: i32,
    #[WCDBField]
    pub target_id: String,
    #[WCDBField]
    pub category_id: String,
    #[WCDBField]
    pub channel_id: String,
    #[WCDBField]
    pub conversation_title: String,
    #[WCDBField]
    pub is_top: bool,
    #[WCDBField]
    pub last_time: i64,
    #[WCDBField]
    pub channel_type: i32,
    #[WCDBField]
    pub draft_message: String,
}

#[derive(WCDBTableCoding)]
#[WCDBTable(
    multi_primaries(columns = ["target_id", "category_id", "channel_id"])
)]
pub struct ConversationTableV3 {
    #[WCDBField]
    pub target_id: String,
    #[WCDBField]
    pub category_id: String,
    #[WCDBField]
    pub channel_id: String,
    #[WCDBField]
    pub conversation_title: String,
    #[WCDBField]
    pub is_top: bool,
    #[WCDBField(default(i32_value = 3))]
    pub status: i32,
    #[WCDBField(default(text_value = "default"))]
    pub extra_column1: String,
}
impl ConversationTableV3 {
    pub fn insert() -> Self {
        ConversationTableV3 {
            target_id: "t1".to_string(),
            category_id: "category_1".to_string(),
            channel_id: "channel_1".to_string(),
            conversation_title: "default".to_string(),
            is_top: true,
            status: 0,
            extra_column1: "".to_string(),
        }
    }
}

#[derive(WCDBTableCoding)]
#[WCDBTable]
pub struct MessageTableV1 {
    #[WCDBField]
    target_id: String,
    #[WCDBField]
    category_id: String,
    #[WCDBField]
    content: String,
}

impl MessageTableV1 {
    pub fn insert(target_id: &str) -> Self {
        MessageTableV1 {
            target_id: target_id.to_string(),
            category_id: "category_1".to_string(),
            content: "insert_content".to_string(),
        }
    }
}

#[derive(WCDBTableCoding)]
#[WCDBTable]
pub struct MessageTableV1_1 {
    #[WCDBField(is_primary = true)]
    target_id: String,
    #[WCDBField]
    category_id: String,
    #[WCDBField]
    content: String,
}

#[derive(WCDBTableCoding)]
#[WCDBTable]
pub struct TagTableV1 {
    #[WCDBField]
    content: String,
}
impl TagTableV1 {
    pub fn new() -> Self {
        TagTableV1 {
            content: "tag".to_string(),
        }
    }
}

#[cfg(test)]
pub mod database_upgrade_test {
    use crate::database::database_upgrade_test_case::{
        ConversationTableV1, ConversationTableV1_2, DbConversationTableV1, DbConversationTableV1_2,
        DbMessageTableV1, DbTagTableV1, MessageTableV1, TagTableV1,
        DBCONVERSATIONTABLEV1_1_INSTANCE, DBCONVERSATIONTABLEV1_2_INSTANCE,
        DBCONVERSATIONTABLEV1_INSTANCE, DBCONVERSATIONTABLEV2_1_INSTANCE,
        DBCONVERSATIONTABLEV2_INSTANCE, DBCONVERSATIONTABLEV3_INSTANCE,
        DBMESSAGETABLEV1_1_INSTANCE, DBMESSAGETABLEV1_INSTANCE, DBTAGTABLEV1_INSTANCE,
    };
    use std::fmt::Pointer;
    use std::panic::AssertUnwindSafe;
    use std::{panic, thread};
    use wcdb::core::database::Database;
    use wcdb::core::handle_orm_operation::HandleORMOperationTrait;
    use wcdb::core::table_orm_operation::TableORMOperationTrait;
    use wcdb::orm::table_binding::TableBinding;
    use wcdb::winq::column::Column;
    use wcdb::winq::identifier::IdentifierTrait;
    use wcdb::winq::statement_alter_table::StatementAlterTable;
    use wcdb::winq::statement_create_index::StatementCreateIndex;
    use wcdb::winq::statement_drop_index::StatementDropIndex;
    use wcdb::winq::statement_drop_table::StatementDropTable;

    #[test]
    pub fn upgrade() {
        {
            let database = Database::new("./tests/database/custom/upgrade_db.sqlite3");
            database.remove_files().unwrap();
        }
        let database = Database::new("./tests/database/custom/upgrade_db.sqlite3");
        database
            .create_table("ConversationTable", &*DBCONVERSATIONTABLEV1_INSTANCE)
            .unwrap();
        // insert
        let conversation_table =
            database.get_table("ConversationTable", &*DBCONVERSATIONTABLEV1_INSTANCE);
        let insert_result = conversation_table.insert_object(
            ConversationTableV1::insert("t1"),
            DbConversationTableV1::all_fields(),
        );
        assert!(insert_result.is_ok());
        let insert_result = conversation_table.insert_object(
            ConversationTableV1::insert("t2"),
            DbConversationTableV1::all_fields(),
        );
        assert!(insert_result.is_ok());
        database.close(Some(|| {}));

        upgrade_to_v2();
        upgrade_to_v3();
        upgrade_to_v4();
        upgrade_to_v5();
        upgrade_to_v6();

        {
            let database = Database::new("./tests/database/custom/upgrade_db.sqlite3");
            database.remove_files().unwrap();
        }
    }

    // 从V1升级到V2，升级内容：
    //      1.ConversationTable 表增加 channel_type、draft_message、id，字段无默认值
    //      2.id 字段增加自增主键约束
    //      3.给 "target_id", "category_id", "channel_id" 增加索引
    fn upgrade_to_v2() {
        let database = Database::new("./tests/database/custom/upgrade_db.sqlite3");
        database
            .create_table("ConversationTable", &*DBCONVERSATIONTABLEV2_INSTANCE)
            .unwrap();
        let conversation_table =
            database.get_table("ConversationTable", &*DBCONVERSATIONTABLEV2_INSTANCE);
        let result = conversation_table.get_all_objects();
        assert!(result.is_ok());
        match result {
            Ok(vec) => {
                for table_v2_item in vec {
                    assert_eq!(table_v2_item.channel_type, 0);
                    assert_eq!(table_v2_item.draft_message, "");
                }
            }
            Err(_) => {}
        }
        database.close(Some(|| {}));

        // id 字段增加自增主键约束
        let database = Database::new("./tests/database/custom/upgrade_db.sqlite3");
        database
            .create_table("ConversationTable", &*DBCONVERSATIONTABLEV2_1_INSTANCE)
            .unwrap();
        let field_id = unsafe { &*(&*DBCONVERSATIONTABLEV2_1_INSTANCE).id };
        assert!(field_id.is_auto_increment());
        assert!(field_id.is_primary_key());
        database.close(Some(|| {}));

        // 验证删除索引
        let database = Database::new("./tests/database/custom/upgrade_db.sqlite3");
        let statement_drop_index = StatementDropIndex::new();
        statement_drop_index.drop_index("index1");
        assert_eq!("DROP INDEX index1", statement_drop_index.get_description());
        database.execute(&statement_drop_index).unwrap();
        database.close(Some(|| {}));

        // 手动创建索引
        let database = Database::new("./tests/database/custom/upgrade_db.sqlite3");
        let statement_create_index = StatementCreateIndex::new();
        let column1 = Column::new("target_id");
        let statement = statement_create_index
            .create_index("my_index")
            .on("ConversationTable")
            .indexed_by(vec![&column1]);
        assert_eq!(
            statement.get_description(),
            "CREATE INDEX my_index ON ConversationTable(target_id)"
        );
        database.execute(statement).unwrap();
        database.close(Some(|| {}));
    }

    // 从V2升级到V3，升级内容：
    //      1.ConversationTable 表增加 status、extra_column1 字段，且有默认值
    //      2.删除 last_time 字段
    //      3.重命名字段 is_top 为 rename_is_top
    fn upgrade_to_v3() {
        let database = Database::new("./tests/database/custom/upgrade_db.sqlite3");
        database
            .create_table("ConversationTable", &*DBCONVERSATIONTABLEV3_INSTANCE)
            .unwrap();
        let conversation_table =
            database.get_table("ConversationTable", &*DBCONVERSATIONTABLEV3_INSTANCE);
        let result = conversation_table.get_all_objects();
        assert!(result.is_ok());
        match result {
            Ok(vec) => {
                for table_v3_item in vec {
                    assert_eq!(table_v3_item.status, 3);
                    assert_eq!(table_v3_item.extra_column1, "default");
                }
            }
            Err(_) => {}
        }

        // 删除字段检查
        let fields = unsafe { &*DBCONVERSATIONTABLEV3_INSTANCE.all_binding_fields() };
        for x in fields {
            assert_ne!(x.get_name(), "last_time");
        }

        // 重命名字段
        let statement_alter_table = StatementAlterTable::new();
        let column_is_top = Column::new("is_top");
        let column_rename_is_top = Column::new("rename_is_top");
        let statement = statement_alter_table
            .alter_table("ConversationTable")
            .rename_column(&column_is_top)
            .to_column(&column_rename_is_top);
        database.execute(statement).unwrap();

        database.close(Some(|| {}));
    }

    // 从V3升级到V4，升级内容：
    //      1.增加MessageTable表
    //      2.给表增加主键
    //      3.修改表名
    //      4.
    fn upgrade_to_v4() {
        let database = Database::new("./tests/database/custom/upgrade_db.sqlite3");
        database
            .create_table("MessageTable", &*DBMESSAGETABLEV1_INSTANCE)
            .unwrap();
        let msg_table = database.get_table("MessageTable", &*DBMESSAGETABLEV1_INSTANCE);
        // insert
        let insert_result =
            msg_table.insert_object(MessageTableV1::insert("t1"), DbMessageTableV1::all_fields());
        assert!(insert_result.is_ok());

        let insert_result =
            msg_table.insert_object(MessageTableV1::insert("t2"), DbMessageTableV1::all_fields());
        assert!(insert_result.is_ok());

        let result = msg_table.get_all_objects();
        assert!(result.is_ok());
        match result {
            Ok(vec) => {
                for msg_v1_item in vec {
                    assert_eq!(msg_v1_item.content, "insert_content");
                }
            }
            Err(_) => {}
        }
        database.close(Some(|| {}));

        // 2.给表增加主键
        let database = Database::new("./tests/database/custom/upgrade_db.sqlite3");
        database
            .create_table("MessageTable", &*DBMESSAGETABLEV1_1_INSTANCE)
            .unwrap();

        let target_id = unsafe { &*(*&DBMESSAGETABLEV1_1_INSTANCE.target_id) };
        assert!(target_id.is_primary_key());

        // 3.修改表名
        let database = Database::new("./tests/database/custom/upgrade_db.sqlite3");
        // database
        //     .execute_sql("ALTER TABLE MessageTable RENAME TO MsgTable")
        //     .unwrap();

        let statement = StatementAlterTable::new();
        statement.alter_table("MessageTable").rename_to("MsgTable");
        database.execute(&statement).unwrap();

        let msg_table = database.get_table("MsgTable", &*DBMESSAGETABLEV1_1_INSTANCE);
        let result = msg_table.get_all_objects();
        assert!(result.is_ok());
        match result {
            Ok(vec) => {
                let mut is_t1: bool = false;
                let mut is_t2: bool = false;
                for msg_v1_item in vec {
                    if msg_v1_item.target_id == "t1" {
                        is_t1 = true;
                    } else if msg_v1_item.target_id == "t2" {
                        is_t2 = true;
                    }
                }
                assert!(is_t1);
                assert!(is_t2);
            }
            Err(_) => {}
        }
        database.close(Some(|| {}));
    }

    // 1.删除 TagTable 表
    fn upgrade_to_v5() {
        let database = Database::new("./tests/database/custom/upgrade_db.sqlite3");
        database
            .create_table("TagTable", &*DBTAGTABLEV1_INSTANCE)
            .unwrap();
        let tag_table = database.get_table("TagTable", &*DBTAGTABLEV1_INSTANCE);
        // insert
        let insert_result = tag_table.insert_object(TagTableV1::new(), DbTagTableV1::all_fields());
        assert!(insert_result.is_ok());

        let insert_result = tag_table.insert_object(TagTableV1::new(), DbTagTableV1::all_fields());
        assert!(insert_result.is_ok());

        // 删除表
        let statement = StatementDropTable::new();
        statement.drop_table("TagTable").if_exist();
        assert_eq!("DROP TABLE IF EXISTS TagTable", statement.get_description());
        database.execute(&statement).unwrap();

        let result = tag_table.get_all_objects();
        match result {
            Ok(tag_vec) => {
                assert!(tag_vec.is_empty());
            }
            Err(_) => {}
        }
        database.close(Some(|| {}));
    }

    // 升级中断
    fn upgrade_to_v6() {
        {
            let database = Database::new("./tests/database/custom/upgrade_db.sqlite3");
            database.remove_files().unwrap();
        }
        let database = Database::new("./tests/database/custom/upgrade_db.sqlite3");
        database
            .create_table("ConversationTable", &*DBCONVERSATIONTABLEV1_INSTANCE)
            .unwrap();
        // insert
        let conversation_table =
            database.get_table("ConversationTable", &*DBCONVERSATIONTABLEV1_INSTANCE);
        let mut vec: Vec<ConversationTableV1> = Vec::with_capacity(100);
        let length = 10000;
        for x in 0..length {
            vec.push(ConversationTableV1::insert(
                &*("x".to_string() + &*String::from(x.to_string())),
            ));
        }
        let insert_result =
            conversation_table.insert_objects(vec, DbConversationTableV1::all_fields());
        assert!(insert_result.is_ok());

        database.close(Some(|| {}));

        // 模拟升级崩溃，ConversationTableV1_1 结构体增加了3个字段，删除了2个字段
        let handle = thread::spawn(move || {
            let database = Database::new("./tests/database/custom/upgrade_db.sqlite3");
            let result = panic::catch_unwind(AssertUnwindSafe(|| {
                database
                    .create_table("ConversationTable", &*DBCONVERSATIONTABLEV1_1_INSTANCE)
                    .unwrap();
            }));
            if let Err(e) = result {}
        });
        thread::sleep(std::time::Duration::from_millis(100));
        let database = Database::new("./tests/database/custom/upgrade_db.sqlite3");
        let is_exist = database.table_exist("ConversationTable").unwrap();
        assert!(is_exist);
        handle.join().unwrap();
        let conversation_table =
            database.get_table("ConversationTable", &*DBCONVERSATIONTABLEV1_1_INSTANCE);
        let result = conversation_table.get_all_objects();
        assert!(result.is_ok());
        match result {
            Ok(vec) => {
                assert_eq!(vec.len(), length);
            }
            Err(_) => {}
        }
        database.close(Some(|| {}));

        // 模拟从6个字段的表降级为3个字段的表
        // 结论：
        //  1.其他3个字段任然在表里 数据也在
        //  2.当给3个字段的结构体代表的表插入数据时，其他字段数据为空
        let database = Database::new("./tests/database/custom/upgrade_db.sqlite3");
        let statement = StatementDropTable::new();
        statement.drop_table("ConversationTable").if_exist();
        database.execute(&statement).unwrap();
        database
            .create_table("ConversationTable", &*DBCONVERSATIONTABLEV1_INSTANCE)
            .unwrap();
        // insert
        let conversation_table =
            database.get_table("ConversationTable", &*DBCONVERSATIONTABLEV1_INSTANCE);
        let mut vec: Vec<ConversationTableV1> = Vec::with_capacity(100);
        let length = 10;
        for x in 0..length {
            vec.push(ConversationTableV1::insert(
                &*("x".to_string() + &*String::from(x.to_string())),
            ));
        }
        let insert_result =
            conversation_table.insert_objects(vec, DbConversationTableV1::all_fields());
        assert!(insert_result.is_ok());

        database.close(Some(|| {}));

        let database = Database::new("./tests/database/custom/upgrade_db.sqlite3");
        database
            .create_table("ConversationTable", &*DBCONVERSATIONTABLEV1_2_INSTANCE)
            .unwrap();
        let conversation_table =
            database.get_table("ConversationTable", &*DBCONVERSATIONTABLEV1_2_INSTANCE);
        let result = conversation_table.get_all_objects();
        assert!(result.is_ok());
        let insert_result = conversation_table.insert_object(
            ConversationTableV1_2::new(),
            DbConversationTableV1_2::all_fields(),
        );
        assert!(insert_result.is_ok());

        let conversation_table =
            database.get_table("ConversationTable", &*DBCONVERSATIONTABLEV1_2_INSTANCE);
        let result = conversation_table.get_all_objects();
        assert!(result.is_ok());

        database.close(Some(|| {}));
    }
}
