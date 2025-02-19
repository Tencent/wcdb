use crate::base::wcdb_exception::WCDBResult;
use crate::chaincall::delete::Delete;
use crate::chaincall::insert::Insert;
use crate::chaincall::select::Select;
use crate::chaincall::update::Update;
use crate::core::database::Database;
use crate::core::table_orm_operation::{TableORMOperation, TableORMOperationTrait};
use crate::orm::field::Field;
use crate::orm::table_binding::TableBinding;
use crate::winq::expression::Expression;
use crate::winq::ordering_term::OrderingTerm;

pub struct Table<'a, T, R: TableBinding<T>> {
    table_orm_operation: TableORMOperation<'a, T, R>,
}

impl<'a, T, R: TableBinding<T>> TableORMOperationTrait<T> for Table<'a, T, R> {
    fn insert_object(&self, object: T, fields: Vec<&Field<T>>) -> WCDBResult<()> {
        self.table_orm_operation.insert_object(object, fields)
    }

    fn insert_or_replace_object(&self, object: T, fields: Vec<&Field<T>>) -> WCDBResult<()> {
        self.table_orm_operation
            .insert_or_replace_object(object, fields)
    }

    fn insert_or_ignore_object(&self, object: T, fields: Vec<&Field<T>>) -> WCDBResult<()> {
        self.table_orm_operation
            .insert_or_ignore_object(object, fields)
    }

    fn insert_objects(&self, objects: Vec<T>, fields: Vec<&Field<T>>) -> WCDBResult<()> {
        self.table_orm_operation.insert_objects(objects, fields)
    }

    fn insert_or_replace_objects(&self, objects: Vec<T>, fields: Vec<&Field<T>>) -> WCDBResult<()> {
        self.table_orm_operation
            .insert_or_replace_objects(objects, fields)
    }

    fn insert_or_ignore_objects(&self, objects: Vec<T>, fields: Vec<&Field<T>>) -> WCDBResult<()> {
        self.table_orm_operation
            .insert_or_ignore_objects(objects, fields)
    }

    fn prepare_insert(&self) -> Insert<T> {
        self.table_orm_operation.prepare_insert()
    }

    fn prepare_update(&self) -> Update<T> {
        self.table_orm_operation.prepare_update()
    }

    fn prepare_select(&self) -> Select<T> {
        self.table_orm_operation.prepare_select()
    }

    fn prepare_delete(&self) -> Delete {
        self.table_orm_operation.prepare_delete()
    }

    fn delete_objects(&self) -> WCDBResult<()> {
        self.table_orm_operation.delete_objects()
    }

    fn delete_objects_by_expression(&self, expression: Expression) -> WCDBResult<()> {
        self.table_orm_operation
            .delete_objects_by_expression(expression)
    }

    fn delete_objects_by_expression_order_limit(
        &self,
        expression: Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()> {
        self.table_orm_operation
            .delete_objects_by_expression_order_limit(expression, order, limit)
    }

    fn delete_objects_by_expression_order_limit_offset(
        &self,
        expression: Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()> {
        self.table_orm_operation
            .delete_objects_by_expression_order_limit_offset(expression, order, limit, offset)
    }

    fn delete_objects_by_order_limit(&self, order: OrderingTerm, limit: i64) -> WCDBResult<()> {
        self.table_orm_operation
            .delete_objects_by_order_limit(order, limit)
    }

    fn delete_objects_by_order_limit_offset(
        &self,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()> {
        self.table_orm_operation
            .delete_objects_by_order_limit_offset(order, limit, offset)
    }

    fn update_object_by_field(&self, object: T, field: &Field<T>) -> WCDBResult<()> {
        self.table_orm_operation
            .update_object_by_field(object, field)
    }

    fn update_object_by_field_expression(
        &self,
        object: T,
        field: &Field<T>,
        expression: Expression,
    ) -> WCDBResult<()> {
        self.table_orm_operation
            .update_object_by_field_expression(object, field, expression)
    }

    fn update_object_by_field_expression_order_limit(
        &self,
        object: T,
        field: &Field<T>,
        expression: Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()> {
        self.table_orm_operation
            .update_object_by_field_expression_order_limit(object, field, expression, order, limit)
    }

    fn update_object_by_field_expression_order_limit_offset(
        &self,
        object: T,
        field: &Field<T>,
        expression: Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()> {
        self.table_orm_operation
            .update_object_by_field_expression_order_limit_offset(
                object, field, expression, order, limit, offset,
            )
    }

    fn update_object_by_field_order_limit(
        &self,
        object: T,
        field: &Field<T>,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()> {
        self.table_orm_operation
            .update_object_by_field_order_limit(object, field, order, limit)
    }

    fn update_object_by_field_order_limit_offset(
        &self,
        object: T,
        field: &Field<T>,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()> {
        self.table_orm_operation
            .update_object_by_field_order_limit_offset(object, field, order, limit, offset)
    }

    fn update_object_by_fields(&self, object: T, fields: Vec<&Field<T>>) -> WCDBResult<()> {
        self.table_orm_operation
            .update_object_by_fields(object, fields)
    }

    fn update_object_by_fields_expression(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        expression: Expression,
    ) -> WCDBResult<()> {
        self.table_orm_operation
            .update_object_by_fields_expression(object, fields, expression)
    }

    fn update_object_by_fields_expression_order_limit(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        expression: Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()> {
        self.table_orm_operation
            .update_object_by_fields_expression_order_limit(
                object, fields, expression, order, limit,
            )
    }

    fn update_object_by_fields_expression_order_limit_offset(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        expression: Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()> {
        self.table_orm_operation
            .update_object_by_fields_expression_order_limit_offset(
                object, fields, expression, order, limit, offset,
            )
    }

    fn update_object_by_fields_order_limit(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()> {
        self.table_orm_operation
            .update_object_by_fields_order_limit(object, fields, order, limit)
    }

    fn update_object_by_fields_order_limit_offset(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()> {
        self.table_orm_operation
            .update_object_by_fields_order_limit_offset(object, fields, order, limit, offset)
    }

    fn get_all_objects(&self) -> WCDBResult<Vec<T>> {
        self.table_orm_operation.get_all_objects()
    }

    fn get_all_objects_by_expression(&self, condition: Expression) -> WCDBResult<Vec<T>> {
        self.table_orm_operation
            .get_all_objects_by_expression(condition)
    }

    fn get_all_objects_by_expression_order(
        &self,
        condition: Expression,
        order: OrderingTerm,
    ) -> WCDBResult<Vec<T>> {
        self.table_orm_operation
            .get_all_objects_by_expression_order(condition, order)
    }

    fn get_all_objects_by_expression_order_limit(
        &self,
        condition: Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<Vec<T>> {
        self.table_orm_operation
            .get_all_objects_by_expression_order_limit(condition, order, limit)
    }

    fn get_all_objects_by_expression_order_limit_offset(
        &self,
        condition: Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<Vec<T>> {
        self.table_orm_operation
            .get_all_objects_by_expression_order_limit_offset(condition, order, limit, offset)
    }

    fn get_all_objects_order(&self, order: OrderingTerm) -> WCDBResult<Vec<T>> {
        self.table_orm_operation.get_all_objects_order(order)
    }

    fn get_all_objects_order_limit(&self, order: OrderingTerm, limit: i64) -> WCDBResult<Vec<T>> {
        self.table_orm_operation
            .get_all_objects_order_limit(order, limit)
    }

    fn get_all_objects_order_limit_offset(
        &self,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<Vec<T>> {
        self.table_orm_operation
            .get_all_objects_order_limit_offset(order, limit, offset)
    }

    fn get_all_objects_by_fields(&self, fields: Vec<&Field<T>>) -> WCDBResult<Vec<T>> {
        self.table_orm_operation.get_all_objects_by_fields(fields)
    }

    fn get_all_objects_by_fields_expression(
        &self,
        fields: Vec<&Field<T>>,
        condition: Expression,
    ) -> WCDBResult<Vec<T>> {
        self.table_orm_operation
            .get_all_objects_by_fields_expression(fields, condition)
    }

    fn get_all_objects_by_fields_expression_order(
        &self,
        fields: Vec<&Field<T>>,
        condition: Expression,
        order: OrderingTerm,
    ) -> WCDBResult<Vec<T>> {
        self.table_orm_operation
            .get_all_objects_by_fields_expression_order(fields, condition, order)
    }

    fn get_all_objects_by_fields_expression_order_limit(
        &self,
        fields: Vec<&Field<T>>,
        condition: Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<Vec<T>> {
        self.table_orm_operation
            .get_all_objects_by_fields_expression_order_limit(fields, condition, order, limit)
    }

    fn get_all_objects_by_fields_expression_order_limit_offset(
        &self,
        fields: Vec<&Field<T>>,
        condition: Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<Vec<T>> {
        self.table_orm_operation
            .get_all_objects_by_fields_expression_order_limit_offset(
                fields, condition, order, limit, offset,
            )
    }

    fn get_all_objects_by_fields_order(
        &self,
        fields: Vec<&Field<T>>,
        order: OrderingTerm,
    ) -> WCDBResult<Vec<T>> {
        self.table_orm_operation
            .get_all_objects_by_fields_order(fields, order)
    }

    fn get_all_objects_by_fields_order_limit(
        &self,
        fields: Vec<&Field<T>>,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<Vec<T>> {
        self.table_orm_operation
            .get_all_objects_by_fields_order_limit(fields, order, limit)
    }

    fn get_all_objects_by_fields_order_limit_offset(
        &self,
        fields: Vec<&Field<T>>,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<Vec<T>> {
        self.table_orm_operation
            .get_all_objects_by_fields_order_limit_offset(fields, order, limit, offset)
    }

    fn get_first_object(&self, fields: Vec<&Field<T>>) -> WCDBResult<T> {
        self.table_orm_operation.get_first_object(fields)
    }

    fn get_first_object_by_expression(
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
