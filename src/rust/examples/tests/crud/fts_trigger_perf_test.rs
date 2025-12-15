use wcdb_derive::WCDBTableCoding;

pub static TABLE_FTS_REBUILD_PROGRESS_NAME: &str = "table_fts_rebuild_progress";

#[derive(WCDBTableCoding)]
pub struct TableFtsRebuildProgress {
    #[WCDBField(is_unique = true, default(text_value = ""))]
    table_name: String,
    #[WCDBField(default(i32_value = 0))]
    last_processed_id: i64,
    #[WCDBField(default(i32_value = 0))]
    total_id: i64,
}

impl TableFtsRebuildProgress {
    pub(crate) fn new(table_name: &str, last_processed_id: i64, total_id: i64) -> Self {
        Self {
            table_name: table_name.to_string(),
            last_processed_id,
            total_id,
        }
    }

    pub(crate) fn default() -> Self {
        Self {
            table_name: "".to_string(),
            last_processed_id: 0,
            total_id: 0,
        }
    }
}

#[cfg(test)]
pub mod fts_trigger_perf_test {
    use crate::base::base_test_case::BaseTestCase;
    use crate::crud::fts_trigger_perf_test::{
        DbTableFtsRebuildProgress, TableFtsRebuildProgress, DB_TABLE_FTS_REBUILD_PROGRESS_INSTANCE,
        TABLE_FTS_REBUILD_PROGRESS_NAME,
    };
    use crate::crud::fts_trigger_test::{
        DbTableMessage, DbTableMessageFtsCn, DbTableMessageFtsPinyin,
        DB_TABLE_MESSAGE_FTS_CN_INSTANCE, DB_TABLE_MESSAGE_FTS_PINYIN_INSTANCE,
        DB_TABLE_MESSAGE_INSTANCE, TABLE_MESSAGE_FTS_CN_NAME, TABLE_MESSAGE_FTS_PINYIN_NAME,
        TABLE_MESSAGE_NAME,
    };
    use crate::crud::pinyin_dict::pinyin_map::PINYIN_MAP;
    use crate::crud::pinyin_dict::traditional_chinese_map::TRADITIONAL_CHINESE_MAP;
    use std::collections::HashMap;
    use std::fs;
    use std::time::{Duration, Instant};
    use wcdb::core::database::Database;
    use wcdb::core::handle_operation::HandleOperationTrait;
    use wcdb::core::handle_orm_operation::HandleORMOperationTrait;
    use wcdb::fts::builtin_tokenizer::BuiltinTokenizer;
    use wcdb::winq::expression_operable::ExpressionOperableTrait;
    use wcdb::winq::statement_select::StatementSelect;

    fn setup(db_name: &str) -> Database {
        BaseTestCase::global_set_up();
        let db = Database::new(&format!("./target/tmp/{}", db_name), None);
        db.remove_files().unwrap();
        config_pinyin_dict();
        copy_db_file(db_name);
        create_tables(&db);
        create_trigger(&db);
        db
    }

    #[test]
    fn test_trigger_rebuild() {
        let db_name = "cn_100k.db";
        let db = setup(db_name);
        bench(
            || {
                // trigger_rebuild_all(&db);
                trigger_rebuild_by_batch(&db);
            },
            db_name,
            10,
        );

        search_data(&db);
    }

    fn search_data(db: &Database) {
        let queries = vec!["的", "de", "实践", "shi jian"];
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
            // assert_eq!(vec.len(), 1);
            assert!(vec.len() >= 1);
        }
    }

    fn bench<F>(mut f: F, db_name: &str, times: usize)
    where
        F: FnMut(),
    {
        let mut results = Vec::<Duration>::new();

        for _ in 0..times {
            let start = Instant::now();
            f();
            results.push(start.elapsed());
        }

        let min = results.iter().min().unwrap();
        let max = results.iter().max().unwrap();
        let avg = results.iter().map(|d| d.as_nanos()).sum::<u128>() / results.len() as u128;

        println!("-------------- Benchmark Result --------------");
        println!("{} 次数: {}", db_name, times);
        println!("最小耗时: {:?}  (≈ {} µs)", min, min.as_micros());
        println!("最大耗时: {:?}  (≈ {} µs)", max, max.as_micros());
        println!("平均耗时: {:?}ns  (≈ {} µs)", avg, avg / 1000);
    }

    fn trigger_rebuild_by_batch(db: &Database) {
        init_fts_rebuild_progress(&db);
        let batch_number = 1000;
        let (mut last_processed_id, total_id) = get_current_rebuild_progress(&db);
        while last_processed_id < total_id {
            let end_id = if last_processed_id + batch_number < total_id {
                last_processed_id + batch_number
            } else {
                total_id
            };
            println!("qxb rebuild from {} to {}", last_processed_id, end_id);

            let sql = "BEGIN";
            db.execute_sql(sql).unwrap();
            let sql_pinyin = format!("INSERT INTO {}(rowid, id, search_word) SELECT id, id, search_word FROM table_message WHERE id > {} AND id <= {};", TABLE_MESSAGE_FTS_PINYIN_NAME, last_processed_id, end_id);
            db.execute_sql(&sql_pinyin).unwrap();
            let sql_cn = format!("INSERT INTO {}(rowid, id, search_word) SELECT id, id, search_word FROM table_message WHERE id > {} AND id <= {};", TABLE_MESSAGE_FTS_CN_NAME, last_processed_id, end_id);
            db.execute_sql(&sql_cn).unwrap();
            last_processed_id = end_id;
            let mut table_progress = TableFtsRebuildProgress::default();
            table_progress.last_processed_id = last_processed_id;
            let exp = DbTableFtsRebuildProgress::table_name().eq(TABLE_MESSAGE_NAME);
            let fields = vec![DbTableFtsRebuildProgress::last_processed_id()];
            db.update_object(
                table_progress,
                fields,
                TABLE_FTS_REBUILD_PROGRESS_NAME,
                Some(&exp),
                None,
                None,
                None,
            )
            .unwrap();
            let sql = "COMMIT";
            db.execute_sql(sql).unwrap();
        }
    }

    fn get_current_rebuild_progress(db: &Database) -> (i64, i64) {
        let progress_opt = db
            .get_first_object(
                DbTableFtsRebuildProgress::all_fields(),
                TABLE_FTS_REBUILD_PROGRESS_NAME,
                None,
                None,
                None,
            )
            .unwrap();
        match progress_opt {
            None => (0, 0),
            Some(progress) => (progress.last_processed_id, progress.total_id),
        }
    }

    fn init_fts_rebuild_progress(db: &Database) {
        let id_max_exp = DbTableMessage::id().max();
        let statement_select = StatementSelect::new();
        statement_select.select(&vec![id_max_exp]);
        statement_select.from(vec![TABLE_MESSAGE_NAME]);
        let value = db.get_value_from_statement(&statement_select).unwrap();
        let id_max = value.get_i64();

        let table_progress = TableFtsRebuildProgress::new(TABLE_MESSAGE_NAME, 0, id_max);
        db.insert_or_ignore_object(
            table_progress,
            DbTableFtsRebuildProgress::all_fields(),
            TABLE_FTS_REBUILD_PROGRESS_NAME,
        )
        .unwrap();
    }

    fn trigger_rebuild_all(db: &Database) {
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

    fn copy_db_file(db_name: &str) {
        let src_path = format!("./target/{}", db_name);
        let target_path = format!("./target/tmp/{}", db_name);
        if !fs::exists(&src_path).unwrap() {
            panic!("{} does not exist", src_path);
        }
        fs::copy(src_path, target_path).unwrap();
    }

    fn config_pinyin_dict() {
        let hash_map: HashMap<&str, Vec<&str>> = PINYIN_MAP
            .entries()
            .map(|(k, v)| (*k, v.to_vec()))
            .collect();
        Database::config_pinyin_dict(&hash_map);

        let map: HashMap<&str, &str> = TRADITIONAL_CHINESE_MAP
            .entries()
            .map(|(k, v)| (*k, *v))
            .collect();
        Database::config_traditional_chinese_dict(&map);
    }

    fn create_tables(db: &Database) {
        db.add_tokenizer(BuiltinTokenizer::VERBATIM);
        db.add_tokenizer(BuiltinTokenizer::PINYIN);

        db.create_table(TABLE_MESSAGE_NAME, &*DB_TABLE_MESSAGE_INSTANCE)
            .unwrap();
        db.create_table(
            TABLE_FTS_REBUILD_PROGRESS_NAME,
            &*DB_TABLE_FTS_REBUILD_PROGRESS_INSTANCE,
        )
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
}
