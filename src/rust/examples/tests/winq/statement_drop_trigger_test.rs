#[cfg(test)]
pub mod statement_drop_trigger_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::statement_drop_trigger::StatementDropTrigger;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(
            StatementDropTrigger::new().drop_trigger("testTrigger"),
            "DROP TRIGGER testTrigger",
        );
        WinqTool::winq_equal(
            StatementDropTrigger::new()
                .drop_trigger("testTrigger")
                .if_exist(),
            "DROP TRIGGER IF EXISTS testTrigger",
        );
        WinqTool::winq_equal(
            StatementDropTrigger::new()
                .drop_trigger("testTrigger")
                .of_with_string("testSchema"),
            "DROP TRIGGER testSchema.testTrigger",
        );
    }
}
