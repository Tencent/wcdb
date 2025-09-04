#[cfg(test)]
pub mod statement_drop_view_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::statement_drop_view::StatementDropView;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(
            StatementDropView::new().drop_view("testView"),
            "DROP VIEW testView",
        );
        WinqTool::winq_equal(
            StatementDropView::new().drop_view("testView").if_exist(),
            "DROP VIEW IF EXISTS testView",
        );
        WinqTool::winq_equal(
            StatementDropView::new()
                .drop_view("testView")
                .of_with_string("testSchema"),
            "DROP VIEW testSchema.testView",
        );
    }
}
