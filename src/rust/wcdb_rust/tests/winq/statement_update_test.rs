#[cfg(test)]
pub mod statement_update_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb_core::winq::column::Column;
    use wcdb_core::winq::expression_operable_trait::ExpressionOperableTrait;
    use wcdb_core::winq::ordering_term::Order;
    use wcdb_core::winq::statement_update::StatementUpdate;

    #[test]
    pub fn test() {
        let test_table = "testTable";
        let statement = StatementUpdate::new();

        let column = Column::new("column1");
        let test = statement.update(test_table);
        WinqTool::winq_equal(test, "UPDATE testTable SET ");

        let test = statement.set_column_objs_to_bind_parameters(&vec![column]);
        WinqTool::winq_equal(test, "UPDATE testTable SET column1 = ?1");

        let column2 = Column::new("column2");
        let expression = column2.gt_long(100);
        let test = statement.where_expression(expression);
        WinqTool::winq_equal(
            test,
            "UPDATE testTable SET column1 = ?1 WHERE column2 > 100",
        );

        let test = statement.limit(100);
        WinqTool::winq_equal(
            test,
            "UPDATE testTable SET column1 = ?1 WHERE column2 > 100 LIMIT 100",
        );

        let test = statement.offset(100);
        WinqTool::winq_equal(
            test,
            "UPDATE testTable SET column1 = ?1 WHERE column2 > 100 LIMIT 100 OFFSET 100",
        );

        let column3 = Column::new("column3");
        let test = statement.order_by(&vec![column3.order(Order::Asc)]);
        WinqTool::winq_equal(
            test,
            "UPDATE testTable SET column1 = ?1 WHERE column2 > 100 ORDER BY column3 ASC LIMIT 100 OFFSET 100",
        );
    }
}
