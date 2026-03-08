#[cfg(test)]
pub mod window_def_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::column::{Column, ColumnTrait};
    use wcdb::winq::expression::Expression;
    use wcdb::winq::expression_operable::ExpressionOperableTrait;
    use wcdb::winq::frame_spec::FrameSpec;
    use wcdb::winq::ordering_term::{Order, OrderingTerm};
    use wcdb::winq::window_def::WindowDef;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(&WindowDef::new(), "");

        let window_def = WindowDef::new().partition_by_with_string(&vec!["column1"]);
        WinqTool::winq_equal(&window_def, "(PARTITION BY column1)");

        let window_def = WindowDef::new().partition_by_with_string(&vec!["column1", "column2"]);
        WinqTool::winq_equal(&window_def, "(PARTITION BY column1, column2)");

        let column1 = Column::new("column1", None).add(1);
        let window_def = WindowDef::new().partition(&vec![&column1]);
        WinqTool::winq_equal(&window_def, "(PARTITION BY column1 + 1)");

        let column1 = Column::new("column1", None).add(1);
        let column2 = Column::new("column2", None);
        let expression = Expression::new(&column2);
        let window_def = WindowDef::new().partition(&vec![&column1, &expression]);
        WinqTool::winq_equal(&window_def, "(PARTITION BY column1 + 1, column2)");

        let ordering_term: OrderingTerm = Column::new("column1", None).order(Order::Asc);
        let window_def = WindowDef::new().order_by(&vec![&ordering_term]);
        WinqTool::winq_equal(&window_def, "(ORDER BY column1 ASC)");

        let frame_spec = FrameSpec::new().range().unbounded_preceding();
        let window_def = WindowDef::new().frame_spec(&frame_spec);
        WinqTool::winq_equal(&window_def, "(RANGE UNBOUNDED PRECEDING)");

        let column1 = Column::new("column1", None).add(1);
        column1.add(1);
        let column2 = Column::new("column2", None);
        let expression = Expression::new(&column2);
        let ordering_term: OrderingTerm = Column::new("column1", None).order(Order::Asc);
        let frame_spec = FrameSpec::new().range().unbounded_preceding();
        let window_def = WindowDef::new()
            .partition(&vec![&column1, &expression])
            .order_by(&vec![&ordering_term])
            .frame_spec(&frame_spec);
        WinqTool::winq_equal(
            &window_def,
            "(PARTITION BY column1 + 1, column2 ORDER BY column1 ASC RANGE UNBOUNDED PRECEDING)",
        );
    }
}
