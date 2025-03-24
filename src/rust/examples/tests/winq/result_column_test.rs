#[cfg(test)]
pub mod result_column_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::column::Column;
    use wcdb::winq::expression_operable_trait::ExpressionOperableTrait;
    use wcdb::winq::result_column::ResultColumn;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(
            &ResultColumn::new_with_column_name("testColumn"),
            "testColumn",
        );
        WinqTool::winq_equal(
            &ResultColumn::new_with_result_column_convertible(&Column::new("testColumn")),
            "testColumn",
        );
        WinqTool::winq_equal(
            &ResultColumn::new_with_result_column_convertible(&Column::new("testColumn"))
                .as_("testColumn2"),
            "testColumn AS testColumn2",
        );
        WinqTool::winq_equal(
            &ResultColumn::new_with_result_column_convertible(&Column::new("testColumn").sum())
                .as_("sum"),
            "SUM(testColumn) AS sum",
        );
    }
}
