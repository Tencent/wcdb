// use wcdb::winq::column::Column;
// use wcdb::winq::identifier::IdentifierTrait;
// use wcdb::winq::statement_select::StatementSelect;
// use wcdb_derive::WCDBTableCoding;
//
// pub struct JoinTest {}
// impl JoinTest {
//     pub fn new() -> JoinTest {
//         JoinTest {}
//     }
//
//     fn select1(&self) -> StatementSelect {
//         let column = Column::new("column1");
//         let columns = vec![column];
//         let ret = StatementSelect::new();
//         ret.select(&columns)
//             .from("testTable1");
//         ret
//     }
//
//     fn select1sql(&self) -> String {
//         let statement_select = self.select1();
//         let mut ret: String = "".to_string();
//         ret = format!("{}{}{}", "(", statement_select.get_description(), ")");
//         ret
//     }
//
//     fn select2(&self) -> StatementSelect {
//         let column = Column::new("column2");
//         let columns = vec![column];
//         let ret = StatementSelect::new();
//         ret.select(&columns)
//             .from("testTable2");
//         ret
//     }
//
//     fn select2sql(&self) -> String {
//         let statement_select = self.select2();
//         let mut ret: String = "".to_string();
//         ret = format!("{}{}{}", "(", statement_select.get_description(), ")");
//         ret
//     }
// }
//
// #[derive(WCDBTableCoding)]
// #[WCDBTable()]
// pub struct MessageTagTable {
//     #[WCDBField]
//     tag_id: String,
//     #[WCDBField]
//     tag_name: String,
//     #[WCDBField]
//     create_time: i64,
// }
// impl MessageTagTable {
//     pub fn new() -> Self {
//         MessageTagTable {
//             tag_id: "".to_string(),
//             tag_name: "".to_string(),
//             create_time: 0,
//         }
//     }
// }
//
// #[derive(WCDBTableCoding)]
// #[WCDBTable()]
// pub struct ConversationTagTable {
//     #[WCDBField]
//     tag_id: String,
//     #[WCDBField]
//     target_id: String,
//     #[WCDBField]
//     category_id: String,
//     #[WCDBField]
//     is_top: bool,
// }
//
// impl ConversationTagTable {
//     pub fn new() -> Self {
//         ConversationTagTable {
//             tag_id: "".to_string(),
//             target_id: "".to_string(),
//             category_id: "".to_string(),
//             is_top: false,
//         }
//     }
// }
//
// pub(crate) struct SelectResult {
//     pub message_tag_table: MessageTagTable,
//     pub conversation_tag_table: ConversationTagTable,
// }
//
// impl SelectResult {
//     pub fn new() -> Self {
//         SelectResult {
//             message_tag_table: MessageTagTable::new(),
//             conversation_tag_table: ConversationTagTable::new(),
//         }
//     }
// }
//
// #[cfg(test)]
// pub mod join_test {
//     use crate::base::winq_tool::WinqTool;
//     use crate::winq::join_test::{
//         ConversationTagTable, DbConversationTagTable, DbMessageTagTable, JoinTest, MessageTagTable,
//         SelectResult, DB_CONVERSATION_TAG_TABLE_INSTANCE, DB_MESSAGE_TAG_TABLE_INSTANCE,
//     };
//     use wcdb::base::value::Value;
//     use wcdb::base::wcdb_exception::WCDBResult;
//     use wcdb::core::database::Database;
//     use wcdb::core::handle_orm_operation::HandleORMOperationTrait;
//     use wcdb::core::table_orm_operation::TableORMOperationTrait;
//     use wcdb::winq::column::Column;
//     use wcdb::winq::identifier::IdentifierTrait;
//     use wcdb::winq::join::Join;
//     use wcdb::winq::result_column::ResultColumn;
//     use wcdb::winq::statement_select::StatementSelect;
//
//     #[test]
//     pub fn test() {
//         let test_table_name1 = "testTable1";
//         let test_table_name2 = "testTable2";
//
//         let join_left = Join::new(test_table_name1);
//         let join_right = Join::new(test_table_name2);
//         WinqTool::winq_equal(
//             join_left.join_with_table_name(test_table_name2),
//             "testTable1 JOIN testTable2",
//         );
//
//         let join_test = JoinTest::new();
//         let join_left = Join::new(&join_test.select1());
//         let join_right = Join::new(&join_test.select2());
//         let select1sql = join_test.select1sql();
//         let select2sql = join_test.select2sql();
//         WinqTool::winq_equal(
//             join_left.join_with_table_or_subquery_convertible(&join_test.select2()),
//             &*format!("{}{}{}", select1sql, " JOIN ", select2sql).to_string(),
//         );
//
//         let join_left = Join::new(test_table_name1);
//         let join_right = Join::new(test_table_name2);
//         WinqTool::winq_equal(
//             join_left.with_table_name(test_table_name2),
//             "testTable1, testTable2",
//         );
//
//         let join_test = JoinTest::new();
//         let join_left = Join::new(&join_test.select1());
//         let join_right = Join::new(&join_test.select2());
//         let select1sql = join_test.select1sql();
//         let select2sql = join_test.select2sql();
//         WinqTool::winq_equal(
//             join_left.with_table_or_subquery_convertible(&join_test.select2()),
//             &*format!("{}{}{}", select1sql, ", ", select2sql).to_string(),
//         );
//
//         let join_left = Join::new_with_table_name(test_table_name1);
//         let join_right = Join::new_with_table_name(test_table_name2);
//         WinqTool::winq_equal(
//             join_left.left_join_with_table_name(test_table_name2),
//             "testTable1 LEFT JOIN testTable2",
//         );
//
//         let join_test = JoinTest::new();
//         let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
//         let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
//         let select1sql = join_test.select1sql();
//         let select2sql = join_test.select2sql();
//         WinqTool::winq_equal(
//             join_left.left_join_with_table_or_subquery_convertible(&join_test.select2()),
//             &*format!("{}{}{}", select1sql, " LEFT JOIN ", select2sql).to_string(),
//         );
//
//         let join_left = Join::new_with_table_name(test_table_name1);
//         let join_right = Join::new_with_table_name(test_table_name2);
//         WinqTool::winq_equal(
//             join_left.left_outer_join_with_table_name(test_table_name2),
//             "testTable1 LEFT OUTER JOIN testTable2",
//         );
//
//         let join_test = JoinTest::new();
//         let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
//         let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
//         let select1sql = join_test.select1sql();
//         let select2sql = join_test.select2sql();
//         WinqTool::winq_equal(
//             join_left.left_outer_join_with_table_or_subquery_convertible(&join_test.select2()),
//             &*format!("{}{}{}", select1sql, " LEFT OUTER JOIN ", select2sql).to_string(),
//         );
//
//         let join_left = Join::new_with_table_name(test_table_name1);
//         let join_right = Join::new_with_table_name(test_table_name2);
//         WinqTool::winq_equal(
//             join_left.inner_join_with_table_name(test_table_name2),
//             "testTable1 INNER JOIN testTable2",
//         );
//
//         let join_test = JoinTest::new();
//         let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
//         let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
//         let select1sql = join_test.select1sql();
//         let select2sql = join_test.select2sql();
//         WinqTool::winq_equal(
//             join_left.inner_join_with_table_or_subquery_convertible(&join_test.select2()),
//             &*format!("{}{}{}", select1sql, " INNER JOIN ", select2sql).to_string(),
//         );
//
//         let join_left = Join::new_with_table_name(test_table_name1);
//         let join_right = Join::new_with_table_name(test_table_name2);
//         WinqTool::winq_equal(
//             join_left.cross_join_with_table_name(test_table_name2),
//             "testTable1 CROSS JOIN testTable2",
//         );
//
//         let join_test = JoinTest::new();
//         let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
//         let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
//         let select1sql = join_test.select1sql();
//         let select2sql = join_test.select2sql();
//         WinqTool::winq_equal(
//             join_left.cross_join_with_table_or_subquery_convertible(&join_test.select2()),
//             &*format!("{}{}{}", select1sql, " CROSS JOIN ", select2sql).to_string(),
//         );
//
//         let join_left = Join::new_with_table_name(test_table_name1);
//         let join_right = Join::new_with_table_name(test_table_name2);
//         WinqTool::winq_equal(
//             join_left.natural_join_with_table_name(test_table_name2),
//             "testTable1 NATURAL JOIN testTable2",
//         );
//
//         let join_test = JoinTest::new();
//         let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
//         let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
//         let select1sql = join_test.select1sql();
//         let select2sql = join_test.select2sql();
//         WinqTool::winq_equal(
//             join_left.natural_join_with_table_or_subquery_convertible(&join_test.select2()),
//             &*format!("{}{}{}", select1sql, " NATURAL JOIN ", select2sql).to_string(),
//         );
//
//         let join_left = Join::new_with_table_name(test_table_name1);
//         let join_right = Join::new_with_table_name(test_table_name2);
//         WinqTool::winq_equal(
//             join_left.natural_left_join_with_table_name(test_table_name2),
//             "testTable1 NATURAL LEFT JOIN testTable2",
//         );
//
//         let join_test = JoinTest::new();
//         let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
//         let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
//         let select1sql = join_test.select1sql();
//         let select2sql = join_test.select2sql();
//         WinqTool::winq_equal(
//             join_left.natural_left_join_with_table_or_subquery_convertible(&join_test.select2()),
//             &*format!("{}{}{}", select1sql, " NATURAL LEFT JOIN ", select2sql).to_string(),
//         );
//
//         let join_left = Join::new_with_table_name(test_table_name1);
//         let join_right = Join::new_with_table_name(test_table_name2);
//         WinqTool::winq_equal(
//             join_left.natural_left_outer_join_with_table_name(test_table_name2),
//             "testTable1 NATURAL LEFT OUTER JOIN testTable2",
//         );
//
//         let join_test = JoinTest::new();
//         let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
//         let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
//         let select1sql = join_test.select1sql();
//         let select2sql = join_test.select2sql();
//         WinqTool::winq_equal(
//             join_left
//                 .natural_left_outer_join_with_table_or_subquery_convertible(&join_test.select2()),
//             &*format!(
//                 "{}{}{}",
//                 select1sql, " NATURAL LEFT OUTER JOIN ", select2sql
//             )
//             .to_string(),
//         );
//
//         let join_left = Join::new_with_table_name(test_table_name1);
//         let join_right = Join::new_with_table_name(test_table_name2);
//         WinqTool::winq_equal(
//             join_left.natural_inner_join_with_table_name(test_table_name2),
//             "testTable1 NATURAL INNER JOIN testTable2",
//         );
//
//         let join_test = JoinTest::new();
//         let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
//         let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
//         let select1sql = join_test.select1sql();
//         let select2sql = join_test.select2sql();
//         WinqTool::winq_equal(
//             join_left.natural_inner_join_with_table_or_subquery_convertible(&join_test.select2()),
//             &*format!("{}{}{}", select1sql, " NATURAL INNER JOIN ", select2sql).to_string(),
//         );
//
//         let join_left = Join::new_with_table_name(test_table_name1);
//         let join_right = Join::new_with_table_name(test_table_name2);
//         WinqTool::winq_equal(
//             join_left.natural_cross_join_with_table_name(test_table_name2),
//             "testTable1 NATURAL CROSS JOIN testTable2",
//         );
//
//         let join_test = JoinTest::new();
//         let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
//         let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
//         let select1sql = join_test.select1sql();
//         let select2sql = join_test.select2sql();
//         WinqTool::winq_equal(
//             join_left.natural_cross_join_with_table_or_subquery_convertible(&join_test.select2()),
//             &*format!("{}{}{}", select1sql, " NATURAL CROSS JOIN ", select2sql).to_string(),
//         );
//
//         let join_left = Join::new_with_table_name(test_table_name1);
//         let join_right = Join::new_with_table_name(test_table_name2);
//         let column1 = Column::new("column1");
//         let column2 = Column::new("column2");
//         WinqTool::winq_equal(
//             join_left
//                 .join_with_table_name(test_table_name2)
//                 .on(&column1.eq_expression_convertible(&column2)),
//             "testTable1 JOIN testTable2 ON column1 == column2",
//         );
//
//         let join_test = JoinTest::new();
//         let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
//         let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
//         let column1 = Column::new("column1");
//         let column2 = Column::new("column2");
//         let select1sql = join_test.select1sql();
//         let select2sql = join_test.select2sql();
//         WinqTool::winq_equal(
//             join_left
//                 .join_with_table_or_subquery_convertible(&join_test.select2())
//                 .on(&column1.eq_expression_convertible(&column2)),
//             &*format!(
//                 "{}{}{}{}",
//                 select1sql, " JOIN ", select2sql, " ON column1 == column2"
//             )
//             .to_string(),
//         );
//
//         let join_left = Join::new_with_table_name(test_table_name1);
//         let column1 = Column::new("column1");
//         let column2 = Column::new("column2");
//         let mut column_vec: Vec<Column> = Vec::new();
//         column_vec.push(column1);
//         column_vec.push(column2);
//         WinqTool::winq_equal(
//             join_left
//                 .join_with_table_name(test_table_name2)
//                 .using_with_column_obj_vector(&column_vec),
//             "testTable1 JOIN testTable2 USING(column1, column2)",
//         );
//
//         let join_test = JoinTest::new();
//         let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
//         let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
//         let column1 = Column::new("column1");
//         let column2 = Column::new("column2");
//         let select1sql = join_test.select1sql();
//         let select2sql = join_test.select2sql();
//         let mut column_vec: Vec<Column> = Vec::new();
//         column_vec.push(column1);
//         column_vec.push(column2);
//         WinqTool::winq_equal(
//             join_left
//                 .join_with_table_or_subquery_convertible(&join_test.select2())
//                 .using_with_column_obj_vector(&column_vec),
//             &*format!(
//                 "{}{}{}{}",
//                 select1sql, " JOIN ", select2sql, " USING(column1, column2)"
//             ),
//         );
//     }
//
//     // 新增的联表查询单测，Java 没有该用例
//     #[test]
//     pub fn join_test1() {
//         let database = Database::new("./tests/winq/custom/JoinDatabase.sqlite3", None);
//         database
//             .create_table("MessageTagTable", &*DB_MESSAGE_TAG_TABLE_INSTANCE)
//             .unwrap();
//         database
//             .create_table("ConversationTagTable", &*DB_CONVERSATION_TAG_TABLE_INSTANCE)
//             .unwrap();
//         let message_tag_table =
//             database.get_table("MessageTagTable", &*DB_MESSAGE_TAG_TABLE_INSTANCE);
//         let conversation_tag_table =
//             database.get_table("ConversationTagTable", &*DB_CONVERSATION_TAG_TABLE_INSTANCE);
//
//         // 插入数据
//         let mut tag = MessageTagTable::new();
//         tag.tag_id = "10001".to_string();
//         tag.tag_name = "10001_name".to_string();
//         tag.create_time = 1790000000;
//         let _ = message_tag_table.insert_object(tag, DbMessageTagTable::all_fields());
//
//         let mut tag = MessageTagTable::new();
//         tag.tag_id = "10002".to_string();
//         tag.tag_name = "10002_name".to_string();
//         tag.create_time = 1790000001;
//         let ret = message_tag_table.insert_object(tag, DbMessageTagTable::all_fields());
//
//         let mut conversation = ConversationTagTable::new();
//         conversation.tag_id = "10001".to_string();
//         conversation.target_id = "target_id".to_string();
//         conversation.category_id = "category_id".to_string();
//         conversation.is_top = true;
//         let ret = conversation_tag_table
//             .insert_object(conversation, DbConversationTagTable::all_fields());
//
//         let mut conversation = ConversationTagTable::new();
//         conversation.tag_id = "20001".to_string();
//         conversation.target_id = "target_id".to_string();
//         conversation.category_id = "category_id".to_string();
//         conversation.is_top = true;
//         let ret = conversation_tag_table
//             .insert_object(conversation, DbConversationTagTable::all_fields());
//
//         // 连表查询
//         let column_vec = vec![
//             ResultColumn::new_with_column_name("tag_id").r#as("a_tag_id"),
//             ResultColumn::new_with_column_name("tag_name"),
//             ResultColumn::new_with_column_name("create_time"),
//         ];
//         let binding = StatementSelect::new();
//         let tag_statement = binding
//             .select_with_result_column_convertible_trait(&column_vec)
//             .from("MessageTagTable");
//         // conversation
//         let column_vec: Vec<Column> = vec![
//             Column::new("tag_id"),
//             Column::new("target_id"),
//             Column::new("category_id"),
//             Column::new("is_top"),
//         ];
//         let binding = StatementSelect::new();
//         let conversation_statement = binding
//             .select_with_result_column_convertible_trait(&column_vec)
//             .from("ConversationTagTable");
//
//         // 构建 join
//         let column1 = Column::new("a_tag_id");
//         let column2 = Column::new("tag_id");
//         let join = Join::new_with_table_or_subquery_convertible(tag_statement);
//         join.left_join_with_table_or_subquery_convertible(conversation_statement)
//             .on(&column1.eq_expression_convertible(&column2));
//
//         // 构建查询需要的 StatementSelect
//         let column_tag_id = Column::new("tag_id");
//         column_tag_id.in_table("MessageTagTable");
//         let select = StatementSelect::new();
//         select
//             .select_with_result_column_convertible_trait(&vec![Column::all()])
//             .from_with_table_or_subquery_convertible_trait(&vec![join]);
//         // .group_by_with_expression_convertible_trait(&vec![column_tag_id]);
//
//         let sql = select.get_description();
//
//         assert_eq!(sql,
//         "SELECT * FROM ((SELECT tag_id AS a_tag_id, tag_name, create_time FROM MessageTagTable) LEFT JOIN (SELECT tag_id, target_id, category_id, is_top FROM ConversationTagTable) ON a_tag_id == tag_id)");
//
//         let ret: WCDBResult<Vec<Vec<Value>>> = database.get_all_rows_from_statement(&select);
//         let mut select_result_vec: Vec<SelectResult> = Vec::new();
//         match ret {
//             Ok(vals) => {
//                 for x in vals {
//                     let mut result = SelectResult::new();
//                     let mut tag = MessageTagTable::new();
//                     let mut conversation = ConversationTagTable::new();
//                     for v in x {
//                         tag.tag_id = v.get_text();
//                         tag.tag_name = v.get_text();
//                         tag.create_time = v.get_long();
//
//                         conversation.tag_id = v.get_text();
//                         conversation.target_id = v.get_text();
//                         conversation.category_id = v.get_text();
//                         conversation.is_top = v.get_bool();
//                     }
//                     result.message_tag_table = tag;
//                     result.conversation_tag_table = conversation;
//                     select_result_vec.push(result);
//                 }
//             }
//             Err(err) => {
//                 println!("Failed to get all rows from the statement.err: {:?}", err);
//             }
//         }
//         assert!(!select_result_vec.is_empty());
//
//         let value_opt = database.get_value_from_sql("SELECT COUNT(*) FROM MessageTagTable");
//         match value_opt {
//             Ok(value) => {
//                 assert!(value.get_long() > 0);
//             }
//             Err(error) => {
//                 println!("get_value_from_sql-->err: {:?}", error);
//             }
//         }
//
//         database.remove_files().unwrap();
//         database.close(Some(|| {}));
//     }
// }
