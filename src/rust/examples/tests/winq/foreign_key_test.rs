#[cfg(test)]
pub mod foreign_key_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::column::Column;
    use wcdb::winq::foreign_key::{Action, ForeignKey, Initially, Match};

    #[test]
    pub fn test() {
        WinqTool::winq_equal(
            gen_foreign_key().on_delete(Action::SetNull),
            "REFERENCES testForeignKeyTable(column1, column2) ON DELETE SET NULL",
        );

        WinqTool::winq_equal(
            gen_foreign_key().on_update(Action::SetDefault),
            "REFERENCES testForeignKeyTable(column1, column2) ON UPDATE SET DEFAULT",
        );
        WinqTool::winq_equal(
            gen_foreign_key().on_delete(Action::Cascade),
            "REFERENCES testForeignKeyTable(column1, column2) ON DELETE CASCADE",
        );
        WinqTool::winq_equal(
            gen_foreign_key().on_update(Action::Restrict),
            "REFERENCES testForeignKeyTable(column1, column2) ON UPDATE RESTRICT",
        );
        WinqTool::winq_equal(
            gen_foreign_key().on_delete(Action::NoAction),
            "REFERENCES testForeignKeyTable(column1, column2) ON DELETE NO ACTION",
        );

        WinqTool::winq_equal(
            gen_foreign_key().deferrable(Initially::Deferred),
            "REFERENCES testForeignKeyTable(column1, column2) DEFERRABLE INITIALLY DEFERRED",
        );

        WinqTool::winq_equal(
            gen_foreign_key().not_deferrable(Initially::Immediate),
            "REFERENCES testForeignKeyTable(column1, column2) NOT DEFERRABLE INITIALLY IMMEDIATE",
        );
        WinqTool::winq_equal(
            gen_foreign_key().match_(Match::Simple),
            "REFERENCES testForeignKeyTable(column1, column2) MATCH SIMPLE",
        );
    }

    pub fn gen_foreign_key() -> ForeignKey {
        let column1 = Column::new("column1", None);
        let column2 = Column::new("column2", None);
        let key = ForeignKey::new();
        key.references("testForeignKeyTable");
        key.columns(&vec![column1, column2]);
        key
    }
}
