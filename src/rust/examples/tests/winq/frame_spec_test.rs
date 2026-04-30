#[cfg(test)]
pub mod frame_spec_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::frame_spec::FrameSpec;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(
            FrameSpec::new().range().unbounded_preceding(),
            "RANGE UNBOUNDED PRECEDING",
        );
        WinqTool::winq_equal(
            FrameSpec::new().rows().unbounded_preceding(),
            "ROWS UNBOUNDED PRECEDING",
        );
        WinqTool::winq_equal(FrameSpec::new().range().preceding(1), "RANGE 1 PRECEDING");
        WinqTool::winq_equal(FrameSpec::new().range().current_row(), "RANGE CURRENT ROW");
        WinqTool::winq_equal(
            FrameSpec::new()
                .range()
                .between_unbounded_preceding()
                .and_preceding(2),
            "RANGE BETWEEN UNBOUNDED PRECEDING AND 2 PRECEDING",
        );
        WinqTool::winq_equal(
            FrameSpec::new()
                .range()
                .between_preceding(1)
                .and_preceding(2),
            "RANGE BETWEEN 1 PRECEDING AND 2 PRECEDING",
        );
        WinqTool::winq_equal(
            FrameSpec::new()
                .range()
                .between_current_row()
                .and_preceding(2),
            "RANGE BETWEEN CURRENT ROW AND 2 PRECEDING",
        );
        WinqTool::winq_equal(
            FrameSpec::new()
                .range()
                .between_following(1)
                .and_preceding(2),
            "RANGE BETWEEN 1 FOLLOWING AND 2 PRECEDING",
        );
        WinqTool::winq_equal(
            FrameSpec::new()
                .range()
                .between_unbounded_preceding()
                .and_current_row(),
            "RANGE BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW",
        );
        WinqTool::winq_equal(
            FrameSpec::new()
                .range()
                .between_unbounded_preceding()
                .and_following(2),
            "RANGE BETWEEN UNBOUNDED PRECEDING AND 2 FOLLOWING",
        );
        WinqTool::winq_equal(
            FrameSpec::new()
                .range()
                .between_unbounded_preceding()
                .and_unbounded_following(),
            "RANGE BETWEEN UNBOUNDED PRECEDING AND UNBOUNDED FOLLOWING",
        );
    }
}
