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

impl HandleOperationTrait for HandleORMOperation {
    fn get_handle(&self, write_hint: bool) -> Handle {
        unimplemented!("Stub: This method should be implemented by subclasses")
    }

    fn auto_invalidate_handle(&self) -> bool {
        unimplemented!("Stub: This method should be implemented by subclasses")
    }

    fn run_transaction<F: FnOnce(Handle) -> bool>(&self, callback: F) -> WCDBResult<()> {
        unimplemented!("Stub: This method should be implemented by subclasses")
    }

    fn execute<T: StatementTrait>(&self, statement: &T) -> WCDBResult<()> {
        self.handle_operation.execute(statement)
    }

    fn execute_sql(&self, sql: &str) -> WCDBResult<()> {
        self.handle_operation.execute_sql(sql)
    }
}

impl HandleORMOperationTrait for HandleORMOperation {
    fn create_table<T, R: TableBinding<T>>(
        &self,
        table_name: &str,
        binding: &R,
    ) -> WCDBResult<bool> {
        let handle = self.get_handle(true);
        binding.base_binding().create_table(table_name, handle)
    }

    fn table_exist(&self, table_name: &str) -> WCDBResult<bool> {
        let handle = self.get_handle(false);
        let ret = Handle::table_exist(handle.get_cpp_handle()?, table_name);
        let mut exception_opt = None;
        if ret > 1 {
            exception_opt = Some(handle.create_exception());
        }
        if self.auto_invalidate_handle() {
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

    fn drop_table(&self, table_name: &str) -> WCDBResult<()> {
        let statement = StatementDropTable::new();
        self.execute(statement.drop_table(table_name).if_exist())
    }

    fn prepare_insert<T>(&self) -> Insert<T> {
        Insert::new(self.get_handle(true), false, self.auto_invalidate_handle())
    }

    fn prepare_update<T>(&self) -> Update<T> {
        Update::new(self.get_handle(true), false, self.auto_invalidate_handle())
    }

    fn prepare_select<T>(&self) -> Select<T> {
        Select::new(self.get_handle(false), false, self.auto_invalidate_handle())
    }

    fn prepare_delete(&self) -> Delete {
        Delete::new(self.get_handle(true), false, self.auto_invalidate_handle())
    }

    fn insert_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.prepare_insert::<T>()
            .into_table(table_name)
            .value(object)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    fn insert_or_replace_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.prepare_insert::<T>()
            .or_replace()
            .into_table(table_name)
            .value(object)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    fn insert_or_ignore_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.prepare_insert::<T>()
            .or_ignore()
            .into_table(table_name)
            .value(object)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    fn insert_objects<T>(
        &self,
        objects: Vec<T>,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.prepare_insert::<T>()
            .into_table(table_name)
            .values(objects)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    fn insert_or_replace_objects<T>(
        &self,
        objects: Vec<T>,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.prepare_insert::<T>()
            .or_replace()
            .into_table(table_name)
            .values(objects)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    fn insert_or_ignore_objects<T>(
        &self,
        objects: Vec<T>,
        fields: Vec<&Field<T>>,
        table_name: &str,
    ) -> WCDBResult<()> {
        self.prepare_insert::<T>()
            .or_ignore()
            .into_table(table_name)
            .values(objects)
            .on_fields(fields)
            .execute()?;
        Ok(())
    }

    fn delete_objects(
        &self,
        table_name: &str,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()> {
        let delete = self.prepare_delete();
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

    fn update_object<T>(
        &self,
        object: T,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()> {
        let update = self.prepare_update::<T>();
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

    fn get_first_object<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<Option<T>> {
        let select = self.prepare_select::<T>();
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

    fn get_all_objects<T>(
        &self,
        fields: Vec<&Field<T>>,
        table_name: &str,
        condition_opt: Option<Expression>,
        order_opt: Option<OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<Vec<T>> {
        let select = self.prepare_select::<T>();
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

impl HandleORMOperation {
    pub fn new(cpp_obj_opt: Option<*mut c_void>) -> Self {
        HandleORMOperation {
            handle_operation: HandleOperation::new(cpp_obj_opt),
        }
    }
}
