use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::wcdb_exception::WCDBResult;
use crate::core::handle::Handle;
use crate::utils::ToCString;
use crate::winq::column_def::ColumnDef;
use std::ffi::{c_char, c_void};
use std::ptr::null_mut;
use std::sync::RwLock;

extern "C" {
    pub fn WCDBRustBinding_create() -> *mut c_void;
    pub fn WCDBRustBinding_addColumnDef(cpp_obj: *mut c_void, column_def: *mut c_void);
    pub fn WCDBRustBinding_enableAutoIncrementForExistingTable(cpp_obj: *mut c_void);
    pub fn WCDBRustBinding_createTable(
        cpp_obj: *mut c_void,
        path: *const c_char,
        handle: *mut c_void,
    ) -> bool;
    pub fn WCDBRustBinding_getBaseBinding(cpp_obj: *mut c_void) -> *mut c_void;
}

pub struct Binding {
    cpp_obj: CppObject,
    base_binding: RwLock<*mut c_void>,
}

unsafe impl Send for Binding {}
unsafe impl Sync for Binding {}

impl Binding {
    pub fn new() -> Binding {
        Binding {
            cpp_obj: CppObject::new_with_obj(unsafe { WCDBRustBinding_create() }),
            base_binding: RwLock::new(null_mut()),
        }
    }

    pub fn add_column_def(&self, column_def: ColumnDef) {
        unsafe { WCDBRustBinding_addColumnDef(*self.cpp_obj, column_def.get_cpp_obj()) };
    }

    pub fn enable_auto_increment_for_existing_table(&self) {
        unsafe { WCDBRustBinding_enableAutoIncrementForExistingTable(*self.cpp_obj) };
    }

    pub fn create_table(&self, table_name: &str, mut handle: Handle) -> WCDBResult<bool> {
        let c_table_name = table_name.to_cstring();
        Ok(unsafe {
            WCDBRustBinding_createTable(
                *self.cpp_obj,
                c_table_name.as_ptr(),
                handle.get_cpp_handle()?,
            )
        })
    }

    pub fn get_base_binding(&self) -> *mut c_void {
        if self.base_binding.read().unwrap().is_null() {
            let base_binding = unsafe { WCDBRustBinding_getBaseBinding(*self.cpp_obj) };
            *self.base_binding.write().unwrap() = base_binding;
        }
        *self.base_binding.read().unwrap()
    }
}
