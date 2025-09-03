#[cfg(test)]
pub mod statement_vacuum_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::statement_vacuum::StatementVacuum;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(&StatementVacuum::new(), "VACUUM");
        WinqTool::winq_equal(
            &StatementVacuum::new().vacuum_with_string("testSchema"),
            "VACUUM testSchema",
        );
    }
}
