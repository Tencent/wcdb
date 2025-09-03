use crate::base::basic_types::WCDBBasicTypes;
use crate::base::value::Value;
use crate::base::wcdb_exception::WCDBResult;
use crate::chaincall::delete::Delete;
use crate::chaincall::insert::Insert;
use crate::chaincall::select::Select;
use crate::chaincall::update::Update;
use crate::core::database::Database;
use crate::core::table_operation::{TableOperation, TableOperationTrait};
use crate::orm::field::Field;
use crate::orm::table_binding::TableBinding;
use crate::winq::column::Column;
use crate::winq::expression::Expression;
use crate::winq::ordering_term::OrderingTerm;
use std::marker::PhantomData;

pub struct TableORMOperation<'a, T, R: TableBinding<T>> {
    table_operation: TableOperation<'a>,
    binding: &'a R,
    _phantom: PhantomData<T>,
}

pub trait TableORMOperationTrait<'a, T, R: TableBinding<T>>: TableOperationTrait {
    fn get_binding(&self) -> &'a R;

    fn prepare_insert(&self) -> Insert<T>;

    fn prepare_update(&self) -> Update<T>;

    fn prepare_select(&self) -> Select<T>;

    fn prepare_delete(&self) -> Delete;

    fn insert_object(&self, object: T, fields_opt: Option<Vec<&Field<T>>>) -> WCDBResult<()>;

    fn insert_or_replace_object(
        &self,
        object: T,
        fields_opt: Option<Vec<&Field<T>>>,
    ) -> WCDBResult<()>;

    fn insert_or_ignore_object(
        &self,
        object: T,
        fields_opt: Option<Vec<&Field<T>>>,
    ) -> WCDBResult<()>;

    fn insert_objects(&self, objects: Vec<T>, fields_opt: Option<Vec<&Field<T>>>)
        -> WCDBResult<()>;

    fn insert_or_replace_objects(
        &self,
        objects: Vec<T>,
        fields_opt: Option<Vec<&Field<T>>>,
    ) -> WCDBResult<()>;

    fn insert_or_ignore_objects(
        &self,
        objects: Vec<T>,
        fields_opt: Option<Vec<&Field<T>>>,
    ) -> WCDBResult<()>;

    fn delete_objects(
        &self,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()>;

    fn update_object(
        &self,
        object: T,
        fields_opt: Option<Vec<&Field<T>>>,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()>;

    fn get_first_object(
        &self,
        fields_opt: Option<Vec<&Field<T>>>,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<Option<T>>;

    fn get_all_objects(
        &self,
        fields_opt: Option<Vec<&Field<T>>>,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<Vec<T>>;
}

impl<'a, T, R: TableBinding<T>> TableOperationTrait for TableORMOperation<'a, T, R> {
    fn get_table_name(&self) -> &str {
        self.table_operation.get_table_name()
    }

    fn get_database(&self) -> &Database {
        self.table_operation.get_database()
    }

    fn insert_rows(&self, rows: Vec<Vec<Value>>, columns: Vec<Column>) -> WCDBResult<()> {
        self.table_operation.insert_rows(rows, columns)
    }

    fn insert_or_replace_rows(
        &self,
        rows: Vec<Vec<Value>>,
        columns: Vec<Column>,
    ) -> WCDBResult<()> {
        self.table_operation.insert_or_replace_rows(rows, columns)
    }

    fn insert_or_ignore_rows(&self, rows: Vec<Vec<Value>>, columns: Vec<Column>) -> WCDBResult<()> {
        self.table_operation.insert_or_ignore_rows(rows, columns)
    }

    fn update_value<V: WCDBBasicTypes>(
        &self,
        value: &V,
        column: Column,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()> {
        self.table_operation.update_value(
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
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()> {
        self.table_operation.update_row(
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
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()> {
        self.table_operation
            .delete_value(condition_opt, order_opt, limit_opt, offset_opt)
    }

    fn get_values(
        &self,
        columns: Vec<&Column>,
        condition_opt: Option<Expression>,
        order_opt: Option<Vec<OrderingTerm>>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<Vec<Vec<Value>>> {
        self.table_operation
            .get_values(columns, condition_opt, order_opt, limit_opt, offset_opt)
    }
}

impl<'a, T, R: TableBinding<T>> TableORMOperationTrait<'a, T, R> for TableORMOperation<'a, T, R> {
    fn get_binding(&self) -> &'a R {
        self.binding
    }

    fn prepare_insert(&self) -> Insert<T> {
        let insert = Insert::new(self.table_operation.get_handle(true), false, true);
        insert.into_table(self.table_operation.get_table_name());
        insert
    }

    fn prepare_update(&self) -> Update<T> {
        let update = Update::new(self.table_operation.get_handle(true), false, true);
        update.table(self.table_operation.get_table_name());
        update
    }

    fn prepare_select(&self) -> Select<T> {
        let select = Select::new(self.table_operation.get_handle(false), false, true);
        select.from(self.table_operation.get_table_name());
        select
    }

    fn prepare_delete(&self) -> Delete {
        let delete = Delete::new(self.table_operation.get_handle(true), false, true);
        delete.from_table(self.table_operation.get_table_name());
        delete
    }

    fn insert_object(&self, object: T, fields_opt: Option<Vec<&Field<T>>>) -> WCDBResult<()> {
        let insert = self.prepare_insert();
        insert.value(object);
        if let Some(fields) = fields_opt {
            insert.on_fields(fields);
        }
        insert.execute()?;
        Ok(())
    }

    fn insert_or_replace_object(
        &self,
        object: T,
        fields_opt: Option<Vec<&Field<T>>>,
    ) -> WCDBResult<()> {
        let insert = self.prepare_insert();
        insert.value(object).or_replace();
        if let Some(fields) = fields_opt {
            insert.on_fields(fields);
        }
        insert.execute()?;
        Ok(())
    }

    fn insert_or_ignore_object(
        &self,
        object: T,
        fields_opt: Option<Vec<&Field<T>>>,
    ) -> WCDBResult<()> {
        let insert = self.prepare_insert();
        insert.value(object).or_ignore();
        if let Some(fields) = fields_opt {
            insert.on_fields(fields);
        }
        insert.execute()?;
        Ok(())
    }

    fn insert_objects(
        &self,
        objects: Vec<T>,
        fields_opt: Option<Vec<&Field<T>>>,
    ) -> WCDBResult<()> {
        let insert = self.prepare_insert();
        insert.values(objects);
        if let Some(fields) = fields_opt {
            insert.on_fields(fields);
        }
        insert.execute()?;
        Ok(())
    }

    fn insert_or_replace_objects(
        &self,
        objects: Vec<T>,
        fields_opt: Option<Vec<&Field<T>>>,
    ) -> WCDBResult<()> {
        let insert = self.prepare_insert();
        insert.values(objects).or_replace();
        if let Some(fields) = fields_opt {
            insert.on_fields(fields);
        }
        insert.execute()?;
        Ok(())
    }

    fn insert_or_ignore_objects(
        &self,
        objects: Vec<T>,
        fields_opt: Option<Vec<&Field<T>>>,
    ) -> WCDBResult<()> {
        let insert = self.prepare_insert();
        insert.values(objects).or_ignore();
        if let Some(fields) = fields_opt {
            insert.on_fields(fields);
        }
        insert.execute()?;
        Ok(())
    }

    fn delete_objects(
        &self,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()> {
        let delete = self.prepare_delete();
        if let Some(condition) = condition_opt {
            delete.r#where(&condition);
        }
        if let Some(order) = order_opt {
            delete.order_by(&vec![order]);
        }
        if let Some(limit) = limit_opt {
            delete.limit(limit);
        }
        if let Some(offset) = offset_opt {
            delete.offset(offset);
        }
        delete.execute()?;
        Ok(())
    }

    fn update_object(
        &self,
        object: T,
        fields_opt: Option<Vec<&Field<T>>>,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()> {
        let update = self.prepare_update();
        if let Some(fields) = fields_opt {
            update.set(fields);
        }
        if let Some(condition) = condition_opt {
            update.r#where(&condition);
        }
        if let Some(order) = order_opt {
            update.order_by(&vec![order]);
        }
        if let Some(limit) = limit_opt {
            update.limit(limit);
        }
        if let Some(offset) = offset_opt {
            update.offset(offset);
        }
        update.to_object(object);
        update.execute()?;
        Ok(())
    }

    fn get_first_object(
        &self,
        fields_opt: Option<Vec<&Field<T>>>,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<Option<T>> {
        let select = self.prepare_select();
        if let Some(fields) = fields_opt {
            select.select(fields);
        } else {
            select.select(self.binding.all_binding_fields());
        }
        if let Some(condition) = condition_opt {
            select.r#where(&condition);
        }
        if let Some(order) = order_opt {
            select.order_by(&vec![order]);
        }
        select.limit(1);
        if let Some(offset) = offset_opt {
            select.offset(offset);
        }
        select.first_object()
    }

    fn get_all_objects(
        &self,
        fields_opt: Option<Vec<&Field<T>>>,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<Vec<T>> {
        let select = self.prepare_select();
        if let Some(fields) = fields_opt {
            select.select(fields);
        } else {
            select.select(self.binding.all_binding_fields());
        }
        if let Some(condition) = condition_opt {
            select.r#where(&condition);
        }
        if let Some(order) = order_opt {
            select.order_by(&vec![order]);
        }
        if let Some(limit) = limit_opt {
            select.limit(limit);
        }
        if let Some(offset) = offset_opt {
            select.offset(offset);
        }
        select.all_objects()
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
