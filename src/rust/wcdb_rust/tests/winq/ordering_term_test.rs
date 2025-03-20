#[cfg(test)]
pub mod ordering_term_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb_core::winq::column::Column;
    use wcdb_core::winq::ordering_term::{Order, OrderingTerm};

    #[test]
    pub fn test() {
        let column = Column::new("testColumn");
        WinqTool::winq_equal(&OrderingTerm::new(&column), "testColumn");
        WinqTool::winq_equal(
            &OrderingTerm::new(&column).order(Order::Asc),
            "testColumn ASC",
        );
        WinqTool::winq_equal(
            &OrderingTerm::new(&column).order(Order::Desc),
            "testColumn DESC",
        );
    }
}
