#[cfg(test)]
pub mod column_constraint_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::column_constraint::ColumnConstraint;
    use wcdb::winq::conflict_action::ConflictAction;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(ColumnConstraint::new(None).primary_key(), "PRIMARY KEY");
        WinqTool::winq_equal(
            ColumnConstraint::new(Some("testColumnConstraint")).primary_key(),
            "CONSTRAINT testColumnConstraint PRIMARY KEY",
        );
        WinqTool::winq_equal(
            ColumnConstraint::new(None).primary_key().auto_increment(),
            "PRIMARY KEY AUTOINCREMENT",
        );
        WinqTool::winq_equal(
            ColumnConstraint::new(Some("testColumnConstraint")).not_null(),
            "CONSTRAINT testColumnConstraint NOT NULL",
        );

        WinqTool::winq_equal(
            ColumnConstraint::new(None)
                .not_null()
                .conflict(ConflictAction::Abort),
            "NOT NULL ON CONFLICT ABORT",
        );

        WinqTool::winq_equal(
            ColumnConstraint::new(Some("testColumnConstraint")).unique(),
            "CONSTRAINT testColumnConstraint UNIQUE",
        );
        WinqTool::winq_equal(
            ColumnConstraint::new(Some("testColumnConstraint")).un_index(),
            "CONSTRAINT testColumnConstraint UNINDEXED",
        );
        WinqTool::winq_equal(ColumnConstraint::new(None).default_to(1), "DEFAULT 1");
        WinqTool::winq_equal(
            ColumnConstraint::new(None).default_to(false),
            "DEFAULT FALSE",
        );
        WinqTool::winq_equal(
            ColumnConstraint::new(None).default_to("abc"),
            "DEFAULT 'abc'",
        );
        // todo dengxudong 缺逻辑，重要，不紧急
        // WinqTool::winq_equal(ColumnConstraint::new().default_to(ExpressionConvertible), "DEFAULT NULL");
        WinqTool::winq_equal(
            ColumnConstraint::new(None).collate("BINARY"),
            "COLLATE BINARY",
        );
    }
}
