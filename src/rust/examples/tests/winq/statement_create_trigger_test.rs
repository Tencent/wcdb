// todo qixinbing 崩溃，待处理
// #[cfg(test)]
// pub mod statement_create_trigger_test {
//     use crate::base::winq_tool::WinqTool;
//     use wcdb::winq::column::Column;
//     use wcdb::winq::expression_operable::ExpressionOperableTrait;
//     use wcdb::winq::object::Object;
//     use wcdb::winq::statement_create_trigger::StatementCreateTrigger;
//     use wcdb::winq::statement_delete::StatementDelete;
//     use wcdb::winq::statement_insert::StatementInsert;
//     use wcdb::winq::statement_select::StatementSelect;
//     use wcdb::winq::statement_update::StatementUpdate;
//
//     #[test]
//     pub fn test() {
//         let schema = "testSchema";
//         let name = "testTrigger";
//         let column1 = Column::new("column1", None);
//         let column2 = Column::new("column2", None);
//         let table = "testTable";
//         let condition = column1.eq(1);
//         let binding_update = StatementUpdate::new();
//         let update = binding_update
//             .update(table)
//             .set_columns_to_bind_parameters(vec![&column1])
//             .to(2);
//         let binding_insert = StatementInsert::new();
//         let insert = binding_insert
//             .insert_into(table)
//             .values(Some(vec![Object::Int(1)]));
//         let binding_select = StatementSelect::new();
//         let select = binding_select.select(vec![&column1]);
//         let binding_delete = StatementDelete::new();
//         let delete = binding_delete.delete_from(table);
//
//         WinqTool::winq_equal(
//             StatementCreateTrigger::new()
//                 .create_trigger(name)
//                 .of_with_string(schema)
//                 .before()
//                 .delete()
//                 .on_table(table)
//                 .for_each_row()
//                 .when(&condition)
//                 .execute(update),
//             "CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN column1 == 1 BEGIN UPDATE testTable SET column1 = 2; END",
//         );
//
//         WinqTool::winq_equal(
//             StatementCreateTrigger::new()
//                 .create_temp_trigger(name)
//                 .before().delete()
//                 .on_table(table)
//                 .for_each_row().when(&condition)
//                 .execute(update),
//             "CREATE TEMP TRIGGER testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN column1 == 1 BEGIN UPDATE testTable SET column1 = 2; END");
//
//         WinqTool::winq_equal(
//             StatementCreateTrigger::new()
//                 .create_temp_trigger(name)
//                 .of_with_string(schema)
//                 .if_not_exist().before().delete()
//                 .on_table(table)
//                 .for_each_row()
//                 .when(&condition).execute(update)
//             ,
//             "CREATE TRIGGER IF NOT EXISTS testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN column1 == 1 BEGIN UPDATE testTable SET column1 = 2; END");
//
//         WinqTool::winq_equal(
//             StatementCreateTrigger::new().create_trigger(name)
//                 .before()
//                 .delete().on_table(table)
//                 .for_each_row().when(&condition).execute(update),
//             "CREATE TRIGGER testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN column1 == 1 BEGIN UPDATE testTable SET column1 = 2; END"
//         );
//
//         WinqTool::winq_equal(
//             StatementCreateTrigger::new()
//                 .create_trigger(name)
//                 .of_with_string(schema)
//                 .after()
//                 .delete().on_table(table)
//                 .for_each_row().when(&condition).execute(update)
//             ,
//             "CREATE TRIGGER testSchema.testTrigger AFTER DELETE ON testTable FOR EACH ROW WHEN column1 == 1 BEGIN UPDATE testTable SET column1 = 2; END"
//         );
//
//         WinqTool::winq_equal(
//             StatementCreateTrigger::new().create_trigger(name)
//                 .of_with_string(schema)
//                 .instead_of()
//                 .delete()
//                 .on_table(table).for_each_row()
//                 .when(&condition)
//                 .execute(update),
//             "CREATE TRIGGER testSchema.testTrigger INSTEAD OF DELETE ON testTable FOR EACH ROW WHEN column1 == 1 BEGIN UPDATE testTable SET column1 = 2; END"
//         );
//
//         WinqTool::winq_equal(
//             StatementCreateTrigger::new()
//                 .create_trigger(name)
//                 .of_with_string(schema)
//                 .before()
//                 .insert()
//                 .on_table(table)
//                 .for_each_row()
//                 .when(&condition)
//                 .execute(update),
//             "CREATE TRIGGER testSchema.testTrigger BEFORE INSERT ON testTable FOR EACH ROW WHEN column1 == 1 BEGIN UPDATE testTable SET column1 = 2; END"
//         );
//
//         WinqTool::winq_equal(
//             StatementCreateTrigger::new()
//                 .create_trigger(name)
//                 .of_with_string(schema)
//                 .before().update().on_table(table)
//                 .for_each_row().when(&condition).execute(update),
//             "CREATE TRIGGER testSchema.testTrigger BEFORE UPDATE ON testTable FOR EACH ROW WHEN column1 == 1 BEGIN UPDATE testTable SET column1 = 2; END"
//         );
//
//         WinqTool::winq_equal(
//             StatementCreateTrigger::new()
//                 .create_trigger(name)
//                 .of_with_string(schema)
//                 .before().update()
//                 .of_columns(vec![&column1])
//                 .on_table(table)
//                 .execute(update),
//             "CREATE TRIGGER testSchema.testTrigger BEFORE UPDATE OF column1 ON testTable BEGIN UPDATE testTable SET column1 = 2; END"
//         );
//
//         WinqTool::winq_equal(
//             StatementCreateTrigger::new()
//                 .create_trigger(name)
//                 .of_with_string(schema)
//                 .before()
//                 .update()
//                 .of_columns(vec![&column1])
//                 .on_table(table)
//                 .execute(update),
//             "CREATE TRIGGER testSchema.testTrigger BEFORE UPDATE OF column1 ON testTable BEGIN UPDATE testTable SET column1 = 2; END"
//         );
//
//         WinqTool::winq_equal(
//             StatementCreateTrigger::new()
//                 .create_trigger(name)
//                 .of_with_string(schema)
//                 .before().update()
//                 .of_columns(&vec![column1, column2]).on_table(table)
//                 .execute(update),
//             "CREATE TRIGGER testSchema.testTrigger BEFORE UPDATE OF column1, column2 ON testTable BEGIN UPDATE testTable SET column1 = 2; END"
//         );
//
//         WinqTool::winq_equal(
//             StatementCreateTrigger::new()
//                 .create_trigger(name)
//                 .of_with_string(schema)
//                 .before().update()
//                 .of_columns(vec![String::from("column1"), String::from("column2")])
//                 .on_table(table).execute(update),
//             "CREATE TRIGGER testSchema.testTrigger BEFORE UPDATE OF column1, column2 ON testTable BEGIN UPDATE testTable SET column1 = 2; END"
//         );
//
//         WinqTool::winq_equal(
//             StatementCreateTrigger::new()
//                 .create_trigger(name)
//                 .of_with_string(schema)
//                 .before()
//                 .delete().on_table(table)
//                 .for_each_row()
//                 .when(&condition)
//                 .execute(insert),
//             "CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN column1 == 1 BEGIN INSERT INTO testTable VALUES(1); END"
//         );
//
//         WinqTool::winq_equal(
//             StatementCreateTrigger::new()
//                 .create_trigger(name)
//                 .of_with_string(schema)
//                 .before()
//                 .delete()
//                 .on_table(table)
//                 .for_each_row()
//                 .when(&condition).execute(delete),
//             "CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN column1 == 1 BEGIN DELETE FROM testTable; END"
//         );
//
//         WinqTool::winq_equal(
//             StatementCreateTrigger::new()
//                 .create_trigger(name)
//                 .of_with_string(schema)
//                 .before()
//                 .delete()
//                 .on_table(table)
//                 .for_each_row()
//                 .when(&condition).execute(select),
//             "CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN column1 == 1 BEGIN SELECT column1; END"
//         );
//
//         WinqTool::winq_equal(
//             StatementCreateTrigger::new()
//                 .create_trigger(name)
//                 .of_with_string(schema)
//                 .before().delete()
//                 .on_table(table)
//                 .for_each_row()
//                 .when(&condition).execute(update).execute(insert).execute(delete).execute(select),
//             "CREATE TRIGGER testSchema.testTrigger BEFORE DELETE ON testTable FOR EACH ROW WHEN column1 == 1 BEGIN UPDATE testTable SET column1 = 2; INSERT INTO testTable VALUES(1); DELETE FROM testTable; SELECT column1; END"
//         );
//     }
// }
