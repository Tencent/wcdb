#[cfg(test)]
pub mod statement_create_table_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb_core::winq::column::Column;
    use wcdb_core::winq::column_type::ColumnType;
    use wcdb_core::winq::statement_create_table::StatementCreateTable;
    use wcdb_core::winq::table_constraint::TableConstraint;

    #[test]
    pub fn test() {
        let column1 = Column::new("column1");
        let column2 = Column::new("column2");

        let def1 = column1.as_def(ColumnType::Integer);
        let def2 = column2.as_def(ColumnType::Text);

        let constraint1 = TableConstraint::new_by_constraint_name("constraint1")
            .primary_key()
            .indexed_by(vec![&column1]);
        let constraint2 = TableConstraint::new_by_constraint_name("constraint2")
            .unique()
            .indexed_by(vec![&column2]);

        let table1 = "table1";

        WinqTool::winq_equal(
            StatementCreateTable::new()
                .create_table(table1)
                .define(vec![&def1, &def2]),
            "CREATE TABLE table1(column1 INTEGER, column2 TEXT)",
        );
    }
}
