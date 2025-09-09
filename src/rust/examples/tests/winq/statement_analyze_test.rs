#[cfg(test)]
pub mod statement_analyze_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::statement_analyze::StatementAnalyze;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(StatementAnalyze::new().analyze(), "ANALYZE");
        WinqTool::winq_equal(
            StatementAnalyze::new().analyze().schema("testSchema"),
            "ANALYZE testSchema",
        );
        WinqTool::winq_equal(
            StatementAnalyze::new()
                .analyze()
                .schema("testSchema")
                .table("testTable"),
            "ANALYZE testSchema.testTable",
        );
        WinqTool::winq_equal(
            StatementAnalyze::new().analyze().table("testTable"),
            "ANALYZE testTable",
        );
        WinqTool::winq_equal(
            StatementAnalyze::new()
                .analyze()
                .schema("testSchema")
                .index("testIndex"),
            "ANALYZE testSchema.testIndex",
        );
    }
}
