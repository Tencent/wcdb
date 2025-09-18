#[cfg(test)]
pub mod column_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::column::{Column, ColumnStaticTrait, ColumnTrait};
    use wcdb::winq::schema::Schema;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(&Column::all(), "*");
        WinqTool::winq_equal(&Column::row_id(), "rowid");
        WinqTool::winq_equal(&Column::row_id().as_("rowidAlias"), "rowid AS rowidAlias");
        WinqTool::winq_equal(&Column::new("testColumn", None), "testColumn");
        WinqTool::winq_equal(
            Column::new("testColumn", None).table("testTable"),
            "testTable.testColumn",
        );
        WinqTool::winq_equal(
            Column::new("testColumn", None)
                .table("testTable")
                .of("testSchema"),
            "testSchema.testTable.testColumn",
        );
        WinqTool::winq_equal(
            Column::new("testColumn", None)
                .table("testTable")
                .of(&Schema::new("testSchema")),
            "testSchema.testTable.testColumn",
        );
    }
}
