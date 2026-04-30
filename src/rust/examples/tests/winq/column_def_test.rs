#[cfg(test)]
pub mod column_def_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::column::Column;
    use wcdb::winq::column_def::ColumnDef;
    use wcdb::winq::column_type::ColumnType;
    use wcdb::winq::foreign_key::ForeignKey;
    use wcdb::winq::literal_value::LiteralValue;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(&ColumnDef::new("testColumn"), "testColumn");
        WinqTool::winq_equal(
            &ColumnDef::new(("testColumn", ColumnType::Integer)),
            "testColumn INTEGER",
        );
        WinqTool::winq_equal(
            &ColumnDef::new(&Column::new("testColumn", None)),
            "testColumn",
        );
        WinqTool::winq_equal(
            &ColumnDef::new((&Column::new("testColumn", None), ColumnType::Integer)),
            "testColumn INTEGER",
        );
        WinqTool::winq_equal(
            &ColumnDef::new(("testColumn", ColumnType::Integer)),
            "testColumn INTEGER",
        );
        WinqTool::winq_equal(
            gen_column_def().make_primary(None),
            "testColumn INTEGER PRIMARY KEY",
        );
        WinqTool::winq_equal(
            gen_column_def().make_primary(Some(true)),
            "testColumn INTEGER PRIMARY KEY AUTOINCREMENT",
        );
        WinqTool::winq_equal(
            gen_column_def().make_primary(Some(false)),
            "testColumn INTEGER PRIMARY KEY",
        );
        WinqTool::winq_equal(
            gen_column_def().make_not_null(),
            "testColumn INTEGER NOT NULL",
        );
        WinqTool::winq_equal(
            gen_column_def().make_default_to(&LiteralValue::current_date()),
            "testColumn INTEGER DEFAULT CURRENT_DATE",
        );
        WinqTool::winq_equal(
            gen_column_def().make_default_to(&LiteralValue::current_time()),
            "testColumn INTEGER DEFAULT CURRENT_TIME",
        );
        WinqTool::winq_equal(
            gen_column_def().make_default_to(&LiteralValue::current_time_stamp()),
            "testColumn INTEGER DEFAULT CURRENT_TIMESTAMP",
        );
        WinqTool::winq_equal(
            gen_column_def().make_foreign_key(&ForeignKey::new()),
            "testColumn INTEGER REFERENCES ",
        );
        WinqTool::winq_equal(gen_column_def().make_unique(), "testColumn INTEGER UNIQUE");
        WinqTool::winq_equal(
            gen_column_def().make_not_indexed(),
            "testColumn INTEGER UNINDEXED",
        );
    }

    fn gen_column_def() -> ColumnDef {
        ColumnDef::new(("testColumn", ColumnType::Integer))
    }
}
