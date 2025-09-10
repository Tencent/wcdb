use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::wcdb_exception::WCDBResult;
use crate::chaincall::delete::Delete;
use crate::chaincall::insert::Insert;
use crate::chaincall::select::Select;
use crate::chaincall::update::Update;
use crate::core::handle::Handle;
use crate::core::handle_operation::{HandleOperation, HandleOperationTrait};
use crate::orm::field::Field;
use crate::orm::table_binding::TableBinding;
use crate::winq::expression::Expression;
use crate::winq::ordering_term::OrderingTerm;
use crate::winq::statement::StatementTrait;
use crate::winq::statement_drop_table::StatementDropTable;
use std::ffi::c_void;

#[derive(Debug)]
pub struct HandleORMOperation {
    handle_operation: HandleOperation,
}

pub trait HandleORMOperationTrait {
    fn create_table<T, R: TableBinding<T>>(
        &self,
        table_name: &str,
        binding: &R,
    ) -> WCDBResult<bool>;

    fn table_exist(&self, table_name: &str) -> WCDBResult<bool>;

    fn drop_table(&self, table_name: &str) -> WCDBResult<()>;

    fn prepare_insert<T>(&self) -> Insert<T>;

    fn prepare_update<T>(&self) -> Update<T>;

    fn prepare_select<T>(&self) -> Select<T>;

    fn prepare_delete(&self) -> Delete;

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

    fn delete_objects(
        &self,
        table_name: &str,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()>;

    fn update_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()>;

    fn get_first_object<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<Option<T>>;

    fn get_all_objects<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<Vec<T>>;
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

impl HandleORMOperation {
    pub fn new(cpp_obj_opt: Option<*mut c_void>) -> Self {
        HandleORMOperation {
            handle_operation: HandleOperation::new(cpp_obj_opt),
        }
    }

    pub(crate) fn run_transaction<F: FnOnce(&Handle) -> bool>(
        &self,
        handle: Handle,
        auto_invalidate_handle: bool,
        callback: F,
    ) -> WCDBResult<()> {
        self.handle_operation
            .run_transaction(handle, auto_invalidate_handle, callback)
    }

    pub(crate) fn execute<T: StatementTrait>(
        &self,
        handle: Handle,
        auto_invalidate_handle: bool,
        statement: &T,
    ) -> WCDBResult<()> {
        self.handle_operation
            .execute(handle, auto_invalidate_handle, statement)
    }

    pub(crate) fn execute_sql(
        &self,
        handle: Handle,
        auto_invalidate_handle: bool,
        sql: &str,
    ) -> WCDBResult<()> {
        self.handle_operation
            .execute_sql(handle, auto_invalidate_handle, sql)
    }

    pub(crate) fn create_table<T, R: TableBinding<T>>(
        &self,
        handle: Handle,
        table_name: &str,
        binding: &R,
    ) -> WCDBResult<bool> {
        binding.base_binding().create_table(table_name, handle)
    }

    pub(crate) fn table_exist(
        &self,
        handle: Handle,
        auto_invalidate_handle: bool,
        table_name: &str,
    ) -> WCDBResult<bool> {
        let mut handle = handle;
        let ret = Handle::table_exist(handle.get_cpp_handle()?, table_name);
        let mut exception_opt = None;
        if ret > 1 {
            exception_opt = Some(handle.create_exception());
        }
        if auto_invalidate_handle {
            handle.invalidate();
        }
        if exception_opt.is_some() {
            match exception_opt {
                None => {}
                Some(ex) => {
                    return Err(ex);
                }
            }
        }
        Ok(ret == 1)
    }

    pub(crate) fn drop_table(
        &self,
        handle: Handle,
        auto_invalidate_handle: bool,
        table_name: &str,
    ) -> WCDBResult<()> {
        let statement = StatementDropTable::new();
        self.handle_operation.execute(
            handle,
            auto_invalidate_handle,
            statement.drop_table(table_name).if_exist(),
        )
    }

    pub(crate) fn prepare_insert<'a, T>(
        &self,
        handle: Handle<'a>,
        auto_invalidate_handle: bool,
    ) -> Insert<'a, T> {
        Insert::new(handle, false, auto_invalidate_handle)
    }

    pub(crate) fn prepare_update<'a, T>(
        &self,
        handle: Handle<'a>,
        auto_invalidate_handle: bool,
    ) -> Update<'a, T> {
        Update::new(handle, false, auto_invalidate_handle)
    }

    pub(crate) fn prepare_select<'a, T>(
        &self,
        handle: Handle<'a>,
        auto_invalidate_handle: bool,
    ) -> Select<'a, T> {
        Select::new(handle, false, auto_invalidate_handle)
    }

    pub(crate) fn prepare_delete<'a>(
        &self,
        handle: Handle<'a>,
        auto_invalidate_handle: bool,
    ) -> Delete<'a> {
        Delete::new(handle, false, auto_invalidate_handle)
    }

    pub(crate) fn insert_object<T>(
        &self,
        handle: Handle,
        auto_invalidate_handle: bool,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.prepare_insert::<T>(handle, auto_invalidate_handle)
            .into_table(table_name)
            .value(object)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    pub(crate) fn insert_or_replace_object<T>(
        &self,
        handle: Handle,
        auto_invalidate_handle: bool,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.prepare_insert::<T>(handle, auto_invalidate_handle)
            .or_replace()
            .into_table(table_name)
            .value(object)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    pub(crate) fn insert_or_ignore_object<T>(
        &self,
        handle: Handle,
        auto_invalidate_handle: bool,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.prepare_insert::<T>(handle, auto_invalidate_handle)
            .or_ignore()
            .into_table(table_name)
            .value(object)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    pub(crate) fn insert_objects<T>(
        &self,
        handle: Handle,
        auto_invalidate_handle: bool,
        objects: Vec<T>,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.prepare_insert::<T>(handle, auto_invalidate_handle)
            .into_table(table_name)
            .values(objects)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    pub(crate) fn insert_or_replace_objects<T>(
        &self,
        handle: Handle,
        auto_invalidate_handle: bool,
        objects: Vec<T>,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.prepare_insert::<T>(handle, auto_invalidate_handle)
            .or_replace()
            .into_table(table_name)
            .values(objects)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    pub(crate) fn insert_or_ignore_objects<T>(
        &self,
        handle: Handle,
        auto_invalidate_handle: bool,
        objects: Vec<T>,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.prepare_insert::<T>(handle, auto_invalidate_handle)
            .or_ignore()
            .into_table(table_name)
            .values(objects)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    pub(crate) fn delete_objects(
        &self,
        handle: Handle,
        auto_invalidate_handle: bool,
        table_name: &str,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()> {
        let delete = self.prepare_delete(handle, auto_invalidate_handle);
        delete.from_table(table_name);
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

    pub(crate) fn update_object<T>(
        &self,
        handle: Handle,
        auto_invalidate_handle: bool,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()> {
        let update = self.prepare_update::<T>(handle, auto_invalidate_handle);
        update.table(table_name);
        update.set(fields);
        update.to_object(object);
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
        update.execute()?;
        Ok(())
    }

    pub(crate) fn get_first_object<T>(
        &self,
        handle: Handle,
        auto_invalidate_handle: bool,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<Option<T>> {
        let select = self.prepare_select::<T>(handle, auto_invalidate_handle);
        select.select(fields);
        select.from(table_name);
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

    pub(crate) fn get_all_objects<T>(
        &self,
        handle: Handle,
        auto_invalidate_handle: bool,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<Vec<T>> {
        let select = self.prepare_select::<T>(handle, auto_invalidate_handle);
        select.select(fields);
        select.from(table_name);
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
