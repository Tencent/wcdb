use criterion::{black_box, criterion_group, criterion_main, Bencher, Criterion};
use rand::prelude::SliceRandom;
use std::sync::Arc;
use std::time::{SystemTime, UNIX_EPOCH};
use wcdb::base::value::Value;
use wcdb::base::wcdb_exception::WCDBResult;
use wcdb::core::database::Database;
use wcdb::core::handle_orm_operation::HandleORMOperationTrait;
use wcdb::core::table::Table;
use wcdb_derive::WCDBTableCoding;

use wcdb::core::table_orm_operation::TableORMOperationTrait;
use wcdb::winq::column::Column;
use wcdb::winq::expression_operable_trait::ExpressionOperableTrait;
use wcdb::winq::identifier::IdentifierTrait;
use wcdb::winq::statement_create_index::StatementCreateIndex;
use wcdb::winq::statement_delete::StatementDelete;
use wcdb::winq::statement_select::StatementSelect;
use wcdb::winq::statement_update::StatementUpdate;

fn current_time_millis() -> u128 {
    let now = SystemTime::now();
    now.duration_since(UNIX_EPOCH)
        .expect("Time went backwards")
        .as_millis()
}

pub fn string_by_length(length: i32) -> String {
    let chars: Vec<char> = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
        .chars()
        .collect();
    let mut rng = rand::thread_rng();
    (0..length)
        .map(|_| *chars.choose(&mut rng).unwrap())
        .collect()
}

#[derive(WCDBTableCoding)]
#[WCDBTable]
pub struct FriendProfileTable {
    #[WCDBField(is_primary = true)]
    pub user_id: String,
    #[WCDBField]
    pub remark: String,
    #[WCDBField(default(i32_value = 1))]
    pub friend_type: i32,
    #[WCDBField]
    pub is_top: bool,
    #[WCDBField]
    pub add_time: i64,
}

impl FriendProfileTable {
    pub fn new(user_id: &str, time: i64) -> Self {
        FriendProfileTable {
            user_id: user_id.to_string(),
            remark: "remark1".to_string(),
            friend_type: 2,
            is_top: false,
            add_time: time,
        }
    }
}

fn insert_data_performance(
    table: &Arc<Table<FriendProfileTable, DbFriendProfileTable>>,
    size: i32,
) {
    let mut vec: Vec<FriendProfileTable> = Vec::with_capacity(100);
    for x in 0..size {
        vec.push(FriendProfileTable::new(
            &*string_by_length(10),
            current_time_millis() as i64,
        ));
    }
    let insert_result = table.insert_objects(vec, DbFriendProfileTable::all_fields());
}

fn select_data_performance(database: &Database, size: i64) {
    let column_vec: Vec<Column> = vec![
        Column::new("user_id"),
        Column::new("remark"),
        Column::new("friend_type"),
        Column::new("is_top"),
        Column::new("add_time"),
    ];
    let binding = StatementSelect::new();
    let condition = Column::new("add_time").gt_int(1);
    let statement = binding
        .select_with_result_column_convertible_trait(&column_vec)
        .from("FriendProfileTable")
        .where_expression(&condition)
        .limit(size);
    // SELECT user_id, remark, friend_type, is_top, add_time FROM FriendProfileTable WHERE add_time > 1 LIMIT 1
    let ret: WCDBResult<Vec<Vec<Value>>> = database.get_all_rows_from_statement(statement);
}

fn update_data_performance(database: &Database, size: i64) {
    let column = Column::new("is_top");
    let column_vec: Vec<&Column> = vec![&column];
    let statement = StatementUpdate::new();
    let condition = Column::new("is_top")
        .not_eq_bool(true)
        .and(&Column::new("add_time").gt_int(1));
    statement
        .update("FriendProfileTable")
        .set_columns(&column_vec)
        .to_bool(true)
        .where_expression(&condition)
        .limit(size);
    // UPDATE FriendProfileTable SET is_top = TRUE WHERE (is_top != TRUE) AND (add_time > 1) LIMIT 1
    let ret = database.execute(&statement);
}

fn delete_data_performance(database: &Database, size: i64) {
    let statement = StatementDelete::new();
    let condition = Column::new("add_time").gt_int(1);
    statement
        .delete_from("FriendProfileTable")
        .r#where(&condition)
        .limit(size);
    // DELETE FROM FriendProfileTable WHERE add_time > 1 LIMIT 1
    let ret = database.execute(&statement);
}

fn index_data_performance(database: &Database) {
    let statement_create_index = StatementCreateIndex::new();
    let column1 = Column::new("add_time");
    let statement = statement_create_index
        .create_index("add_time_index")
        .on("FriendProfileTable")
        .indexed_by(vec![&column1]);
    // CREATE INDEX add_time_index ON FriendProfileTable(add_time)
    database.execute(statement).unwrap();
}

fn benchmark_function(c: &mut Criterion) {
    {
        let database = Database::new("./tests/database/custom/upgrade_db.sqlite3");
        database.remove_files().unwrap();
    }
    let database = Database::new("./tests/database/custom/upgrade_db.sqlite3");
    database
        .create_table("FriendProfileTable", &*DB_FRIEND_PROFILE_TABLE_INSTANCE)
        .unwrap();
    let conversation_table =
        database.get_table("FriendProfileTable", &*DB_FRIEND_PROFILE_TABLE_INSTANCE);

    // 插入测试
    let mut group = c.benchmark_group("db-performance-example");
    group.significance_level(0.05).sample_size(10);
    group.bench_function("insert_1", |b: &mut Bencher| {
        b.iter(|| insert_data_performance(black_box(&conversation_table.clone()), black_box(1)))
    });
    group.bench_function("insert_10k", |b: &mut Bencher| {
        b.iter(|| insert_data_performance(black_box(&conversation_table.clone()), black_box(1000)))
    });
    group.bench_function("insert_1m", |b: &mut Bencher| {
        b.iter(|| {
            insert_data_performance(black_box(&conversation_table.clone()), black_box(1000000))
        })
    });

    // 查询测试 select_data_performance
    group.bench_function("select_1", |b: &mut Bencher| {
        b.iter(|| select_data_performance(black_box(&database), black_box(1)))
    });
    group.bench_function("select_10k", |b: &mut Bencher| {
        b.iter(|| select_data_performance(black_box(&database), black_box(10000)))
    });
    group.bench_function("select_1m", |b: &mut Bencher| {
        b.iter(|| select_data_performance(black_box(&database), black_box(1000000)))
    });

    // 修改测试
    group.bench_function("update_1", |b: &mut Bencher| {
        b.iter(|| update_data_performance(black_box(&database), black_box(1)))
    });
    group.bench_function("update_10k", |b: &mut Bencher| {
        b.iter(|| update_data_performance(black_box(&database), black_box(10000)))
    });
    group.bench_function("update_1m", |b: &mut Bencher| {
        b.iter(|| update_data_performance(black_box(&database), black_box(1000000)))
    });

    // 创建索引
    index_data_performance(&database);
    group.bench_function("index_select_1", |b: &mut Bencher| {
        b.iter(|| select_data_performance(black_box(&database), black_box(1)))
    });
    group.bench_function("index_select_10k", |b: &mut Bencher| {
        b.iter(|| select_data_performance(black_box(&database), black_box(10000)))
    });
    group.bench_function("index_select_1m", |b: &mut Bencher| {
        b.iter(|| select_data_performance(black_box(&database), black_box(1000000)))
    });

    // 删除测试
    group.bench_function("delete_1", |b: &mut Bencher| {
        b.iter(|| delete_data_performance(black_box(&database), black_box(1)))
    });
    group.bench_function("delete_10k", |b: &mut Bencher| {
        b.iter(|| delete_data_performance(black_box(&database), black_box(10000)))
    });
    group.bench_function("delete_1m", |b: &mut Bencher| {
        b.iter(|| update_data_performance(black_box(&database), black_box(1000000)))
    });

    group.finish();
    database.remove_files().unwrap();
    database.close(Some(|| {}));
}

criterion_group!(benches, benchmark_function);
criterion_main!(benches);
