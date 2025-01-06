use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::value::Value;
use crate::base::wcdb_exception::{WCDBException, WCDBResult};
use crate::orm::field::Field;
use crate::winq::column_type::ColumnType;
use crate::winq::statement::StatementTrait;
use std::ffi::c_void;
use std::sync::Arc;

extern "C" {
    pub fn WCDBRustHandleStatement_getError(cpp_obj: *mut c_void) -> *mut c_void;
    pub fn WCDBRustHandleStatement_prepare(cpp_obj: *mut c_void, statement: *mut c_void) -> bool;
    pub fn WCDBRustHandleStatement_step(cpp_obj: *mut c_void) -> bool;
    pub fn WCDBRustHandleStatement_reset(cpp_obj: *mut c_void);
    pub fn WCDBRustHandleStatement_finalize(cpp_obj: *mut c_void);
    pub fn WCDBRustHandleStatement_bindInteger(cpp_obj: *mut c_void, value: i64, index: usize);
    pub fn WCDBRustHandleStatement_bindNull(cpp_obj: *mut c_void, index: usize);
    pub fn WCDBRustHandleStatement_getInteger(cpp_obj: *mut c_void, index: usize) -> i64;
}

pub struct PreparedStatement {
    cpp_obj: CppObject,
    pub auto_finalize: bool,
    column_count: i32,
}

impl CppObjectTrait for PreparedStatement {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.cpp_obj.set_cpp_obj(cpp_obj)
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.cpp_obj.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.cpp_obj.release_cpp_object()
    }
}

impl PreparedStatement {
    pub fn new(cpp_obj: *mut c_void) -> PreparedStatement {
        PreparedStatement {
            cpp_obj: CppObject::new_with_obj(cpp_obj),
            auto_finalize: false,
            column_count: -1,
        }
    }

    pub fn create_exception(&self) -> WCDBException {
        WCDBException::create_exception(unsafe {
            WCDBRustHandleStatement_getError(self.get_cpp_obj())
        })
    }

    pub fn bind_int(&self, value: i32, index: usize) {
        self.bind_integer(value as i64, index);
    }

    pub fn bind_integer(&self, value: i64, index: usize) {
        unsafe { WCDBRustHandleStatement_bindInteger(*self.cpp_obj, value, index) }
    }

    pub fn bind_double(&self, value: f64, index: usize) {
        todo!("qixinbing")
    }

    pub fn bind_text(&self, value: &str, index: usize) {
        todo!("qixinbing")
    }

    pub fn bind_blob(&self, value: &Vec<u8>, index: usize) {
        todo!("qixinbing")
    }

    pub fn bind_null(&self, index: usize) {
        unsafe { WCDBRustHandleStatement_bindNull(*self.cpp_obj, index) }
    }

    pub fn bind_value(&self, value: &Value, index: usize) {
        let value_type = value.get_type();
        if ColumnType::Integer == value_type {
            self.bind_integer(value.get_long(), index);
            return;
        }
        if ColumnType::Float == value_type {
            self.bind_double(value.get_double(), index);
            return;
        }
        if ColumnType::Text == value_type {
            self.bind_text(value.get_text().as_str(), index);
            return;
        }
        if ColumnType::BLOB == value_type {
            self.bind_blob(&value.get_blob(), index);
            return;
        }
        self.bind_null(index);
    }

    pub fn bind_row(&self, row: &Vec<Value>) {
        let mut index = 1;
        for value in row {
            self.bind_value(value, index);
            index += 1;
        }
    }

    pub fn bind_object_by_fields<T>(
        arc_self: &Arc<PreparedStatement>,
        object: T,
        fields: &Vec<&Field<T>>,
    ) {
        if fields.is_empty() {
            return;
        }
        let mut index = 1;
        let binding = fields[0].get_table_binding();
        for field in fields {
            binding.bind_field(&object, field, index, arc_self);
            index += 1;
        }
    }

    pub fn get_int(&self, index: usize) -> i32 {
        unsafe { WCDBRustHandleStatement_getInteger(*self.cpp_obj, index) as i32 }
    }

    pub fn prepare<T: StatementTrait>(&self, statement: &T) -> WCDBResult<()> {
        if unsafe { WCDBRustHandleStatement_prepare(*self.cpp_obj, CppObject::get(statement)) } {
            Ok(())
        } else {
            Err(self.create_exception())
        }
    }

    pub fn step(&self) -> WCDBResult<()> {
        if !unsafe { WCDBRustHandleStatement_step(*self.cpp_obj) } {
            if self.auto_finalize {
                self.finalize_statement();
            }
            Err(self.create_exception())
        } else {
            Ok(())
        }
    }

    pub fn reset(&self) {
        unsafe { WCDBRustHandleStatement_reset(*self.cpp_obj) }
    }

    pub fn finalize_statement(&self) {
        unsafe { WCDBRustHandleStatement_finalize(*self.cpp_obj) }
    }
}
