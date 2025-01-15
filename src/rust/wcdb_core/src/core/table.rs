use crate::base::wcdb_exception::WCDBResult;
use crate::chaincall::insert::Insert;
use crate::chaincall::select::Select;
use crate::core::database::Database;
use crate::core::table_orm_operation::{TableORMOperation, TableORMOperationTrait};
use crate::orm::field::Field;
use crate::orm::table_binding::TableBinding;
use crate::winq::expression::Expression;

pub struct Table<'a, K, R: TableBinding<K>> {
    table_orm_operation: TableORMOperation<'a, K, R>,
}

impl<'a, K, R: TableBinding<K>> TableORMOperationTrait for Table<'a, K, R> {
    fn insert_object<T>(&self, object: T, fields: Vec<&Field<T>>) -> WCDBResult<()> {
        self.table_orm_operation.insert_object(object, fields)
    }
    fn insert_objects<T>(&self, objects: Vec<T>, fields: Vec<&Field<T>>) -> WCDBResult<()> {
        self.table_orm_operation.insert_objects(objects, fields)
    }
    fn prepare_insert<T>(&self) -> Insert<T> {
        self.table_orm_operation.prepare_insert()
    }

    fn prepare_select<T>(&self) -> Select<T> {
        self.table_orm_operation.prepare_select()
    }

    fn get_first_object_by_expression<T>(
        &self,
        fields: Vec<&Field<T>>,
        expression: Expression,
    ) -> WCDBResult<T> {
        self.table_orm_operation
            .get_first_object_by_expression(fields, expression)
    }
}

impl<'a, K, R: TableBinding<K>> Table<'a, K, R> {
    pub fn new(table_name: &str, binding: &'a R, database: &'a Database) -> Table<'a, K, R> {
        Table {
            table_orm_operation: TableORMOperation::new(table_name, binding, database),
        }
    }
}
