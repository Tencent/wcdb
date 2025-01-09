use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::value::Value;
use crate::base::wcdb_exception::{WCDBException, WCDBResult};
use crate::orm::field::Field;
use crate::utils::ToCow;
use crate::winq::column_type::ColumnType;
use crate::winq::statement::StatementTrait;
use std::ffi::{c_char, c_double, c_void, CString};
use std::sync::Arc;

extern "C" {
    pub fn WCDBRustHandleStatement_getError(cpp_obj: *mut c_void) -> *mut c_void;
    pub fn WCDBRustHandleStatement_prepare(cpp_obj: *mut c_void, statement: *mut c_void) -> bool;
    pub fn WCDBRustHandleStatement_step(cpp_obj: *mut c_void) -> bool;
    pub fn WCDBRustHandleStatement_reset(cpp_obj: *mut c_void);
    pub fn WCDBRustHandleStatement_finalize(cpp_obj: *mut c_void);
    pub fn WCDBRustHandleStatement_isDone(cpp_obj: *mut c_void) -> bool;
    pub fn WCDBRustHandleStatement_bindInteger(cpp_obj: *mut c_void, value: i64, index: usize);
    pub fn WCDBRustHandleStatement_bindDouble(cpp_obj: *mut c_void, value: c_double, index: usize);
    pub fn WCDBRustHandleStatement_bindText(
        cpp_obj: *mut c_void,
        value: *const c_char,
        index: usize,
    );
    pub fn WCDBRustHandleStatement_bindNull(cpp_obj: *mut c_void, index: usize);
    pub fn WCDBRustHandleStatement_getInteger(cpp_obj: *mut c_void, index: usize) -> i64;
    pub fn WCDBRustHandleStatement_getDouble(cpp_obj: *mut c_void, index: usize) -> f64;
    pub fn WCDBRustHandleStatement_getText(cpp_obj: *mut c_void, index: usize) -> *const c_char;
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

    pub fn bind_bool(&self, value: bool, index: usize) {
        self.bind_integer(if value { 1 } else { 0 }, index);
    }

    pub fn bind_byte(&self, value: i8, index: usize) {
        self.bind_integer(value as i64, index);
    }

    pub fn bind_short(&self, value: i16, index: usize) {
        self.bind_integer(value as i64, index);
    }

    pub fn bind_int(&self, value: i32, index: usize) {
        self.bind_integer(value as i64, index);
    }

    pub fn bind_integer(&self, value: i64, index: usize) {
        self.bind_long(value, index);
    }

    pub fn bind_long(&self, value: i64, index: usize) {
        unsafe { WCDBRustHandleStatement_bindInteger(*self.cpp_obj, value, index) }
    }

    pub fn bind_float(&self, value: f32, index: usize) {
        self.bind_double(value as f64, index);
    }

    pub fn bind_double(&self, value: f64, index: usize) {
        unsafe { WCDBRustHandleStatement_bindDouble(*self.cpp_obj, value, index) }
    }

    pub fn bind_text(&self, value: String, index: usize) {
        let c_path = CString::new(value).unwrap_or_default();
        unsafe { WCDBRustHandleStatement_bindText(*self.cpp_obj, c_path.as_ptr(), index) }
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
            self.bind_text(value.get_text().to_string(), index);
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

    pub fn get_bool(&self, index: usize) -> bool {
        self.get_long(index) == 1
    }

    pub fn get_byte(&self, index: usize) -> i8 {
        self.get_long(index) as i8
    }

    pub fn get_short(&self, index: usize) -> i16 {
        self.get_long(index) as i16
    }

    pub fn get_int(&self, index: usize) -> i32 {
        self.get_long(index) as i32
    }

    pub fn get_long(&self, index: usize) -> i64 {
        unsafe { WCDBRustHandleStatement_getInteger(*self.cpp_obj, index) }
    }

    pub fn get_float(&self, index: usize) -> f32 {
        self.get_double(index) as f32
    }

    pub fn get_double(&self, index: usize) -> f64 {
        unsafe { WCDBRustHandleStatement_getDouble(*self.cpp_obj, index) }
    }

    pub fn get_text(&self, index: usize) -> String {
        unsafe {
            WCDBRustHandleStatement_getText(*self.cpp_obj, index)
                .to_cow()
                .to_string()
        }
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

    pub fn get_one_object<T>(&self, fields: &Vec<&Field<T>>) -> WCDBResult<T> {
        assert!(fields.len() > 0);
        let field_opt = fields.first();
        match field_opt {
            Some(field) => {
                let ret = field.get_table_binding().extract_object(fields, self);
                Ok(ret)
            }
            None => Err(WCDBException::create_exception(self.get_cpp_obj())),
        }
    }

    pub fn get_all_objects<T>(&self, fields: &Vec<&Field<T>>) -> WCDBResult<Vec<T>> {
        assert!(fields.len() > 0);
        let field_opt = fields.first();
        if field_opt.is_none() {
            return Err(WCDBException::create_exception(self.get_cpp_obj()));
        }
        let field = field_opt.unwrap();
        let tb = field.get_table_binding();

        let mut obj_vec: Vec<T> = Vec::new();
        self.step()?;

        while !self.is_done() {
            let obj = tb.extract_object(fields, self);
            obj_vec.push(obj);
            self.step()?;
        }

        Ok(obj_vec)
    }

    pub fn is_done(&self) -> bool {
        unsafe { WCDBRustHandleStatement_isDone(*self.cpp_obj) }
    }
}
