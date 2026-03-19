#[cfg(test)]
pub mod statement_rollback_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::statement_rollback::StatementRollback;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(&StatementRollback::new(), "ROLLBACK");
        WinqTool::winq_equal(
            StatementRollback::new().rollback_to("testSavepoint"),
            "ROLLBACK TO testSavepoint",
        );
    }
}
