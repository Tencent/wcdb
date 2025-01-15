use crate::base::wcdb_exception::WCDBResult;
use crate::chaincall::insert::Insert;
use crate::chaincall::select::Select;
use crate::core::database::Database;
use crate::core::table_operation::TableOperation;
use crate::orm::field::Field;
use crate::orm::table_binding::TableBinding;
use crate::winq::expression::Expression;
use std::marker::PhantomData;

pub struct TableORMOperation<'a, K, R: TableBinding<K>> {
    table_operation: TableOperation<'a>,
    binding: &'a R,
    _phantom: PhantomData<K>,
}

pub trait TableORMOperationTrait {
    fn insert_object<T>(&self, object: T, fields: Vec<&Field<T>>) -> WCDBResult<()>;

    fn insert_objects<T>(&self, objects: Vec<T>, fields: Vec<&Field<T>>) -> WCDBResult<()>;

    fn prepare_insert<T>(&self) -> Insert<T>;

    fn prepare_select<T>(&self) -> Select<T>;

    fn get_first_object_by_expression<T>(
        &self,
        fields: Vec<&Field<T>>,
        expression: Expression,
    ) -> WCDBResult<T>;
}

impl<'a, K, R: TableBinding<K>> TableORMOperationTrait for TableORMOperation<'a, K, R> {
    fn insert_object<T>(&self, object: T, fields: Vec<&Field<T>>) -> WCDBResult<()> {
        self.prepare_insert::<T>()
            .value(object)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    fn insert_objects<T>(&self, objects: Vec<T>, fields: Vec<&Field<T>>) -> WCDBResult<()> {
        self.prepare_insert::<T>()
            .values(objects)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    fn prepare_insert<T>(&self) -> Insert<T> {
        let mut insert = Insert::new(self.table_operation.get_handle(true), false, true);
        insert = insert.into_table(self.table_operation.get_table_name());
        insert
    }

    fn prepare_select<T>(&self) -> Select<T> {
        let mut select = Select::new(self.table_operation.get_handle(false), false, true);
        select = select.from(self.table_operation.get_table_name());
        select
    }

    fn get_first_object_by_expression<T>(
        &self,
        fields: Vec<&Field<T>>,
        expression: Expression,
    ) -> WCDBResult<T> {
        self.prepare_select()
            .select(fields)
            .where_expression(expression)
            .first_object()
    }
}

impl<'a, K, R: TableBinding<K>> TableORMOperation<'a, K, R> {
    pub fn new(
        table_name: &str,
        binding: &'a R,
        database: &'a Database,
    ) -> TableORMOperation<'a, K, R> {
        TableORMOperation {
            table_operation: TableOperation::new(table_name, database),
            binding,
            _phantom: PhantomData,
        }
    }
}
