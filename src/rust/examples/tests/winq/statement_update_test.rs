#[cfg(test)]
pub mod statement_update_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::column::Column;
    use wcdb::winq::expression_operable_trait::ExpressionOperableTrait;
    use wcdb::winq::ordering_term::Order;
    use wcdb::winq::statement_update::StatementUpdate;

    #[test]
    pub fn test() {
        let test_table_str = String::from("testTable");
        let column_vec = vec![Column::new("column1"), Column::new("column2")];
        let column1_vec = vec![Column::new("column1")];
        let column2_vec = vec![Column::new("column2")];

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(&test_table_str.clone())
                .set_columns(&column1_vec)
                .to_i32(1),
            "UPDATE testTable SET column1 = 1",
        );

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(&test_table_str.clone())
                .set_columns(&column1_vec)
                .to_bool(true),
            "UPDATE testTable SET column1 = TRUE",
        );

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(&test_table_str.clone())
                .set_columns(&column1_vec)
                .to_string(Some("abc".to_string())),
            "UPDATE testTable SET column1 = 'abc'",
        );

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(&test_table_str.clone())
                .set_columns(&column1_vec)
                .to_f64(1.1),
            "UPDATE testTable SET column1 = 1.1000000000000001",
        );

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(&test_table_str.clone())
                .set_columns(&column1_vec)
                .to_string(None),
            "UPDATE testTable SET column1 = NULL",
        );

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(&test_table_str.clone())
                .or_replace()
                .set_columns(&column1_vec)
                .to_i32(1)
                .set_columns(&column2_vec)
                .to_string(Some("abc".to_string())),
            "UPDATE OR REPLACE testTable SET column1 = 1, column2 = 'abc'",
        );

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(&test_table_str.clone())
                .set_column_objs_to_bind_parameters(&column_vec),
            "UPDATE testTable SET column1 = ?1, column2 = ?2",
        );

        // todo dengxudong 重要不紧急
        // winqEqual(new StatementUpdate().update("testTable").setColumnsToValues(new Column[]{column1, column2}, new Object[]{1, "abc"}),
        //           "UPDATE testTable SET column1 = 1, column2 = 'abc'");
        // winqEqual(new StatementUpdate().update("testTable").setColumnsToValues(new Column[]{column1, column2}, new Value[]{new Value(1), new Value("abc")}),
        //           "UPDATE testTable SET column1 = 1, column2 = 'abc'");

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(&test_table_str.clone())
                .set_columns(&column1_vec)
                .to_i32(1)
                .where_expression(&Column::new("column1").gt_int(1)),
            "UPDATE testTable SET column1 = 1 WHERE column1 > 1",
        );

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(&test_table_str.clone())
                .set_columns(&column1_vec)
                .to_i32(1)
                .order_by(&vec![
                    Column::new("column1").order(Order::Asc),
                    Column::new("column2").order(Order::Desc),
                ]),
            "UPDATE testTable SET column1 = 1 ORDER BY column1 ASC, column2 DESC",
        );

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(&test_table_str.clone())
                .set_columns(&column1_vec)
                .to_i32(1)
                .limit(1),
            "UPDATE testTable SET column1 = 1 LIMIT 1",
        );

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(&test_table_str.clone())
                .set_columns(&column1_vec)
                .to_i32(1)
                .limit_i64_i64(1, 2),
            "UPDATE testTable SET column1 = 1 LIMIT 1, 2",
        );

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(&test_table_str.clone())
                .set_columns(&column1_vec)
                .to_i32(1)
                .limit(1)
                .offset(3),
            "UPDATE testTable SET column1 = 1 LIMIT 1 OFFSET 3",
        );
    }
}
