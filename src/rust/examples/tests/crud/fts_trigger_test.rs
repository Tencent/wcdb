use wcdb::core::handle_operation::HandleOperationTrait;
use wcdb_derive::WCDBTableCoding;

pub static TABLE_MESSAGE_NAME: &str = "table_message";

#[derive(WCDBTableCoding)]
pub struct TableMessage {
    #[WCDBField(is_primary = true, is_auto_increment = true)]
    id: i64,
    #[WCDBField]
    content: String,
    #[WCDBField]
    search_word: String,
}

impl TableMessage {
    pub fn new() -> Self {
        Self {
            id: 1,
            content: "".to_string(),
            search_word: "".to_string(),
        }
    }
}

pub static TABLE_MESSAGE_FTS_PINYIN_NAME: &str = "table_message_fts_pinyin";

#[derive(WCDBTableCoding, PartialEq, Clone)]
#[WCDBTable(fts_module(
    version = "FTS5",
    tokenizer = "wcdb_pinyin",
    external_table = "table_message"
))]
pub struct TableMessageFtsPinyin {
    #[WCDBField]
    pub(crate) id: i64,
    #[WCDBField]
    pub(crate) search_word: String,
}

impl TableMessageFtsPinyin {
    pub(crate) fn new(search_word: &str) -> Self {
        Self {
            id: -1,
            search_word: String::from(search_word),
        }
    }
}

pub static TABLE_MESSAGE_FTS_CN_NAME: &str = "table_message_fts_cn";
#[derive(WCDBTableCoding, PartialEq, Clone)]
#[WCDBTable(
    fts_module(version = "FTS5", tokenizer = "wcdb_verbatim", tokenizer_parameters = ["skip_stemming", "chinese_traditional_to_simplified"], external_table = "table_message")
)]
pub struct TableMessageFtsCn {
    #[WCDBField]
    pub(crate) id: i64,
    #[WCDBField]
    pub(crate) search_word: String,
}

impl TableMessageFtsCn {
    pub(crate) fn new(search_word: &str) -> Self {
        Self {
            id: -1,
            search_word: String::from(search_word),
        }
    }
}

#[cfg(test)]
pub mod fts_trigger_test {
    use crate::base::base_test_case::BaseTestCase;
    use crate::crud::fts_trigger_test::{
        DbTableMessage, DbTableMessageFtsCn, DbTableMessageFtsPinyin, TableMessage,
        DB_TABLE_MESSAGE_FTS_CN_INSTANCE, DB_TABLE_MESSAGE_FTS_PINYIN_INSTANCE,
        DB_TABLE_MESSAGE_INSTANCE, TABLE_MESSAGE_FTS_CN_NAME, TABLE_MESSAGE_FTS_PINYIN_NAME,
        TABLE_MESSAGE_NAME,
    };
    use std::collections::HashMap;
    use wcdb::core::database::Database;
    use wcdb::core::handle_operation::HandleOperationTrait;
    use wcdb::core::handle_orm_operation::HandleORMOperationTrait;
    use wcdb::fts::builtin_tokenizer::BuiltinTokenizer;
    use wcdb::winq::expression_operable::ExpressionOperableTrait;
    use wcdb::winq::statement_select::StatementSelect;

    fn setup() -> Database {
        BaseTestCase::global_set_up();
        let db = Database::new("./target/tmp/test.db", None);
        db.remove_files().unwrap();
        config_pinyin_dict();
        create_tables(&db);
        create_trigger(&db);

        db
    }

    #[test]
    fn test_insert() {
        let db = setup();
        let (content, queries) = get_content1();
        insert_data(&db, content);
        sync_all_data(&db);
        search_all(&db, content, &queries, true);

        let (content, queries) = get_content2();
        insert_data(&db, content);
        search_all(&db, content, &queries, true);

        let (content, queries) = get_content3();
        insert_data(&db, content);
        search_all(&db, content, &queries, true);
    }

    #[test]
    fn test_update() {
        let db = setup();
        // 插入新数据，触发器更新虚表。确保能搜到
        let (content1, queries1) = get_content1();
        insert_data(&db, content1);
        search_all(&db, content1, &queries1, true);

        // 更新数据，触发器先删除旧数据，后更新新数据。确保旧数据搜不到，新数据能搜到
        let (content2, queries2) = get_content2();
        update_data(&db, content2);
        search_all(&db, content1, &queries1, false); // 旧数据搜不到
        search_all(&db, content2, &queries2, true); // 新数据能搜到
    }

    #[test]
    fn test_insert_or_replace() {
        let db = setup();
        // 插入新数据，触发器更新虚表。确保能搜到
        let (content1, queries1) = get_content1();
        insert_data(&db, content1);
        search_all(&db, content1, &queries1, true);

        // 替换数据，触发器先删除旧数据，后更新新数据。确保旧数据搜不到，新数据能搜到
        let (content2, queries2) = get_content2();
        insert_or_replace_data(&db, content2);
        search_all(&db, content1, &queries1, false);
        search_all(&db, content2, &queries2, true);
    }

    // 这是一个异常的用例，触发器无法支持 insert_or_ignore
    #[test]
    fn test_insert_or_ignore() {
        let db = setup();
        // 插入新数据，触发器更新虚表。确保能搜到
        let (content1, queries1) = get_content1();
        insert_data(&db, content1);
        search_all(&db, content1, &queries1, true);

        let (content2, queries2) = get_content2();
        insert_or_ignore_data(&db, content2);
        search_all(&db, content2, &queries2, false);
        search_all(&db, content1, &queries1, false); // 此处被错误删除
    }

    #[test]
    fn test_delete() {
        let db = setup();
        // 插入新数据，触发器更新虚表。确保能搜到
        let (content1, queries1) = get_content1();
        insert_data(&db, content1);
        search_all(&db, content1, &queries1, true);

        // 删除数据，触发器更新虚表。确保搜不到
        delete_data(&db, content1);
        search_all(&db, content1, &queries1, false);
    }

    fn search_all(db: &Database, content: &str, queries: &Vec<&str>, with_data: bool) {
        for query in queries {
            let stat = {
                let statement_select_v = StatementSelect::new();
                statement_select_v.select(vec![DbTableMessageFtsPinyin::id()]);
                statement_select_v.from(vec![TABLE_MESSAGE_FTS_PINYIN_NAME]);
                let exp = DbTableMessageFtsPinyin::search_word().match_(query);
                statement_select_v.where_(&exp);
                statement_select_v.union();
                statement_select_v.select(vec![DbTableMessageFtsCn::id()]);
                statement_select_v.from(vec![TABLE_MESSAGE_FTS_CN_NAME]);
                let exp = DbTableMessageFtsPinyin::search_word().match_(query);
                statement_select_v.where_(&exp);
                // let desc = statement_select_v.get_description();
                // println!("statement_select: {}", desc);
                statement_select_v
            };

            let statement_select = StatementSelect::new();
            statement_select.select(DbTableMessage::all_fields());
            statement_select.from(vec![TABLE_MESSAGE_NAME]);
            let exp = DbTableMessage::id().in_statement_select(&stat);
            statement_select.where_(&exp);
            let vec = db.get_all_rows_from_statement(&statement_select).unwrap();
            // let desc = statement_select.get_description();
            // println!("statement_select: {}", desc);
            if with_data {
                assert_eq!(vec.len(), 1);
                assert_eq!(vec[0].len(), 3);
                assert_eq!(vec[0][2].get_text(), content);
            } else {
                assert_eq!(vec.len(), 0);
            }
        }
    }

    fn insert_data(db: &Database, content: &str) {
        let mut msg_box = TableMessage::new();
        msg_box.content = content.to_string();
        msg_box.search_word = content.to_string();
        let mut fields = DbTableMessage::all_fields();
        fields.retain(|field| field.get_name() != "id");
        db.insert_object(msg_box, fields, TABLE_MESSAGE_NAME)
            .unwrap();
    }

    fn update_data(db: &Database, content: &str) {
        let exp = DbTableMessage::id().eq(1);
        let mut msg_box = TableMessage::new();
        msg_box.id = 1;
        msg_box.content = content.to_string();
        msg_box.search_word = content.to_string();
        let fields = DbTableMessage::all_fields();

        db.update_object(
            msg_box,
            fields,
            TABLE_MESSAGE_NAME,
            Some(&exp),
            None,
            None,
            None,
        )
        .unwrap();
    }

    fn insert_or_replace_data(db: &Database, content: &str) {
        let mut msg_box = TableMessage::new();
        msg_box.id = 1;
        msg_box.content = content.to_string();
        msg_box.search_word = content.to_string();
        let fields = DbTableMessage::all_fields();

        db.insert_or_replace_object(msg_box, fields, TABLE_MESSAGE_NAME)
            .unwrap();
    }

    fn insert_or_ignore_data(db: &Database, content: &str) {
        let mut msg_box = TableMessage::new();
        msg_box.id = 1;
        msg_box.content = content.to_string();
        msg_box.search_word = content.to_string();
        let fields = DbTableMessage::all_fields();

        db.insert_or_ignore_object(msg_box, fields, TABLE_MESSAGE_NAME)
            .unwrap();
    }

    fn delete_data(db: &Database, content: &str) {
        let exp = DbTableMessage::content().eq(content);
        db.delete_objects(TABLE_MESSAGE_NAME, Some(&exp), None, None, None)
            .unwrap();
    }

    fn sync_all_data(db: &Database) {
        db.execute_sql(&format!(
            "INSERT INTO {}({}) VALUES('rebuild');",
            TABLE_MESSAGE_FTS_PINYIN_NAME, TABLE_MESSAGE_FTS_PINYIN_NAME
        ))
        .unwrap();

        db.execute_sql(&format!(
            "INSERT INTO {}({}) VALUES('rebuild');",
            TABLE_MESSAGE_FTS_CN_NAME, TABLE_MESSAGE_FTS_CN_NAME
        ))
        .unwrap();
    }

    fn get_content3() -> (&'static str, Vec<&'static str>) {
        let content = "what's wrong with you?";
        let queries = vec!["wrong with you", "what", "wrong", "with", "you"];
        (content, queries)
    }

    fn get_content2() -> (&'static str, Vec<&'static str>) {
        let content = "中国人";
        let queries = vec![
            "中国人",
            "中国",
            "国人",
            "中",
            "国",
            "人",
            "zhong guo ren",
            "zhong guo",
            "guo ren",
            "zhong",
            "guo",
            "ren",
        ];
        (content, queries)
    }

    fn get_content1() -> (&'static str, Vec<&'static str>) {
        let content = "单于骑模具单车";
        let queries = vec![
            "\"shan yu qi mu ju dan che\"",
            "\"chan yu\"",
            "\"dan yu qi mo ju chan che\"",
            "\"dan yu qi mu ju ch\"*",
            "\"dan yu qi mo ju d\"*",
            "\"s y q m j d c\"",
            "\"c y q m j s c\"",
            "\"c y q m j\"",
            "单于",
            "\"单yu\"",
            "单于骑模具单车",
            "模具",
            "单车",
            "车",
        ];
        (content, queries)
    }

    fn create_trigger(db: &Database) {
        // 处理 insert_or_replace : sql 逻辑是内部先 delete 后 insert，但是不会触发 delete 的触发器
        let sql = "
    CREATE TRIGGER IF NOT EXISTS table_message_bi_cn BEFORE INSERT ON table_message
    BEGIN
        INSERT INTO table_message_fts_cn(table_message_fts_cn, rowid, search_word)
        SELECT 'delete', id, search_word FROM table_message WHERE id = new.id;
    END;";

        db.execute_sql(&sql).unwrap();

        // 处理 insert
        let sql = "
    CREATE TRIGGER IF NOT EXISTS table_message_ai_cn AFTER INSERT ON table_message
    BEGIN
        INSERT INTO table_message_fts_cn(rowid, id, search_word)
        VALUES (new.id, new.id, new.search_word);
    END;";

        db.execute_sql(&sql).unwrap();

        // 处理 delete
        let sql = "
    CREATE TRIGGER IF NOT EXISTS table_message_ad_cn AFTER DELETE ON table_message
    BEGIN
        INSERT INTO table_message_fts_cn(table_message_fts_cn, rowid, search_word)
        VALUES ('delete', old.id, old.search_word);
    END;";

        db.execute_sql(&sql).unwrap();

        // 处理 update : 先删后加
        let sql = "
    CREATE TRIGGER IF NOT EXISTS table_message_au_cn AFTER UPDATE ON table_message
    BEGIN
        INSERT INTO table_message_fts_cn(table_message_fts_cn, rowid, search_word)
        VALUES ('delete', old.id, old.search_word);

        INSERT INTO table_message_fts_cn(rowid, id, search_word)
        VALUES (new.id, new.id, new.search_word);
    END;";

        db.execute_sql(&sql).unwrap();

        let sql = "
    CREATE TRIGGER IF NOT EXISTS table_message_bi_pinyin BEFORE INSERT ON table_message
    BEGIN
        INSERT INTO table_message_fts_pinyin(table_message_fts_pinyin, rowid, search_word)
        SELECT 'delete', id, search_word FROM table_message WHERE id = new.id;
    END;";

        db.execute_sql(&sql).unwrap();

        let sql = "
    CREATE TRIGGER IF NOT EXISTS table_message_ai_pinyin AFTER INSERT ON table_message
    BEGIN
        INSERT INTO table_message_fts_pinyin(rowid, id, search_word)
        VALUES (new.id, new.id, new.search_word);
    END;";

        db.execute_sql(&sql).unwrap();

        let sql = "
    CREATE TRIGGER IF NOT EXISTS table_message_ad_pinyin AFTER DELETE ON table_message
    BEGIN
        INSERT INTO table_message_fts_pinyin(table_message_fts_pinyin, rowid, search_word)
        VALUES ('delete', old.id, old.search_word);
    END;";

        db.execute_sql(&sql).unwrap();

        let sql = "
    CREATE TRIGGER IF NOT EXISTS table_message_au_pinyin AFTER UPDATE ON table_message
    BEGIN
        INSERT INTO table_message_fts_pinyin(table_message_fts_pinyin, rowid, search_word)
        VALUES ('delete', old.id, old.search_word);

        INSERT INTO table_message_fts_pinyin(rowid, id, search_word)
        VALUES (new.id, new.id, new.search_word);
    END;";

        db.execute_sql(&sql).unwrap();
    }

    fn create_tables(db: &Database) {
        db.add_tokenizer(BuiltinTokenizer::VERBATIM);
        db.add_tokenizer(BuiltinTokenizer::PINYIN);

        db.create_table(TABLE_MESSAGE_NAME, &*DB_TABLE_MESSAGE_INSTANCE)
            .unwrap();
        db.create_virtual_table(
            TABLE_MESSAGE_FTS_PINYIN_NAME,
            &*DB_TABLE_MESSAGE_FTS_PINYIN_INSTANCE,
        )
        .unwrap();
        db.create_virtual_table(
            TABLE_MESSAGE_FTS_CN_NAME,
            &*DB_TABLE_MESSAGE_FTS_CN_INSTANCE,
        )
        .unwrap();
    }

    fn config_pinyin_dict() {
        Database::config_pinyin_dict(&HashMap::from([
            ("单", vec!["shan", "dan", "chan"]),
            ("于", vec!["yu"]),
            ("骑", vec!["qi"]),
            ("模", vec!["mo", "mu"]),
            ("具", vec!["ju"]),
            ("车", vec!["che"]),
            ("中", vec!["zhong"]),
            ("国", vec!["guo"]),
            ("人", vec!["ren"]),
        ]));
    }
}
