use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::wcdb_error::{WCDBError, WCDBResult};
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::c_void;

extern "C" {
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

impl PreparedStatement {
    pub fn new(cpp_obj: *mut c_void) -> PreparedStatement {
        PreparedStatement {
            cpp_obj: CppObject::new_with_obj(cpp_obj),
            auto_finalize: false,
            column_count: -1,
        }
    }

    pub fn bind_integer(&self, value: i32, index: usize) {
        unsafe { WCDBRustHandleStatement_bindInteger(*self.cpp_obj, value as i64, index) }
    }

    pub fn bind_null(&self, index: usize) {
        unsafe { WCDBRustHandleStatement_bindNull(*self.cpp_obj, index) }
    }

    pub fn get_int(&self, index: usize) -> i32 {
        unsafe { WCDBRustHandleStatement_getInteger(*self.cpp_obj, index) as i32 }
    }

    pub fn prepare<T: StatementTrait + CppObjectTrait>(&self, statement: &T) -> WCDBResult<()> {
        if unsafe { WCDBRustHandleStatement_prepare(*self.cpp_obj, CppObject::get(statement)) } {
            Ok(())
        } else {
            Err(WCDBError::Exception)
        }
    }

    pub fn step(&self) {
        if !unsafe { WCDBRustHandleStatement_step(*self.cpp_obj) } {
            if self.auto_finalize {
                self.finalize_statement();
            }
            // throw createException();
        }
    }

    pub fn reset(&self) {
        unsafe { WCDBRustHandleStatement_reset(*self.cpp_obj) }
    }

    pub fn finalize_statement(&self) {
        unsafe { WCDBRustHandleStatement_finalize(*self.cpp_obj) }
    }
}
