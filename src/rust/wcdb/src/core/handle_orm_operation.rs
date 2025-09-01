use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::wcdb_exception::WCDBResult;
use crate::chaincall::delete::Delete;
use crate::chaincall::insert::Insert;
use crate::chaincall::select::Select;
use crate::chaincall::update::Update;
use crate::core::handle_operation::{HandleOperation, HandleOperationTrait};
use crate::orm::field::Field;
use crate::orm::table_binding::TableBinding;
use crate::winq::expression::Expression;
use crate::winq::ordering_term::OrderingTerm;
use std::ffi::c_void;
use crate::core::handle::Handle;

#[derive(Debug, Clone)]
pub struct HandleORMOperation {
    handle_operation: HandleOperation,
}

pub trait HandleORMOperationTrait: HandleOperationTrait {
    fn create_table<T, R: TableBinding<T>>(
        &self,
        table_name: &str,
        binding: &R,
    ) -> WCDBResult<bool>;

    fn table_exist(&self, table_name: &str) -> WCDBResult<bool>;

    fn drop_table(&self, table_name: &str) -> WCDBResult<()>;

    fn insert_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()>;

    fn insert_or_replace_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()>;

    fn insert_or_ignore_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()>;

    fn insert_objects<T>(
        &self,
        objects: Vec<T>,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()>;

    fn insert_or_replace_objects<T>(
        &self,
        objects: Vec<T>,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()>;

    fn insert_or_ignore_objects<T>(
        &self,
        objects: Vec<T>,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()>;

    fn prepare_insert<T>(&self) -> Insert<T>;

    fn prepare_update<T>(&self) -> Update<T>;

    fn prepare_select<T>(&self) -> Select<T>;

    fn prepare_delete(&self) -> Delete;

    fn delete_objects(&self, table_name: &str) -> WCDBResult<()>;

    fn delete_objects_by_expression(
        &self,
        table_name: &str,
        expression: &Expression,
    ) -> WCDBResult<()>;

    fn delete_objects_by_expression_order_limit(
        &self,
        table_name: &str,
        expression: &Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()>;

    fn delete_objects_by_expression_order_limit_offset(
        &self,
        table_name: &str,
        expression: &Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()>;

    fn delete_objects_by_order_limit(
        &self,
        table_name: &str,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()>;

    fn delete_objects_by_order_limit_offset(
        &self,
        table_name: &str,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()>;

    fn update_object_by_field<T>(
        &self,
        object: T,
        field: &Field<T>,
        table_name: &str,
    ) -> WCDBResult<()>;

    fn update_object_by_field_expression<T>(
        &self,
        object: T,
        field: &Field<T>,
        table_name: &str,
        expression: &Expression,
    ) -> WCDBResult<()>;

    fn update_object_by_field_expression_order_limit<T>(
        &self,
        object: T,
        field: &Field<T>,
        table_name: &str,
        expression: &Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()>;

    fn update_object_by_field_expression_order_limit_offset<T>(
        &self,
        object: T,
        field: &Field<T>,
        table_name: &str,
        expression: &Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()>;

    fn update_object_by_field_order_limit<T>(
        &self,
        object: T,
        field: &Field<T>,
        table_name: &str,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()>;

    fn update_object_by_field_order_limit_offset<T>(
        &self,
        object: T,
        field: &Field<T>,
        table_name: &str,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()>;

    fn update_object_by_fields<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()>;

    fn update_object_by_fields_expression<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
        expression: &Expression,
    ) -> WCDBResult<()>;

    fn update_object_by_fields_expression_order_limit<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
        expression: &Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()>;

    fn update_object_by_fields_expression_order_limit_offset<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
        expression: &Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()>;

    fn update_object_by_fields_order_limit<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<()>;

    fn update_object_by_fields_order_limit_offset<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<()>;

    fn get_first_object<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<Option<T>>;

    fn get_first_object_by_expression<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        expression: &Expression,
    ) -> WCDBResult<Option<T>>;

    // todo dengxudong
    // public <T, R extends T> R getFirstObject(@NotNull Field<T>[] fields, @NotNull String tableName, @NotNull Class<R> cls)

    fn get_first_object_by_table_name_expression<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition: &Expression,
    ) -> WCDBResult<Option<T>>;

    // public <T, R extends T> R getFirstObject(@NotNull Field<T>[] fields, @NotNull String tableName, @Nullable Expression condition, @NotNull Class<R> cls)

    fn get_first_object_by_table_name_expression_order<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition: &Expression,
        order: OrderingTerm,
    ) -> WCDBResult<Option<T>>;

    //public <T, R extends T> R getFirstObject(@NotNull Field<T>[] fields, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, @NotNull Class<R> cls)

    fn get_first_object_by_table_name_expression_order_offset<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition: &Expression,
        order: OrderingTerm,
        offset: i64,
    ) -> WCDBResult<Option<T>>;

    //public <T, R extends T> R getFirstObject(@NotNull Field<T>[] fields, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, long offset, @NotNull Class<R> cls)

    fn get_first_object_by_table_name_order<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        order: OrderingTerm,
    ) -> WCDBResult<Option<T>>;

    //public <T, R extends T> R getFirstObject(@NotNull Field<T>[] fields, @NotNull String tableName, @Nullable OrderingTerm order, @NotNull Class<R> cls)

    fn get_first_object_by_table_name_order_offset<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        order: OrderingTerm,
        offset: i64,
    ) -> WCDBResult<Option<T>>;

    // public <T, R extends T> R getFirstObject(@NotNull Field<T>[] fields, @NotNull String tableName, @Nullable OrderingTerm order, long offset, @NotNull Class<R> cls)

    fn get_all_objects<T>(&self, fields: Vec<&Field<T>>, table_name: &str) -> WCDBResult<Vec<T>>;

    //public <T, R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @NotNull String tableName, @NotNull Class<R> cls)

    fn get_all_objects_by_table_name_expression<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition: &Expression,
    ) -> WCDBResult<Vec<T>>;

    //public <T, R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @NotNull String tableName, @Nullable Expression condition, @NotNull Class<R> cls)

    fn get_all_objects_by_table_name_expression_order<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition: &Expression,
        order: OrderingTerm,
    ) -> WCDBResult<Vec<T>>;

    //public <T, R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, @NotNull Class<R> cls)

    fn get_all_objects_by_table_name_expression_order_limit<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition: &Expression,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<Vec<T>>;

    //public <T, R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, long limit, @NotNull Class<R> cls)

    fn get_all_objects_by_table_name_expression_order_limit_offset<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition: &Expression,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<Vec<T>>;

    //public <T, R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @NotNull String tableName, @Nullable Expression condition, @Nullable OrderingTerm order, long limit, long offset, @NotNull Class<R> cls)

    fn get_all_objects_by_table_name_order<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        order: OrderingTerm,
    ) -> WCDBResult<Vec<T>>;

    //public <T, R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @NotNull String tableName, @Nullable OrderingTerm order, @NotNull Class<R> cls)

    fn get_all_objects_by_table_name_order_limit<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        order: OrderingTerm,
        limit: i64,
    ) -> WCDBResult<Vec<T>>;

    //public <T, R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @NotNull String tableName, @Nullable OrderingTerm order, long limit, @NotNull Class<R> cls)

    fn get_all_objects_by_table_name_order_limit_offset<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        order: OrderingTerm,
        limit: i64,
        offset: i64,
    ) -> WCDBResult<Vec<T>>;

    //public <T, R extends T> List<R> getAllObjects(@NotNull Field<T>[] fields, @NotNull String tableName, @Nullable OrderingTerm order, long limit, long offset, @NotNull Class<R> cls)
}

impl CppObjectTrait for HandleORMOperation {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.handle_operation.set_cpp_obj(cpp_obj)
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.handle_operation.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.handle_operation.release_cpp_object();
    }
}

impl CppObjectConvertibleTrait for HandleORMOperation {
    fn as_cpp_object(&self) -> &CppObject {
        self.handle_operation.as_cpp_object()
    }
}

impl HandleOperationTrait for HandleORMOperation {
    fn get_handle(&self, write_hint: bool) -> Handle {
        todo!()
    }

    fn auto_invalidate_handle(&self) -> bool {
        todo!()
    }

    fn run_transaction<F: FnOnce(Handle) -> bool>(&self, callback: F) -> WCDBResult<()> {
        todo!()
    }
}

impl HandleORMOperationTrait for HandleORMOperation {
    fn create_table<T, R: TableBinding<T>>(&self, table_name: &str, binding: &R) -> WCDBResult<bool> {
        todo!()
    }

    fn table_exist(&self, table_name: &str) -> WCDBResult<bool> {
        todo!()
    }

    fn drop_table(&self, table_name: &str) -> WCDBResult<()> {
        todo!()
    }

    fn insert_object<T>(&self, object: T, fields: Vec<&Field<T>>, table_name: &str) -> WCDBResult<()> {
        todo!()
    }

    fn insert_or_replace_object<T>(&self, object: T, fields: Vec<&Field<T>>, table_name: &str) -> WCDBResult<()> {
        todo!()
    }

    fn insert_or_ignore_object<T>(&self, object: T, fields: Vec<&Field<T>>, table_name: &str) -> WCDBResult<()> {
        todo!()
    }

    fn insert_objects<T>(&self, objects: Vec<T>, fields: Vec<&Field<T>>, table_name: &str) -> WCDBResult<()> {
        todo!()
    }

    fn insert_or_replace_objects<T>(&self, objects: Vec<T>, fields: Vec<&Field<T>>, table_name: &str) -> WCDBResult<()> {
        todo!()
    }

    fn insert_or_ignore_objects<T>(&self, objects: Vec<T>, fields: Vec<&Field<T>>, table_name: &str) -> WCDBResult<()> {
        todo!()
    }

    fn prepare_insert<T>(&self) -> Insert<T> {
        todo!()
    }

    fn prepare_update<T>(&self) -> Update<T> {
        todo!()
    }

    fn prepare_select<T>(&self) -> Select<T> {
        todo!()
    }

    fn prepare_delete(&self) -> Delete {
        todo!()
    }

    fn delete_objects(&self, table_name: &str) -> WCDBResult<()> {
        todo!()
    }

    fn delete_objects_by_expression(&self, table_name: &str, expression: &Expression) -> WCDBResult<()> {
        todo!()
    }

    fn delete_objects_by_expression_order_limit(&self, table_name: &str, expression: &Expression, order: OrderingTerm, limit: i64) -> WCDBResult<()> {
        todo!()
    }

    fn delete_objects_by_expression_order_limit_offset(&self, table_name: &str, expression: &Expression, order: OrderingTerm, limit: i64, offset: i64) -> WCDBResult<()> {
        todo!()
    }

    fn delete_objects_by_order_limit(&self, table_name: &str, order: OrderingTerm, limit: i64) -> WCDBResult<()> {
        todo!()
    }

    fn delete_objects_by_order_limit_offset(&self, table_name: &str, order: OrderingTerm, limit: i64, offset: i64) -> WCDBResult<()> {
        todo!()
    }

    fn update_object_by_field<T>(&self, object: T, field: &Field<T>, table_name: &str) -> WCDBResult<()> {
        todo!()
    }

    fn update_object_by_field_expression<T>(&self, object: T, field: &Field<T>, table_name: &str, expression: &Expression) -> WCDBResult<()> {
        todo!()
    }

    fn update_object_by_field_expression_order_limit<T>(&self, object: T, field: &Field<T>, table_name: &str, expression: &Expression, order: OrderingTerm, limit: i64) -> WCDBResult<()> {
        todo!()
    }

    fn update_object_by_field_expression_order_limit_offset<T>(&self, object: T, field: &Field<T>, table_name: &str, expression: &Expression, order: OrderingTerm, limit: i64, offset: i64) -> WCDBResult<()> {
        todo!()
    }

    fn update_object_by_field_order_limit<T>(&self, object: T, field: &Field<T>, table_name: &str, order: OrderingTerm, limit: i64) -> WCDBResult<()> {
        todo!()
    }

    fn update_object_by_field_order_limit_offset<T>(&self, object: T, field: &Field<T>, table_name: &str, order: OrderingTerm, limit: i64, offset: i64) -> WCDBResult<()> {
        todo!()
    }

    fn update_object_by_fields<T>(&self, object: T, fields: Vec<&Field<T>>, table_name: &str) -> WCDBResult<()> {
        todo!()
    }

    fn update_object_by_fields_expression<T>(&self, object: T, fields: Vec<&Field<T>>, table_name: &str, expression: &Expression) -> WCDBResult<()> {
        todo!()
    }

    fn update_object_by_fields_expression_order_limit<T>(&self, object: T, fields: Vec<&Field<T>>, table_name: &str, expression: &Expression, order: OrderingTerm, limit: i64) -> WCDBResult<()> {
        todo!()
    }

    fn update_object_by_fields_expression_order_limit_offset<T>(&self, object: T, fields: Vec<&Field<T>>, table_name: &str, expression: &Expression, order: OrderingTerm, limit: i64, offset: i64) -> WCDBResult<()> {
        todo!()
    }

    fn update_object_by_fields_order_limit<T>(&self, object: T, fields: Vec<&Field<T>>, table_name: &str, order: OrderingTerm, limit: i64) -> WCDBResult<()> {
        todo!()
    }

    fn update_object_by_fields_order_limit_offset<T>(&self, object: T, fields: Vec<&Field<T>>, table_name: &str, order: OrderingTerm, limit: i64, offset: i64) -> WCDBResult<()> {
        todo!()
    }

    fn get_first_object<T>(&self, fields: Vec<&Field<T>>, table_name: &str, condition_opt: Option<Expression>, order_opt: Option<OrderingTerm>, offset_opt: Option<i64>) -> WCDBResult<Option<T>> {
        self.prepare_select()
    }

    fn get_first_object_by_expression<T>(&self, fields: Vec<&Field<T>>, table_name: &str, expression: &Expression) -> WCDBResult<Option<T>> {
        todo!()
    }

    fn get_first_object_by_table_name_expression<T>(&self, fields: Vec<&Field<T>>, table_name: &str, condition: &Expression) -> WCDBResult<Option<T>> {
        todo!()
    }

    fn get_first_object_by_table_name_expression_order<T>(&self, fields: Vec<&Field<T>>, table_name: &str, condition: &Expression, order: OrderingTerm) -> WCDBResult<Option<T>> {
        todo!()
    }

    fn get_first_object_by_table_name_expression_order_offset<T>(&self, fields: Vec<&Field<T>>, table_name: &str, condition: &Expression, order: OrderingTerm, offset: i64) -> WCDBResult<Option<T>> {
        todo!()
    }

    fn get_first_object_by_table_name_order<T>(&self, fields: Vec<&Field<T>>, table_name: &str, order: OrderingTerm) -> WCDBResult<Option<T>> {
        todo!()
    }

    fn get_first_object_by_table_name_order_offset<T>(&self, fields: Vec<&Field<T>>, table_name: &str, order: OrderingTerm, offset: i64) -> WCDBResult<Option<T>> {
        todo!()
    }

    fn get_all_objects<T>(&self, fields: Vec<&Field<T>>, table_name: &str) -> WCDBResult<Vec<T>> {
        todo!()
    }

    fn get_all_objects_by_table_name_expression<T>(&self, fields: Vec<&Field<T>>, table_name: &str, condition: &Expression) -> WCDBResult<Vec<T>> {
        todo!()
    }

    fn get_all_objects_by_table_name_expression_order<T>(&self, fields: Vec<&Field<T>>, table_name: &str, condition: &Expression, order: OrderingTerm) -> WCDBResult<Vec<T>> {
        todo!()
    }

    fn get_all_objects_by_table_name_expression_order_limit<T>(&self, fields: Vec<&Field<T>>, table_name: &str, condition: &Expression, order: OrderingTerm, limit: i64) -> WCDBResult<Vec<T>> {
        todo!()
    }

    fn get_all_objects_by_table_name_expression_order_limit_offset<T>(&self, fields: Vec<&Field<T>>, table_name: &str, condition: &Expression, order: OrderingTerm, limit: i64, offset: i64) -> WCDBResult<Vec<T>> {
        todo!()
    }

    fn get_all_objects_by_table_name_order<T>(&self, fields: Vec<&Field<T>>, table_name: &str, order: OrderingTerm) -> WCDBResult<Vec<T>> {
        todo!()
    }

    fn get_all_objects_by_table_name_order_limit<T>(&self, fields: Vec<&Field<T>>, table_name: &str, order: OrderingTerm, limit: i64) -> WCDBResult<Vec<T>> {
        todo!()
    }

    fn get_all_objects_by_table_name_order_limit_offset<T>(&self, fields: Vec<&Field<T>>, table_name: &str, order: OrderingTerm, limit: i64, offset: i64) -> WCDBResult<Vec<T>> {
        todo!()
    }
}

impl HandleORMOperation {
    pub fn new(cpp_obj_opt: Option<*mut c_void>) -> Self {
        HandleORMOperation {
            handle_operation: HandleOperation::new(cpp_obj_opt),
        }
    }
}
