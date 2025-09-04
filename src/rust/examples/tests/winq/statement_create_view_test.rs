#[cfg(test)]
pub mod statement_create_view_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::column::Column;
    use wcdb::winq::statement_create_view::StatementCreateView;
    use wcdb::winq::statement_select::StatementSelect;

    #[test]
    pub fn test() {
        let column1 = Column::new("column1", None);
        let column2 = Column::new("column2", None);
        let select = StatementSelect::new()
            .select(&vec![column1, column2])
            .from(&vec!["testTable"]);
        let view = "testView";

        WinqTool::winq_equal(
            StatementCreateView::new()
                .create_view("testView")
                .with_columns(&vec![column1, column2])
                .as_statement_select(&select),
            "CREATE VIEW testView(column1, column2) AS SELECT column1, column2 FROM testTable",
        );

        WinqTool::winq_equal(
            StatementCreateView::new()
                .create_temp_view("testView")
                .with_columns(&vec![column1, column2])
                .as_statement_select(&select),
            "CREATE TEMP VIEW testView(column1, column2) AS SELECT column1, column2 FROM testTable",
        );

        WinqTool::winq_equal(
            StatementCreateView::new().create_view("testView").of_with_string("testSchema").with_columns(&vec![column1, column2]).as_statement_select(&select),
            "CREATE VIEW testSchema.testView(column1, column2) AS SELECT column1, column2 FROM testTable"
        );

        WinqTool::winq_equal(
            StatementCreateView::new().create_view("testView").if_not_exist().with_columns(&vec![column1, column2]).as_statement_select(&select),
            "CREATE VIEW IF NOT EXISTS testView(column1, column2) AS SELECT column1, column2 FROM testTable"
        );
    }
}
