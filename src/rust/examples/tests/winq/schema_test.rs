#[cfg(test)]
pub mod schema_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::schema::{Schema, SchemaTrait};

    #[test]
    pub fn test() {
        WinqTool::winq_equal(&Schema::main(), "main");
        WinqTool::winq_equal(&Schema::temp(), "temp");
        WinqTool::winq_equal(&Schema::new("testSchema"), "testSchema");
    }
}
