#[cfg(test)]
pub mod statement_begin_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::statement_begin::StatementBegin;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(&StatementBegin::new(None), "BEGIN DEFERRED");
        WinqTool::winq_equal(&StatementBegin::begin_deferred(), "BEGIN DEFERRED");
        WinqTool::winq_equal(&StatementBegin::begin_immediate(), "BEGIN IMMEDIATE");
        WinqTool::winq_equal(&StatementBegin::begin_exclusive(), "BEGIN EXCLUSIVE");
    }
}
