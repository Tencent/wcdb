#[cfg(test)]
pub mod statement_create_index_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::column::Column;
    use wcdb::winq::expression_operable::ExpressionOperableTrait;
    use wcdb::winq::indexed_column::IndexedColumn;
    use wcdb::winq::ordering_term::Order;
    use wcdb::winq::statement_create_index::StatementCreateIndex;

    #[test]
    pub fn test() {
        let index1 = IndexedColumn::new("column1");
        let index2 = IndexedColumn::new("column2");
        index2.order(Order::Asc);
        let index_name = "index1";
        let table_name = "table1";

        let mut indexed_column_vec: Vec<&IndexedColumn> = Vec::new();
        indexed_column_vec.push(&index1);
        indexed_column_vec.push(&index2);
        WinqTool::winq_equal(
            StatementCreateIndex::new()
                .create_index(index_name)
                .on(table_name)
                .indexed_by(indexed_column_vec),
            "CREATE INDEX index1 ON table1(column1, column2 ASC)",
        );

        let mut indexed_column_vec: Vec<&IndexedColumn> = Vec::new();
        indexed_column_vec.push(&index1);
        indexed_column_vec.push(&index2);
        WinqTool::winq_equal(
            StatementCreateIndex::new()
                .create_index(index_name)
                .of("testSchema")
                .on(table_name)
                .indexed_by(indexed_column_vec),
            "CREATE INDEX testSchema.index1 ON table1(column1, column2 ASC)",
        );

        let mut indexed_column_vec: Vec<&IndexedColumn> = Vec::new();
        indexed_column_vec.push(&index2);
        WinqTool::winq_equal(
            StatementCreateIndex::new()
                .create_index(index_name)
                .unique()
                .on(table_name)
                .indexed_by(indexed_column_vec),
            "CREATE UNIQUE INDEX index1 ON table1(column2 ASC)",
        );

        let mut column_names: Vec<String> = Vec::new();
        column_names.push("newColumn".parse().unwrap());
        WinqTool::winq_equal(
            StatementCreateIndex::new()
                .create_index(index_name)
                .if_not_exist()
                .on(table_name)
                .indexed_by_column_names(&column_names),
            "CREATE INDEX IF NOT EXISTS index1 ON table1(newColumn)",
        );

        let mut column_names: Vec<String> = Vec::new();
        column_names.push("column1".parse().unwrap());
        column_names.push("column2".parse().unwrap());
        let expression = Column::new("column1", None).ge(1);
        WinqTool::winq_equal(
            StatementCreateIndex::new()
                .create_index(index_name)
                .on(table_name)
                .indexed_by_column_names(&column_names)
                .r#where(expression),
            "CREATE INDEX index1 ON table1(column1, column2) WHERE column1 >= 1",
        );
    }
}
