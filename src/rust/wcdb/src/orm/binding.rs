use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::wcdb_exception::WCDBResult;
use crate::core::handle::Handle;
use crate::utils::ToCString;
use crate::winq::column_def::ColumnDef;
use crate::winq::statement_create_index::StatementCreateIndex;
use crate::winq::table_constraint::TableConstraint;
use std::ffi::{c_char, c_void, CString};
use std::sync::RwLock;

extern "C" {
    fn WCDBRustBinding_create() -> *mut c_void;

    fn WCDBRustBinding_addColumnDef(cpp_obj: *mut c_void, column_def: *mut c_void);

    fn WCDBRustBinding_enableAutoIncrementForExistingTable(cpp_obj: *mut c_void);

    fn WCDBRustBinding_addIndex(
        cpp_obj: *mut c_void,
        index_name_or_suffix: *const c_char,
        is_full_name: bool,
        create_index: *mut c_void,
    );

    fn WCDBRustBinding_configWithoutRowId(cpp_obj: *mut c_void);

    fn WCDBRustBinding_addTableConstraint(cpp_obj: *mut c_void, table_constraint: *mut c_void);

    fn WCDBRustBinding_configVirtualModule(cpp_obj: *mut c_void, module: *const c_char);

    fn WCDBRustBinding_configVirtualModuleArgument(cpp_obj: *mut c_void, argument: *const c_char);

    fn WCDBRustBinding_createTable(
        cpp_obj: *mut c_void,
        path: *const c_char,
        handle: *mut c_void,
    ) -> bool;

    fn WCDBRustBinding_getBaseBinding(cpp_obj: *mut c_void) -> *mut c_void;
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
            cpp_obj: CppObject::new(Some(unsafe { WCDBRustBinding_create() })),
            base_binding: RwLock::new(std::ptr::null_mut()),
        }
    }

    pub fn add_column_def(&self, column_def: ColumnDef) {
        unsafe {
            WCDBRustBinding_addColumnDef(self.cpp_obj.get_cpp_obj(), column_def.get_cpp_obj())
        };
    }

    pub fn enable_auto_increment_for_existing_table(&self) {
        unsafe { WCDBRustBinding_enableAutoIncrementForExistingTable(self.cpp_obj.get_cpp_obj()) };
    }

    pub fn add_index(
        &self,
        index_name_or_suffix: &str,
        is_full_name: bool,
        create_index: StatementCreateIndex,
    ) {
        let c_index_name = CString::new(index_name_or_suffix).unwrap_or_default();
        unsafe {
            WCDBRustBinding_addIndex(
                self.cpp_obj.get_cpp_obj(),
                c_index_name.as_ptr(),
                is_full_name,
                create_index.get_cpp_obj(),
            );
        }
    }

    pub fn add_table_constraint(&self, constraint: TableConstraint) {
        unsafe {
            WCDBRustBinding_addTableConstraint(self.cpp_obj.get_cpp_obj(), constraint.get_cpp_obj())
        };
    }

    pub fn config_virtual_module(&self, module: &str) -> &Binding {
        let cstr = module.to_cstring();
        unsafe { WCDBRustBinding_configVirtualModule(*self.cpp_obj, cstr.as_ptr()) }
        self
    }

    pub fn config_virtual_module_argument(&self, argument: &str) {
        let cstr = argument.to_cstring();
        unsafe {
            WCDBRustBinding_configVirtualModuleArgument(*self.cpp_obj, cstr.as_ptr());
        }
    }

    pub fn config_without_row_id(&self) {
        unsafe { WCDBRustBinding_configWithoutRowId(*self.cpp_obj) }
    }

    pub fn create_table(&self, table_name: &str, handle: Handle) -> WCDBResult<bool> {
        let c_table_name = table_name.to_cstring();
        let cpp_handle = handle.get_cpp_handle()?;
        Ok(unsafe {
            WCDBRustBinding_createTable(
                self.cpp_obj.get_cpp_obj(),
                c_table_name.as_ptr(),
                cpp_handle,
            )
        })
    }

    pub fn get_base_binding(&self) -> *mut c_void {
        // 先检查是否为空，但不保持读锁
        let is_null = match self.base_binding.read() {
            Ok(guard) => guard.is_null(),
            Err(_) => false, // 如果读取失败，假设不为空
        };

        // 如果为空，则获取写锁并设置值
        if is_null {
            if let Ok(mut write_guard) = self.base_binding.write() {
                // 再次检查是否为空，因为可能在我们释放读锁和获取写锁之间被其他线程修改
                if write_guard.is_null() {
                    let base_binding =
                        unsafe { WCDBRustBinding_getBaseBinding(self.cpp_obj.get_cpp_obj()) };
                    *write_guard = base_binding;
                }
            }
        }

        // 最后再读取一次返回值
        match self.base_binding.read() {
            Ok(guard) => *guard,
            Err(_) => std::ptr::null_mut(), // 如果读取失败，返回空指针
        }
    }
}
