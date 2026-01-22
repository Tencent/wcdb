use crate::base::value::{Value, ValueObject};
use crate::base::wcdb_exception::WCDBResult;
use crate::chaincall::delete::Delete;
use crate::chaincall::insert::Insert;
use crate::chaincall::select::Select;
use crate::chaincall::update::Update;
use crate::core::database::Database;
use crate::core::table_operation::TableOperationTrait;
use crate::core::table_orm_operation::{TableORMOperation, TableORMOperationTrait};
use crate::orm::field::Field;
use crate::orm::table_binding::TableBinding;
use crate::winq::column::Column;
use crate::winq::expression::Expression;
use crate::winq::ordering_term::OrderingTerm;
use std::marker::PhantomData;

pub struct Table<'a, T, R: TableBinding<T>> {
    table_orm_operation: TableORMOperation<'a, T, R>,
    _phantom: PhantomData<T>,
}

impl<'a, T, R: TableBinding<T>> TableOperationTrait for Table<'a, T, R> {
    fn get_table_name(&self) -> &str {
        self.table_orm_operation.get_table_name()
    }

    fn get_database(&self) -> &Database {
        self.table_orm_operation.get_database()
    }

    fn insert_rows(&self, rows: Vec<Vec<Value>>, columns: Vec<Column>) -> WCDBResult<()> {
        self.table_orm_operation.insert_rows(rows, columns)
    }

    fn insert_or_replace_rows(
        &self,
        rows: Vec<Vec<Value>>,
        columns: Vec<Column>,
    ) -> WCDBResult<()> {
        self.table_orm_operation
            .insert_or_replace_rows(rows, columns)
    }

    fn insert_or_ignore_rows(&self, rows: Vec<Vec<Value>>, columns: Vec<Column>) -> WCDBResult<()> {
        self.table_orm_operation
            .insert_or_ignore_rows(rows, columns)
    }

    fn update_value<V: Into<ValueObject>>(
        &self,
        value: V,
        column: Column,
        condition_opt: Option<&Expression>,
        order_opt: Option<&OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()> {
        self.table_orm_operation.update_value(
            value,
            column,
            condition_opt,
            order_opt,
            limit_opt,
            offset_opt,
        )
    }

    fn update_row(
        &self,
        row: &Vec<Value>,
        columns: &Vec<Column>,
        condition_opt: Option<&Expression>,
        order_opt: Option<&OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()> {
        self.table_orm_operation.update_row(
            row,
            columns,
            condition_opt,
            order_opt,
            limit_opt,
            offset_opt,
        )
    }

    fn delete_value(
        &self,
        condition_opt: Option<&Expression>,
        order_opt: Option<&OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()> {
        self.table_orm_operation
            .delete_value(condition_opt, order_opt, limit_opt, offset_opt)
    }

    fn get_values(
        &self,
        columns: Vec<&Column>,
        condition_opt: Option<&Expression>,
        order_opt: Option<Vec<&OrderingTerm>>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<Vec<Vec<Value>>> {
        self.table_orm_operation.get_values(
            columns,
            condition_opt,
            order_opt,
            limit_opt,
            offset_opt,
        )
    }
}

impl<'a, T, R: TableBinding<T>> TableORMOperationTrait<'a, T, R> for Table<'a, T, R> {
    fn get_binding(&self) -> &'a R {
        self.table_orm_operation.get_binding()
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

    fn insert_object(&self, object: T, fields_opt: Option<Vec<&Field<T>>>) -> WCDBResult<()> {
        self.table_orm_operation.insert_object(object, fields_opt)
    }

    fn insert_or_replace_object(
        &self,
        object: T,
        fields_opt: Option<Vec<&Field<T>>>,
    ) -> WCDBResult<()> {
        self.table_orm_operation
            .insert_or_replace_object(object, fields_opt)
    }

    fn insert_or_ignore_object(
        &self,
        object: T,
        fields_opt: Option<Vec<&Field<T>>>,
    ) -> WCDBResult<()> {
        self.table_orm_operation
            .insert_or_ignore_object(object, fields_opt)
    }

    fn insert_objects(
        &self,
        objects: Vec<T>,
        fields_opt: Option<Vec<&Field<T>>>,
    ) -> WCDBResult<()> {
        self.table_orm_operation.insert_objects(objects, fields_opt)
    }

    fn insert_or_replace_objects(
        &self,
        objects: Vec<T>,
        fields_opt: Option<Vec<&Field<T>>>,
    ) -> WCDBResult<()> {
        self.table_orm_operation
            .insert_or_replace_objects(objects, fields_opt)
    }

    fn insert_or_ignore_objects(
        &self,
        objects: Vec<T>,
        fields_opt: Option<Vec<&Field<T>>>,
    ) -> WCDBResult<()> {
        self.table_orm_operation
            .insert_or_ignore_objects(objects, fields_opt)
    }

    fn delete_objects(
        &self,
        condition_opt: Option<&Expression>,
        order_opt: Option<&OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()> {
        self.table_orm_operation
            .delete_objects(condition_opt, order_opt, limit_opt, offset_opt)
    }

    fn update_object(
        &self,
        object: T,
        fields_opt: Option<Vec<&Field<T>>>,
        condition_opt: Option<&Expression>,
        order_opt: Option<&OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()> {
        self.table_orm_operation.update_object(
            object,
            fields_opt,
            condition_opt,
            order_opt,
            limit_opt,
            offset_opt,
        )
    }

    fn get_first_object(
        &self,
        fields_opt: Option<Vec<&Field<T>>>,
        condition_opt: Option<&Expression>,
        order_opt: Option<&OrderingTerm>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<Option<T>> {
        self.table_orm_operation
            .get_first_object(fields_opt, condition_opt, order_opt, offset_opt)
    }

    fn get_all_objects(
        &self,
        fields_opt: Option<Vec<&Field<T>>>,
        condition_opt: Option<&Expression>,
        order_opt: Option<&OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<Vec<T>> {
        self.table_orm_operation.get_all_objects(
            fields_opt,
            condition_opt,
            order_opt,
            limit_opt,
            offset_opt,
        )
    }
}

impl<'a, T, R: TableBinding<T>> Table<'a, T, R> {
    pub fn new(table_name: &str, binding: &'a R, database: &'a Database) -> Table<'a, T, R> {
        Table {
            table_orm_operation: TableORMOperation::new(table_name, binding, database),
            _phantom: PhantomData,
        }
    }
}
