#[cfg(test)]
pub mod statement_detach_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::statement_reindex::StatementReindex;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(
            StatementReindex::new().reindex_table("testTable"),
            "REINDEX testTable",
        );
        WinqTool::winq_equal(
            StatementReindex::new()
                .reindex_table("testTable")
                .of("testSchema"),
            "REINDEX testSchema.testTable",
        );
        WinqTool::winq_equal(
            StatementReindex::new().reindex("testIndex"),
            "REINDEX testIndex",
        );
        WinqTool::winq_equal(
            StatementReindex::new().reindex_collation("testCollation"),
            "REINDEX testCollation",
        );
    }
}
