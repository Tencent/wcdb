#[cfg(test)]
pub mod statement_create_virtual_table_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::statement_create_virtual_table::StatementCreateVirtualTable;

    #[test]
    pub fn test() {
        let virtual_table1 = "virtualTable1";
        let tokenizer = "tokenize=WCDB";
        let virtual_table2 = "virtualTable2";
        let module = "testModule";
        let argument = "left=right";
        WinqTool::winq_equal(
            StatementCreateVirtualTable::new()
                .create_virtual_table(virtual_table1)
                .if_not_exist()
                .using_module("fts3")
                .arguments(vec![tokenizer.to_string()]),
            "CREATE VIRTUAL TABLE IF NOT EXISTS virtualTable1 USING fts3(tokenize=WCDB)",
        );

        WinqTool::winq_equal(
            StatementCreateVirtualTable::new()
                .create_virtual_table(virtual_table2)
                .using_module(module)
                .arguments(vec![argument.to_string()]),
            "CREATE VIRTUAL TABLE virtualTable2 USING testModule(left=right)",
        );
    }
}
