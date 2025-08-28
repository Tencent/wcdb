#[cfg(test)]
pub mod qualified_table_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::qualified_table::QualifiedTable;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(
            &QualifiedTable::new_with_table_name("testTable"),
            "testTable",
        );
        WinqTool::winq_equal(
            QualifiedTable::new_with_table_name("testTable")
                .of_string("testSchema")
                .as_("testAlias"),
            "testSchema.testTable AS testAlias",
        );
        WinqTool::winq_equal(
            QualifiedTable::new_with_table_name("testTable").indexed("testIndex"),
            "testTable INDEXED BY testIndex",
        );
        WinqTool::winq_equal(
            QualifiedTable::new_with_table_name("testTable").not_indexed(),
            "testTable NOT INDEXED",
        );
    }
}
