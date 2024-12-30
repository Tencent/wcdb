use crate::core::database::Database;
use crate::core::table_orm_operation::TableORMOperation;
use crate::orm::table_binding::TableBinding;

pub struct Table<'a, T, R: TableBinding<T>> {
    table_orm_operation: TableORMOperation<'a, T, R>,
}

impl <'a, T, R: TableBinding<T>> Table<'a, T, R> {
    pub fn new(table_name: &str, binding: &'a R, database: &'a Database) -> Table<'a, T, R> {
        Table {
            table_orm_operation: TableORMOperation::new(table_name, binding, database),
        }
    }
}