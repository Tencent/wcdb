use crate::core::database::Database;
use crate::core::table_operation::TableOperation;
use crate::orm::table_binding::TableBinding;
use std::marker::PhantomData;

pub struct TableORMOperation<'a, T, R: TableBinding<T>> {
    table_operation: TableOperation<'a>,
    binding: &'a R,
    _phantom: PhantomData<T>,
}

impl<'a, T, R: TableBinding<T>> TableORMOperation<'a, T, R> {
    pub fn new(
        table_name: &str,
        binding: &'a R,
        database: &'a Database,
    ) -> TableORMOperation<'a, T, R> {
        TableORMOperation {
            table_operation: TableOperation::new(table_name, database),
            binding,
            _phantom: PhantomData,
        }
    }
}
