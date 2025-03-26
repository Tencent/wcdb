#[cfg(test)]
pub mod statement_delete_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::column::Column;
    use wcdb::winq::expression_operable_trait::ExpressionOperableTrait;
    use wcdb::winq::ordering_term::Order;
    use wcdb::winq::statement_delete::StatementDelete;

    #[test]
    pub fn test() {
        let test_table = "testTable";
        let statement = StatementDelete::new();

        let test = statement.delete_from(test_table);
        WinqTool::winq_equal(test, "DELETE FROM testTable");

        let column1 = Column::new("column1");
        let test = statement.where_expression(&column1.gt_long(100));
        WinqTool::winq_equal(test, "DELETE FROM testTable WHERE column1 > 100");

        let test = statement.limit(100);
        WinqTool::winq_equal(test, "DELETE FROM testTable WHERE column1 > 100 LIMIT 100");

        let column2 = Column::new("column2");
        let order = vec![column1.order(Order::Asc), column2.order(Order::Desc)];
        let test = statement.order_by(&order);
        WinqTool::winq_equal(test, "DELETE FROM testTable WHERE column1 > 100 ORDER BY column1 ASC, column2 DESC LIMIT 100");

        let test = statement.offset(100);
        WinqTool::winq_equal(test, "DELETE FROM testTable WHERE column1 > 100 ORDER BY column1 ASC, column2 DESC LIMIT 100 OFFSET 100");
    }

    #[test]
    pub fn limit_from_to() {
        let test_table = "testTable";
        let statement = StatementDelete::new();

        let test = statement.delete_from(test_table).limit_range(100, 200);
        WinqTool::winq_equal(test, "DELETE FROM testTable LIMIT 100, 200");
    }
}
