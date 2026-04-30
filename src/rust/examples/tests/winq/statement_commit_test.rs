#[cfg(test)]
pub mod statement_commit_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::statement_commit::StatementCommit;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(&StatementCommit::new(), "COMMIT");
    }
}
