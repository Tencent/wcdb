use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::wcdb_exception::WCDBException;
use crate::wcdb_error::WCDBResult;
use crate::winq::statement::StatementTrait;
use std::ffi::c_void;

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

    pub fn bind_integer(&self, value: i32, index: usize) {
        unsafe { WCDBRustHandleStatement_bindInteger(*self.cpp_obj, value as i64, index) }
    }

    pub fn bind_null(&self, index: usize) {
        unsafe { WCDBRustHandleStatement_bindNull(*self.cpp_obj, index) }
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
