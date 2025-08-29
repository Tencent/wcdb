#[cfg(test)]
pub mod statement_alter_table_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::column::Column;
    use wcdb::winq::column_type::ColumnType;
    use wcdb::winq::statement_alter_table::StatementAlterTable;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(
            StatementAlterTable::new()
                .alter_table("table1")
                .rename_to("table2"),
            "ALTER TABLE table1 RENAME TO table2",
        );

        WinqTool::winq_equal(
            StatementAlterTable::new()
                .alter_table("table1")
                .of("testSchema")
                .rename_to("table2"),
            "ALTER TABLE testSchema.table1 RENAME TO table2",
        );

        let column_def = Column::new("column1", None).as_def(ColumnType::Float);
        WinqTool::winq_equal(
            StatementAlterTable::new()
                .alter_table("table1")
                .add_column(&column_def),
            "ALTER TABLE table1 ADD COLUMN column1 REAL",
        );

        WinqTool::winq_equal(
            StatementAlterTable::new()
                .alter_table("table1")
                .rename_column_by_name("column1")
                .to_column_by_name("column2"),
            "ALTER TABLE table1 RENAME COLUMN column1 TO column2",
        );

        let column1 = Column::new("column1", None);
        let column2 = Column::new("column2", None);
        WinqTool::winq_equal(
            StatementAlterTable::new()
                .alter_table("table1")
                .rename_column(&column1)
                .to_column(&column2),
            "ALTER TABLE table1 RENAME COLUMN column1 TO column2",
        );
    }
}
