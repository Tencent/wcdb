#[cfg(test)]
pub mod statement_explain_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::column::Column;
    use wcdb::winq::statement_explain::StatementExplain;
    use wcdb::winq::statement_select::StatementSelect;

    #[test]
    pub fn test() {
        let select = StatementSelect::new();
        select.select(vec!["testColumn"]).from(vec!["testTable"]);
        WinqTool::winq_equal(
            StatementExplain::new().explain(&select),
            "EXPLAIN SELECT testColumn FROM testTable",
        );

        WinqTool::winq_equal(
            StatementExplain::new().explain_query_plan(select),
            "EXPLAIN QUERY PLAN SELECT testColumn FROM testTable",
        );
    }
}
