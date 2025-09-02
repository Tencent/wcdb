#[cfg(test)]
pub mod result_column_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::column::Column;
    use wcdb::winq::result_column::ResultColumn;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(&ResultColumn::new("testColumn"), "testColumn");
        WinqTool::winq_equal(
            &ResultColumn::new(&Column::new("testColumn", None)),
            "testColumn",
        );
        WinqTool::winq_equal(
            &ResultColumn::new(&Column::new("testColumn", None)).r#as("testColumn2"),
            "testColumn AS testColumn2",
        );
        WinqTool::winq_equal(
            &ResultColumn::new(&Column::new("testColumn", None).sum()).r#as("sum"),
            "SUM(testColumn) AS sum",
        );
    }
}
