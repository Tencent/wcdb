#[cfg(test)]
pub mod statement_release_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::statement_release::StatementRelease;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(
            StatementRelease::new().release("testSavepoint"),
            "RELEASE testSavepoint",
        );
    }
}
