use crate::core::database::Database;

pub struct TableOperation<'a> {
    table_name: String,
    database: &'a Database, 
}

impl<'a> TableOperation<'a> {
    pub fn new(table_name: &str, database: &'a Database) -> TableOperation<'a> {
        TableOperation {
            table_name: table_name.to_string(),
            database,
        }
    }
}