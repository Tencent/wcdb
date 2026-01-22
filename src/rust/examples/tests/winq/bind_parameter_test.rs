#[cfg(test)]
pub mod bind_parameter_test_test {
    use crate::base::winq_tool::WinqTool;
    use std::ops::Deref;
    use wcdb::winq::bind_parameter;
    use wcdb::winq::bind_parameter::BindParameter;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(&BindParameter::new(1), "?1");
        WinqTool::winq_equal(&BindParameter::new("testName"), ":testName");
        WinqTool::winq_equal(&BindParameter::at("testName"), "@testName");
        WinqTool::winq_equal(&BindParameter::dollar("testName"), "$testName");
        WinqTool::winq_equal(&BindParameter::colon("testName"), ":testName");
        WinqTool::winq_equal(&BindParameter::bind_parameters(5)[4], "?5");

        WinqTool::winq_equal(bind_parameter::DEF.deref(), "?");
        WinqTool::winq_equal(bind_parameter::_1.deref(), "?1");
        WinqTool::winq_equal(bind_parameter::_2.deref(), "?2");
        WinqTool::winq_equal(bind_parameter::_3.deref(), "?3");
        WinqTool::winq_equal(bind_parameter::_4.deref(), "?4");
        WinqTool::winq_equal(bind_parameter::_5.deref(), "?5");
        WinqTool::winq_equal(bind_parameter::_6.deref(), "?6");
        WinqTool::winq_equal(bind_parameter::_7.deref(), "?7");
        WinqTool::winq_equal(bind_parameter::_8.deref(), "?8");
        WinqTool::winq_equal(bind_parameter::_9.deref(), "?9");
        WinqTool::winq_equal(bind_parameter::_10.deref(), "?10");
    }
}
