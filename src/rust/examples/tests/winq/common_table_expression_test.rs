#[cfg(test)]
pub mod common_table_expression_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::column::Column;
    use wcdb::winq::common_table_expression::CommonTableExpression;
    use wcdb::winq::statement_select::StatementSelect;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(
            CommonTableExpression::new("testTable")
                .as_(StatementSelect::new().select(vec!["columnA"])),
            "testTable AS(SELECT columnA)",
        );

        let column_a = Column::new("columnA", None);
        let column_b = Column::new("columnB", None);
        WinqTool::winq_equal(
            CommonTableExpression::new("testTable")
                .column(&column_a)
                .as_(StatementSelect::new().select(vec!["columnB"])),
            "testTable(columnA) AS(SELECT columnB)",
        );

        WinqTool::winq_equal(
            CommonTableExpression::new("testTable")
                .column(&column_a)
                .column(&column_b)
                .as_(StatementSelect::new().select(vec!["columnC", "columnD"])),
            "testTable(columnA, columnB) AS(SELECT columnC, columnD)",
        );
    }
}
