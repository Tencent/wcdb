#[cfg(test)]
pub mod statement_pragma_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::pragma::Pragma;
    use wcdb::winq::statement_pragma::StatementPragma;

    #[test]
    pub fn test() {
        let pragma = Pragma::new("page_size");
        let statement = StatementPragma::new();

        let test = statement.pragma(pragma.unwrap());
        WinqTool::winq_equal(test, "PRAGMA page_size");

        let pragma = Pragma::new("secureDelete");
        let test = statement.pragma(pragma.unwrap()).to_value(1);
        WinqTool::winq_equal(test, "PRAGMA secureDelete = 1");
    }
}
