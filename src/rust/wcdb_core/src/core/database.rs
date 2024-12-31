use crate::base::cpp_object::CppObjectTrait;
use crate::base::wcdb_exception::{ExceptionInner, WCDBException};
use crate::chaincall::insert::Insert;
use crate::core::handle::Handle;
use crate::core::handle_operation::HandleOperationTrait;
use crate::core::handle_orm_operation::{HandleORMOperation, HandleORMOperationTrait};
use crate::core::table::Table;
use crate::orm::field::Field;
use crate::orm::table_binding::TableBinding;
use crate::utils::ToCow;
use crate::wcdb_error::WCDBResult;
use crate::winq::expression::Expression;
use std::ffi::{c_char, c_void, CString};
use std::ptr::null_mut;
use std::sync::{Arc, Mutex};

pub type DatabaseCloseCallback = extern "C" fn(context: *mut c_void);

extern "C" {
    pub fn WCDBRustCore_createDatabase(path: *const c_char) -> *mut c_void;
    pub fn WCDBRustDatabase_getPath(cpp_obj: *mut c_void) -> *const c_char;
    pub fn WCDBRustDatabase_close(
        cpp_obj: *mut c_void,
        context: *mut c_void,
        cb: DatabaseCloseCallback,
    );
    pub fn WCDBRustDatabase_getHandle(cpp_obj: *mut c_void, write_hint: bool) -> *mut c_void;
    pub fn WCDBRustDatabase_getError(cpp_obj: *mut c_void) -> *mut c_void;
}

extern "C" fn close_callback_wrapper(context: *mut c_void) {
    if !context.is_null() {
        let boxed_cb: Box<Box<dyn FnOnce()>> = unsafe { Box::from_raw(context as *mut _) };
        boxed_cb();
    }
}

pub struct Database {
    handle_orm_operation: HandleORMOperation,
    close_callback: Arc<Mutex<Option<Box<dyn FnOnce() + Send>>>>,
}

impl CppObjectTrait for Database {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.handle_orm_operation.set_cpp_obj(cpp_obj)
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.handle_orm_operation.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.handle_orm_operation.release_cpp_object();
    }
}

impl HandleOperationTrait for Database {
    fn get_handle(&self, write_hint: bool) -> Handle {
        Handle::new(self, write_hint)
    }

    fn auto_invalidate_handle(&self) -> bool {
        true
    }

    fn run_transaction<F: FnOnce(Handle) -> bool>(&self, callback: F) -> WCDBResult<()> {
        unimplemented!()
    }
}

impl HandleORMOperationTrait for Database {
    fn create_table<T, R: TableBinding<T>>(
        &self,
        table_name: &str,
        binding: &R,
    ) -> WCDBResult<bool> {
        let handle = self.get_handle(true);
        binding.base_binding().create_table(table_name, handle)
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

    fn prepare_insert<T>(&self) -> Insert<T> {
        Insert::new(self.get_handle(true), false, self.auto_invalidate_handle())
    }

    fn delete_objects(&self, table_name: &str, expression: Expression) -> WCDBResult<()> {
        unimplemented!()
    }
}

impl Database {
    pub fn new(path: &str) -> Self {
        let c_path = CString::new(path).unwrap_or_default();
        let cpp_obj = unsafe { WCDBRustCore_createDatabase(c_path.as_ptr()) };
        Database {
            handle_orm_operation: HandleORMOperation::new_with_obj(cpp_obj),
            close_callback: Arc::new(Mutex::new(None)),
        }
    }

    pub fn get_path(&self) -> String {
        let path = unsafe { WCDBRustDatabase_getPath(self.get_cpp_obj()) };
        path.to_cow().to_string()
    }

    pub fn get_table<'a, T, R: TableBinding<T>>(
        &'a self,
        table_name: &str,
        binding: &'a R,
    ) -> Table<'a, T, R> {
        assert!(!table_name.is_empty());
        Table::new(table_name, binding, self)
    }

    pub fn close<CB>(&self, cb_opt: Option<CB>)
    where
        CB: FnOnce() + Send + 'static,
    {
        match cb_opt {
            None => unsafe {
                WCDBRustDatabase_close(self.get_cpp_obj(), null_mut(), close_callback_wrapper)
            },
            Some(cb) => {
                let boxed_cb: Box<Box<dyn FnOnce()>> = Box::new(Box::new(cb));
                let context = Box::into_raw(boxed_cb) as *mut c_void;
                unsafe {
                    WCDBRustDatabase_close(self.get_cpp_obj(), context, close_callback_wrapper)
                }
            }
        }
    }

    pub(crate) fn get_handle_raw(cpp_obj: *mut c_void, write_hint: bool) -> *mut c_void {
        unsafe { WCDBRustDatabase_getHandle(cpp_obj, write_hint) }
    }

    pub(crate) fn create_exception(&self) -> WCDBException {
        WCDBException::create_exception(unsafe { WCDBRustDatabase_getError(self.get_cpp_obj()) })
    }
}
