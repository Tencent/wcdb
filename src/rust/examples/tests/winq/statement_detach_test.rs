#[cfg(test)]
pub mod statement_detach_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::statement_detach::StatementDetach;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(
            &StatementDetach::new().detach_with_string("testSchema"),
            "DETACH testSchema",
        );
    }
}
