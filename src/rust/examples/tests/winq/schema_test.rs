#[cfg(test)]
pub mod schema_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::schema::Schema;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(&Schema::main(), "main");
        WinqTool::winq_equal(&Schema::temp(), "temp");
        WinqTool::winq_equal(&Schema::new_with_table_name("testSchema"), "testSchema");
    }
}
