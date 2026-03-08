#[cfg(test)]
pub mod statement_attach_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::bind_parameter::BindParameter;
    use wcdb::winq::statement_attach::StatementAttach;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(
            StatementAttach::new()
                .attach_with_string("testPath")
                .as_with_name("testSchema"),
            "ATTACH 'testPath' AS testSchema",
        );

        WinqTool::winq_equal(
            StatementAttach::new()
                .attach_with_bind_parameter(BindParameter::new(1))
                .as_with_name("testSchema"),
            "ATTACH ?1 AS testSchema",
        );

        WinqTool::winq_equal(
            StatementAttach::new()
                .attach_with_string("testPath")
                .as_with_name("testSchema")
                .key_with_name("testKey"),
            "ATTACH 'testPath' AS testSchema KEY 'testKey'",
        );

        WinqTool::winq_equal(
            StatementAttach::new()
                .attach_with_bind_parameter(BindParameter::new(1))
                .as_with_name("testSchema")
                .key_with_bind_parameter(BindParameter::new(2)),
            "ATTACH ?1 AS testSchema KEY ?2",
        );
    }
}
