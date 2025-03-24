#[cfg(test)]
pub mod statement_drop_index_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::statement_drop_index::StatementDropIndex;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(
            StatementDropIndex::new().drop_index("testIndex"),
            "DROP INDEX testIndex",
        );

        WinqTool::winq_equal(
            StatementDropIndex::new().drop_index("testIndex").if_exist(),
            "DROP INDEX IF EXISTS testIndex",
        );

        WinqTool::winq_equal(
            StatementDropIndex::new()
                .drop_index("testIndex")
                .of("testSchema"),
            "DROP INDEX testSchema.testIndex",
        );
    }
}
