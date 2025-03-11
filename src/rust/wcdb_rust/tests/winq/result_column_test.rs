#[cfg(test)]
pub mod result_column_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb_core::winq::column::Column;
    use wcdb_core::winq::expression_operable_trait::ExpressionOperableTrait;
    use wcdb_core::winq::result_column::ResultColumn;

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
