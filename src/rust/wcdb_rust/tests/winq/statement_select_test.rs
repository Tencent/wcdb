#[cfg(test)]
pub mod statement_select_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb_core::winq::column::Column;
    use wcdb_core::winq::expression_operable_trait::ExpressionOperableTrait;
    use wcdb_core::winq::ordering_term::Order;
    use wcdb_core::winq::statement_select::StatementSelect;

    #[test]
    pub fn test() {
        let test_table = "testTable";
        let statement = StatementSelect::new();

        let column = Column::new("column1");
        let test = statement.from(test_table).select_columns(&vec![&column]);
        WinqTool::winq_equal(test, "SELECT column1 FROM testTable");

        let expression = column.gt_long(100);
        let test = statement.where_expression(&expression);
        WinqTool::winq_equal(test, "SELECT column1 FROM testTable WHERE column1 > 100");

        let test = statement.limit(100);
        WinqTool::winq_equal(
            test,
            "SELECT column1 FROM testTable WHERE column1 > 100 LIMIT 100",
        );

        let column2 = Column::new("column2");
        let order = vec![column2.order(Order::Desc)];
        let test = statement.order_by(&order);
        WinqTool::winq_equal(
            test,
            "SELECT column1 FROM testTable WHERE column1 > 100 ORDER BY column2 DESC LIMIT 100",
        );

        let test = statement.offset(100);
        WinqTool::winq_equal(
            test,
            "SELECT column1 FROM testTable WHERE column1 > 100 ORDER BY column2 DESC LIMIT 100 OFFSET 100",
        );

        let test = statement.group_by(&vec!["column3".to_string()]);
        WinqTool::winq_equal(
            test,
            "SELECT column1 FROM testTable WHERE column1 > 100 GROUP BY column3 ORDER BY column2 DESC LIMIT 100 OFFSET 100",
        );
    }
}
