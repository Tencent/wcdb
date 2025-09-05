#[cfg(test)]
pub mod statement_create_table_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::column::Column;
    use wcdb::winq::column_type::ColumnType;
    use wcdb::winq::statement_create_table::StatementCreateTable;
    use wcdb::winq::table_constraint::TableConstraint;

    #[test]
    pub fn test() {
        let column1 = Column::new("column1", None);
        let column2 = Column::new("column2", None);

        let def1 = column1.as_def(ColumnType::Integer);
        let def2 = column2.as_def(ColumnType::Text);

        let constraint1 = TableConstraint::new(Some("constraint1"))
            .primary_key()
            .indexed_by(&vec![column1]);
        let constraint2 = TableConstraint::new(Some("constraint2"))
            .unique()
            .indexed_by(&vec![column2]);

        let table1 = "table1";

        WinqTool::winq_equal(
            StatementCreateTable::new()
                .create_table(table1)
                .define(vec![&def1, &def2]),
            "CREATE TABLE table1(column1 INTEGER, column2 TEXT)",
        );
    }
}
