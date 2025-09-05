#[cfg(test)]
pub mod statement_update_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::column::{Column, ColumnTrait};
    use wcdb::winq::expression_operable::ExpressionOperableTrait;
    use wcdb::winq::ordering_term::Order;
    use wcdb::winq::statement_update::StatementUpdate;

    #[test]
    pub fn test() {
        let column1 = Column::new("column1", None);
        let column2 = Column::new("column2", None);
        let test_table_str = "testTable";
        let column_vec = vec![Column::new("column1", None), Column::new("column2", None)];
        let column1_vec = vec![column1];
        let column2_vec = vec![column2];

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(test_table_str.clone())
                .set(&column1_vec)
                .to(1),
            "UPDATE testTable SET column1 = 1",
        );

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(test_table_str)
                .set(&column1_vec)
                .to(true),
            "UPDATE testTable SET column1 = TRUE",
        );

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(test_table_str)
                .set(&column1_vec)
                .to("abc".to_string()),
            "UPDATE testTable SET column1 = 'abc'",
        );

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(test_table_str)
                .set(&column1_vec)
                .to(1.1),
            "UPDATE testTable SET column1 = 1.1000000000000001",
        );

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(test_table_str)
                .set(&column1_vec)
                .to(None),
            "UPDATE testTable SET column1 = NULL",
        );

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(test_table_str)
                .or_replace()
                .set(&column1_vec)
                .to(1)
                .set(&column2_vec)
                .to("abc".to_string()),
            "UPDATE OR REPLACE testTable SET column1 = 1, column2 = 'abc'",
        );

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(test_table_str)
                .set_columns_to_bind_parameters(&column_vec),
            "UPDATE testTable SET column1 = ?1, column2 = ?2",
        );

        // todo dengxudong 重要不紧急
        // winqEqual(new StatementUpdate().update("testTable").setColumnsToValues(new Column[]{column1, column2}, new Object[]{1, "abc"}),
        //           "UPDATE testTable SET column1 = 1, column2 = 'abc'");
        // winqEqual(new StatementUpdate().update("testTable").setColumnsToValues(new Column[]{column1, column2}, new Value[]{new Value(1), new Value("abc")}),
        //           "UPDATE testTable SET column1 = 1, column2 = 'abc'");

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(test_table_str)
                .set(&column1_vec)
                .to(1)
                .r#where(&Column::new("column1", None).gt(1)),
            "UPDATE testTable SET column1 = 1 WHERE column1 > 1",
        );

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(test_table_str)
                .set(&column1_vec)
                .to(1)
                .order_by(&vec![
                    Column::new("column1", None).order(Order::Asc),
                    Column::new("column2", None).order(Order::Desc),
                ]),
            "UPDATE testTable SET column1 = 1 ORDER BY column1 ASC, column2 DESC",
        );

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(test_table_str)
                .set(&column1_vec)
                .to(1)
                .limit(1, None),
            "UPDATE testTable SET column1 = 1 LIMIT 1",
        );

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(test_table_str)
                .set(&column1_vec)
                .to(1)
                .limit(1, 2),
            "UPDATE testTable SET column1 = 1 LIMIT 1, 2",
        );

        WinqTool::winq_equal(
            StatementUpdate::new()
                .update(test_table_str)
                .set(&column1_vec)
                .to(1)
                .limit(1, None)
                .offset(3),
            "UPDATE testTable SET column1 = 1 LIMIT 1 OFFSET 3",
        );
    }
}
