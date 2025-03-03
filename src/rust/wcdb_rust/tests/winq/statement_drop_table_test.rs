#[cfg(test)]
pub mod statement_drop_table_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb_core::winq::statement_drop_table::StatementDropTable;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(
            StatementDropTable::new().drop_table("testTable"),
            "DROP TABLE testTable",
        );

        WinqTool::winq_equal(
            StatementDropTable::new().drop_table("testTable").if_exist(),
            "DROP TABLE IF EXISTS testTable",
        );

        WinqTool::winq_equal(
            StatementDropTable::new()
                .drop_table("testTable")
                .of("testSchema"),
            "DROP TABLE testSchema.testTable",
        );
    }
}
