#[cfg(test)]
pub mod statement_delete_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::column::{Column, ColumnTrait};
    use wcdb::winq::expression_operable::ExpressionOperableTrait;
    use wcdb::winq::ordering_term::Order;
    use wcdb::winq::statement_delete::StatementDelete;

    #[test]
    pub fn test() {
        let test_table = "testTable";
        let statement = StatementDelete::new();

        let test = statement.delete_from(test_table);
        WinqTool::winq_equal(test, "DELETE FROM testTable");

        let column1 = Column::new("column1", None);
        let test = statement.where_(&column1.gt(100));
        WinqTool::winq_equal(test, "DELETE FROM testTable WHERE column1 > 100");

        let test = statement.limit(100);
        WinqTool::winq_equal(test, "DELETE FROM testTable WHERE column1 > 100 LIMIT 100");

        let column2 = Column::new("column2", None);
        let order1 = column1.order(Order::Asc);
        let order2 = column2.order(Order::Desc);
        let order_vec = vec![&order1, &order2];
        let test = statement.order_by(order_vec);
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
