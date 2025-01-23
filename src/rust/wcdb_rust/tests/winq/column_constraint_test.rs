#[cfg(test)]
pub mod column_constraint_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb_core::winq::column_constraint::ColumnConstraint;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(ColumnConstraint::new().primary_key(), "PRIMARY KEY");
        WinqTool::winq_equal(
            ColumnConstraint::new_by_column_name("testColumnConstraint").primary_key(),
            "CONSTRAINT testColumnConstraint PRIMARY KEY",
        );
        WinqTool::winq_equal(
            ColumnConstraint::new().primary_key().auto_increment(),
            "PRIMARY KEY AUTOINCREMENT",
        );
    }
}
