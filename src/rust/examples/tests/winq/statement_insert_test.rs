#[cfg(test)]
pub mod statement_insert_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::column::Column;
    use wcdb::winq::object::Object;
    use wcdb::winq::statement_insert::StatementInsert;
    use wcdb::winq::upsert::Upsert;

    #[test]
    pub fn test() {
        let test_table = "testTable";
        let statement = StatementInsert::new();

        let test = statement
            .insert_into(test_table)
            .values_with_bind_parameters(1);
        WinqTool::winq_equal(test, "INSERT INTO testTable VALUES(?1)");
    }

    #[test]
    pub fn replace() {
        let test_table = "testTable";
        let statement = StatementInsert::new();

        let test = statement
            .insert_into(test_table)
            .values_with_bind_parameters(1)
            .or_replace();
        WinqTool::winq_equal(test, "INSERT OR REPLACE INTO testTable VALUES(?1)");

        let upsert = Upsert::new();
        upsert.on_conflict().do_no_thing();
        let statement = StatementInsert::new();
        statement
            .insert_into("testTable")
            .column_objs(&vec![Column::new("testColumn")])
            .values(Some(vec![Object::Int(1)]))
            .upsert(&upsert);
        WinqTool::winq_equal(
            &statement,
            "INSERT INTO testTable(testColumn) VALUES(1) ON CONFLICT DO NOTHING",
        );
    }

    #[test]
    pub fn ignore() {
        let test_table = "testTable";
        let statement = StatementInsert::new();

        let test = statement
            .insert_into(test_table)
            .values_with_bind_parameters(1)
            .or_ignore();
        WinqTool::winq_equal(test, "INSERT OR IGNORE INTO testTable VALUES(?1)");
    }

    #[test]
    pub fn fail() {
        let test_table = "testTable";
        let statement = StatementInsert::new();

        let test = statement
            .insert_into(test_table)
            .values_with_bind_parameters(1)
            .or_fail();
        WinqTool::winq_equal(test, "INSERT OR FAIL INTO testTable VALUES(?1)");
    }

    #[test]
    pub fn rollback() {
        let test_table = "testTable";
        let statement = StatementInsert::new();

        let test = statement
            .insert_into(test_table)
            .values_with_bind_parameters(1)
            .or_rollback();
        WinqTool::winq_equal(test, "INSERT OR ROLLBACK INTO testTable VALUES(?1)");
    }

    #[test]
    pub fn abort() {
        let test_table = "testTable";
        let statement = StatementInsert::new();

        let test = statement
            .insert_into(test_table)
            .values_with_bind_parameters(1)
            .or_abort();
        WinqTool::winq_equal(test, "INSERT OR ABORT INTO testTable VALUES(?1)");
    }

    #[test]
    pub fn default_values() {
        let test_table = "testTable";
        let statement = StatementInsert::new();

        let test = statement.insert_into(test_table).default_values();
        WinqTool::winq_equal(test, "INSERT INTO testTable DEFAULT VALUES");
    }
}
