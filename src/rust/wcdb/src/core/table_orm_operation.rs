use crate::base::wcdb_exception::WCDBResult;
use crate::chaincall::delete::Delete;
use crate::chaincall::insert::Insert;
use crate::chaincall::select::Select;
use crate::chaincall::update::Update;
use crate::core::database::Database;
use crate::core::table_operation::TableOperation;
use crate::orm::field::Field;
use crate::orm::table_binding::TableBinding;
use crate::winq::expression::Expression;
use crate::winq::ordering_term::OrderingTerm;
use std::marker::PhantomData;

pub struct TableORMOperation<'a, T, R: TableBinding<T>> {
    table_operation: TableOperation<'a>,
    binding: &'a R,
    _phantom: PhantomData<T>,
}

pub trait TableORMOperationTrait<T> {
    fn insert_object(&self, object: T, fields: Vec<&Field<T>>) -> WCDBResult<()>;

    fn insert_or_replace_object(&self, object: T, fields: Vec<&Field<T>>) -> WCDBResult<()>;

    fn insert_or_ignore_object(&self, object: T, fields: Vec<&Field<T>>) -> WCDBResult<()>;

    fn insert_objects(&self, objects: Vec<T>, fields: Vec<&Field<T>>) -> WCDBResult<()>;

    fn insert_or_replace_objects(&self, objects: Vec<T>, fields: Vec<&Field<T>>) -> WCDBResult<()>;

    fn insert_or_ignore_objects(&self, objects: Vec<T>, fields: Vec<&Field<T>>) -> WCDBResult<()>;

    fn prepare_insert(&self) -> Insert<T>;

    fn prepare_update(&self) -> Update<T>;

    fn prepare_select(&self) -> Select<T>;

    fn prepare_delete(&self) -> Delete;

    fn delete_objects(&self) -> WCDBResult<()>;

    fn delete_objects_by_expression(&self, condition: &Expression) -> WCDBResult<()>;

    fn delete_objects_by_expression_order_limit(
        &self,
        condition: &Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()>;

    fn delete_objects_by_expression_order_limit_offset(
        &self,
        condition: &Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()>;

    fn delete_objects_by_order_limit(&self, order: OrderingTerm, limit: i64) -> WCDBResult<()>;

    fn delete_objects_by_order_limit_offset(
        &self,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()>;

    fn update_object_by_field(&self, object: T, field: &Field<T>) -> WCDBResult<()>;

    fn update_object_by_field_expression(
        &self,
        object: T,
        field: &Field<T>,
        condition: &Expression,
    ) -> WCDBResult<()>;

    fn update_object_by_field_expression_order_limit(
        &self,
        object: T,
        field: &Field<T>,
        condition: &Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()>;

    fn update_object_by_field_expression_order_limit_offset(
        &self,
        object: T,
        field: &Field<T>,
        condition: &Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()>;

    fn update_object_by_field_order_limit(
        &self,
        object: T,
        field: &Field<T>,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()>;

    fn update_object_by_field_order_limit_offset(
        &self,
        object: T,
        field: &Field<T>,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()>;

    fn update_object_by_fields(&self, object: T, fields: Vec<&Field<T>>) -> WCDBResult<()>;

    fn update_object_by_fields_expression(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        condition: &Expression,
    ) -> WCDBResult<()>;

    fn update_object_by_fields_expression_order_limit(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        condition: &Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()>;

    fn update_object_by_fields_expression_order_limit_offset(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        condition: &Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()>;

    fn update_object_by_fields_order_limit(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()>;

    fn update_object_by_fields_order_limit_offset(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()>;

    // getFirstObject( binding

    // getFirstObject select(fields)

    fn get_all_objects(&self) -> WCDBResult<Vec<T>>;
    //public <R extends T> List<R> getAllObjects(@NotNull Class<R> cls)
    fn get_all_objects_by_expression(&self, condition: &Expression) -> WCDBResult<Vec<T>>;
    //public <R extends T> List<R> getAllObjects(@Nullable Expression condition, @NotNull Class<R> cls)
    fn get_all_objects_by_expression_order(
        &self,
        condition: &Expression,
        order: OrderingTerm,
    ) -> WCDBResult<Vec<T>>;
    //public <R extends T> List<R> getAllObjects(@Nullable Expression condition, @Nullable OrderingTerm order, @NotNull Class<R> cls)
    fn get_all_objects_by_expression_order_limit(
        &self,
        condition: &Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<Vec<T>>;
    //public <R extends T> List<R> getAllObjects(@Nullable Expression condition, @Nullable OrderingTerm order, long limit, @NotNull Class<R> cls)

    fn get_all_objects_by_expression_order_limit_offset(
        &self,
        condition: &Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<Vec<T>>;
    // public <R extends T> List<R> getAllObjects(@Nullable Expression condition, @Nullable OrderingTerm order, long limit, long offset, @NotNull Class<R> cls)

    fn get_all_objects_order(&self, order: OrderingTerm) -> WCDBResult<Vec<T>>;
    // public <R extends T> List<R> getAllObjects(@Nullable OrderingTerm order, @NotNull Class<R> cls)
    fn get_all_objects_order_limit(&self, order: OrderingTerm, limit: i64) -> WCDBResult<Vec<T>>;
    //public <R extends T> List<R> getAllObjects(@Nullable OrderingTerm order, long limit, @NotNull Class<R> cls)
    fn get_all_objects_order_limit_offset(
        &self,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<Vec<T>>;
    //public <R extends T> List<R> getAllObjects(@Nullable OrderingTerm order, long limit, long offset, @NotNull Class<R> cls)

    fn get_all_objects_by_fields(&self, fields: Vec<&Field<T>>) -> WCDBResult<Vec<T>>;

    //  public <R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @NotNull Class<R> cls) throws WCDBException {

    fn get_all_objects_by_fields_expression(
        &self,
        fields: Vec<&Field<T>>,
        condition: &Expression,
    ) -> WCDBResult<Vec<T>>;

    //public <R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @Nullable Expression condition, @NotNull Class<R> cls)

    fn get_all_objects_by_fields_expression_order(
        &self,
        fields: Vec<&Field<T>>,
        condition: &Expression,
        order: OrderingTerm,
    ) -> WCDBResult<Vec<T>>;

    //  public <R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @Nullable Expression condition, @Nullable OrderingTerm order, @NotNull Class<R> cls)

    fn get_all_objects_by_fields_expression_order_limit(
        &self,
        fields: Vec<&Field<T>>,
        condition: &Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<Vec<T>>;

    // public <R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @Nullable Expression condition, @Nullable OrderingTerm order, long limit, @NotNull Class<R> cls)

    fn get_all_objects_by_fields_expression_order_limit_offset(
        &self,
        fields: Vec<&Field<T>>,
        condition: &Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<Vec<T>>;

    // public <R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @Nullable Expression condition, @Nullable OrderingTerm order, long limit, long offset, @NotNull Class<R> cls)

    fn get_all_objects_by_fields_order(
        &self,
        fields: Vec<&Field<T>>,
        order: OrderingTerm,
    ) -> WCDBResult<Vec<T>>;

    // public <R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @Nullable OrderingTerm order, @NotNull Class<R> cls)

    fn get_all_objects_by_fields_order_limit(
        &self,
        fields: Vec<&Field<T>>,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<Vec<T>>;

    //     fn get_all_objects_by_order_limit(&self, fields: Vec<&Field<T>>, order: OrderingTerm, limit: i64) -> WCDBResult<Vec<T>>;

    fn get_all_objects_by_fields_order_limit_offset(
        &self,
        fields: Vec<&Field<T>>,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<Vec<T>>;

    // public <R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @Nullable OrderingTerm order, long limit, long offset, @NotNull Class<R> cls)

    fn get_first_object(&self, fields: Vec<&Field<T>>) -> WCDBResult<Option<T>>;

    fn get_first_object_by_expression(
        &self,
        fields: Vec<&Field<T>>,
        condition: &Expression,
    ) -> WCDBResult<Option<T>>;
}

impl<'a, T, R: TableBinding<T>> TableORMOperationTrait<T> for TableORMOperation<'a, T, R> {
    fn insert_object(&self, object: T, fields: Vec<&Field<T>>) -> WCDBResult<()> {
        self.prepare_insert()
            .value(object)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    fn insert_or_replace_object(&self, object: T, fields: Vec<&Field<T>>) -> WCDBResult<()> {
        self.prepare_insert()
            .or_replace()
            .value(object)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    fn insert_or_ignore_object(&self, object: T, fields: Vec<&Field<T>>) -> WCDBResult<()> {
        self.prepare_insert()
            .or_ignore()
            .value(object)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    fn insert_objects(&self, objects: Vec<T>, fields: Vec<&Field<T>>) -> WCDBResult<()> {
        self.prepare_insert()
            .values(objects)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    fn insert_or_replace_objects(&self, objects: Vec<T>, fields: Vec<&Field<T>>) -> WCDBResult<()> {
        self.prepare_insert()
            .or_replace()
            .values(objects)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    fn insert_or_ignore_objects(&self, objects: Vec<T>, fields: Vec<&Field<T>>) -> WCDBResult<()> {
        self.prepare_insert()
            .or_ignore()
            .values(objects)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    fn prepare_insert(&self) -> Insert<T> {
        let mut insert = Insert::new(self.table_operation.get_handle(true), false, true);
        insert = insert.into_table(self.table_operation.get_table_name());
        insert
    }

    fn prepare_update(&self) -> Update<T> {
        let mut update = Update::new(self.table_operation.get_handle(true), false, true);
        update = update.table(self.table_operation.get_table_name());
        update
    }

    fn prepare_select(&self) -> Select<T> {
        let mut select = Select::new(self.table_operation.get_handle(false), false, true);
        select = select.from(self.table_operation.get_table_name());
        select
    }

    fn prepare_delete(&self) -> Delete {
        let mut delete = Delete::new(self.table_operation.get_handle(true), false, true);
        delete = delete.from_table(self.table_operation.get_table_name());
        delete
    }

    fn delete_objects(&self) -> WCDBResult<()> {
        self.prepare_delete().execute()?;
        Ok(())
    }

    fn delete_objects_by_expression(&self, condition: &Expression) -> WCDBResult<()> {
        self.prepare_delete()
            .r#where(condition)
            .execute()?;
        Ok(())
    }

    fn delete_objects_by_expression_order_limit(
        &self,
        condition: &Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()> {
        self.prepare_delete()
            .r#where(condition)
            .order_by(&vec![order])
            .limit(limit)
            .execute()?;
        Ok(())
    }

    fn delete_objects_by_expression_order_limit_offset(
        &self,
        condition: &Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()> {
        self.prepare_delete()
            .r#where(condition)
            .order_by(&vec![order])
            .limit(limit)
            .offset(offset)
            .execute()?;
        Ok(())
    }

    fn delete_objects_by_order_limit(&self, order: OrderingTerm, limit: i64) -> WCDBResult<()> {
        self.prepare_delete()
            .order_by(&vec![order])
            .limit(limit)
            .execute()?;
        Ok(())
    }

    fn delete_objects_by_order_limit_offset(
        &self,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()> {
        self.prepare_delete()
            .order_by(&vec![order])
            .limit(limit)
            .offset(offset)
            .execute()?;
        Ok(())
    }

    fn update_object_by_field(&self, object: T, field: &Field<T>) -> WCDBResult<()> {
        self.prepare_update()
            .set(vec![field])
            .to_object(object)
            .execute()?;
        Ok(())
    }

    fn update_object_by_field_expression(
        &self,
        object: T,
        field: &Field<T>,
        condition: &Expression,
    ) -> WCDBResult<()> {
        self.prepare_update()
            .set(vec![field])
            .to_object(object)
            .r#where(condition)
            .execute()?;
        Ok(())
    }

    fn update_object_by_field_expression_order_limit(
        &self,
        object: T,
        field: &Field<T>,
        condition: &Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()> {
        self.prepare_update()
            .set(vec![field])
            .to_object(object)
            .r#where(condition)
            .order_by(&vec![order])
            .limit(limit)
            .execute()?;
        Ok(())
    }

    fn update_object_by_field_expression_order_limit_offset(
        &self,
        object: T,
        field: &Field<T>,
        condition: &Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()> {
        self.prepare_update()
            .set(vec![field])
            .to_object(object)
            .r#where(condition)
            .order_by(&vec![order])
            .limit(limit)
            .offset(offset)
            .execute()?;
        Ok(())
    }

    fn update_object_by_field_order_limit(
        &self,
        object: T,
        field: &Field<T>,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()> {
        self.prepare_update()
            .set(vec![field])
            .to_object(object)
            .order_by(&vec![order])
            .limit(limit)
            .execute()?;
        Ok(())
    }

    fn update_object_by_field_order_limit_offset(
        &self,
        object: T,
        field: &Field<T>,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()> {
        self.prepare_update()
            .set(vec![field])
            .to_object(object)
            .order_by(&vec![order])
            .limit(limit)
            .offset(offset)
            .execute()?;
        Ok(())
    }

    fn update_object_by_fields(&self, object: T, fields: Vec<&Field<T>>) -> WCDBResult<()> {
        self.prepare_update()
            .set(fields)
            .to_object(object)
            .execute()?;
        Ok(())
    }

    fn update_object_by_fields_expression(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        condition: &Expression,
    ) -> WCDBResult<()> {
        self.prepare_update()
            .set(fields)
            .to_object(object)
            .r#where(condition)
            .execute()?;
        Ok(())
    }

    fn update_object_by_fields_expression_order_limit(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        condition: &Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()> {
        self.prepare_update()
            .set(fields)
            .to_object(object)
            .r#where(condition)
            .order_by(&vec![order])
            .limit(limit)
            .execute()?;
        Ok(())
    }

    fn update_object_by_fields_expression_order_limit_offset(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        condition: &Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()> {
        self.prepare_update()
            .set(fields)
            .to_object(object)
            .r#where(condition)
            .order_by(&vec![order])
            .limit(limit)
            .offset(offset)
            .execute()?;
        Ok(())
    }

    fn update_object_by_fields_order_limit(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()> {
        self.prepare_update()
            .set(fields)
            .to_object(object)
            .order_by(&vec![order])
            .limit(limit)
            .execute()?;
        Ok(())
    }

    fn update_object_by_fields_order_limit_offset(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()> {
        self.prepare_update()
            .set(fields)
            .to_object(object)
            .order_by(&vec![order])
            .limit(limit)
            .offset(offset)
            .execute()?;
        Ok(())
    }

    fn get_all_objects(&self) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(self.binding.all_binding_fields())
            .all_objects()
    }

    fn get_all_objects_by_expression(&self, condition: &Expression) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(self.binding.all_binding_fields())
            .r#where(condition)
            .all_objects()
    }

    fn get_all_objects_by_expression_order(
        &self,
        condition: &Expression,
        order: OrderingTerm,
    ) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(self.binding.all_binding_fields())
            .r#where(condition)
            .order_by(order)
            .all_objects()
    }

    fn get_all_objects_by_expression_order_limit(
        &self,
        condition: &Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(self.binding.all_binding_fields())
            .r#where(condition)
            .order_by(order)
            .limit(limit)
            .all_objects()
    }

    fn get_all_objects_by_expression_order_limit_offset(
        &self,
        condition: &Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(self.binding.all_binding_fields())
            .r#where(condition)
            .order_by(order)
            .limit(limit)
            .offset(offset)
            .all_objects()
    }

    fn get_all_objects_order(&self, order: OrderingTerm) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(self.binding.all_binding_fields())
            .order_by(order)
            .all_objects()
    }

    fn get_all_objects_order_limit(&self, order: OrderingTerm, limit: i64) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(self.binding.all_binding_fields())
            .order_by(order)
            .limit(limit)
            .all_objects()
    }

    fn get_all_objects_order_limit_offset(
        &self,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(self.binding.all_binding_fields())
            .order_by(order)
            .limit(limit)
            .offset(offset)
            .all_objects()
    }

    fn get_all_objects_by_fields(&self, fields: Vec<&Field<T>>) -> WCDBResult<Vec<T>> {
        self.prepare_select().select(fields).all_objects()
    }

    fn get_all_objects_by_fields_expression(
        &self,
        fields: Vec<&Field<T>>,
        condition: &Expression,
    ) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(fields)
            .r#where(condition)
            .all_objects()
    }

    fn get_all_objects_by_fields_expression_order(
        &self,
        fields: Vec<&Field<T>>,
        condition: &Expression,
        order: OrderingTerm,
    ) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(fields)
            .r#where(condition)
            .order_by(order)
            .all_objects()
    }

    fn get_all_objects_by_fields_expression_order_limit(
        &self,
        fields: Vec<&Field<T>>,
        condition: &Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(fields)
            .r#where(condition)
            .order_by(order)
            .limit(limit)
            .all_objects()
    }

    fn get_all_objects_by_fields_expression_order_limit_offset(
        &self,
        fields: Vec<&Field<T>>,
        condition: &Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(fields)
            .r#where(condition)
            .order_by(order)
            .limit(limit)
            .offset(offset)
            .all_objects()
    }

    fn get_all_objects_by_fields_order(
        &self,
        fields: Vec<&Field<T>>,
        order: OrderingTerm,
    ) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(fields)
            .order_by(order)
            .all_objects()
    }

    fn get_all_objects_by_fields_order_limit(
        &self,
        fields: Vec<&Field<T>>,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(fields)
            .order_by(order)
            .limit(limit)
            .all_objects()
    }

    fn get_all_objects_by_fields_order_limit_offset(
        &self,
        fields: Vec<&Field<T>>,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<Vec<T>> {
        self.prepare_select()
            .select(fields)
            .order_by(order)
            .limit(limit)
            .offset(offset)
            .all_objects()
    }

    fn get_first_object(&self, fields: Vec<&Field<T>>) -> WCDBResult<Option<T>> {
        self.prepare_select().select(fields).first_object()
    }

    fn get_first_object_by_expression(
        &self,
        fields: Vec<&Field<T>>,
        expression: &Expression,
    ) -> WCDBResult<Option<T>> {
        self.prepare_select()
            .select(fields)
            .r#where(expression)
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
