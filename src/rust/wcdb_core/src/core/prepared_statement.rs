use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::value::Value;
use crate::base::wcdb_exception::{WCDBException, WCDBResult};
use crate::orm::field::Field;
use crate::utils::{ToCString, ToCow};
use crate::winq::column_type::ColumnType;
use crate::winq::statement::StatementTrait;
use core::ffi::c_size_t;
use std::ffi::{c_char, c_double, c_int, c_long, c_void, CString};
use std::slice;
use std::sync::Arc;

extern "C" {
    fn WCDBRustHandleStatement_getError(cpp_obj: *mut c_void) -> *mut c_void;
    fn WCDBRustHandleStatement_prepare(cpp_obj: *mut c_void, statement: *mut c_void) -> bool;
    fn WCDBRustHandleStatement_prepareSQL(cpp_obj: *mut c_void, sql: *const c_char) -> bool;
    fn WCDBRustHandleStatement_step(cpp_obj: *mut c_void) -> bool;
    fn WCDBRustHandleStatement_reset(cpp_obj: *mut c_void);
    fn WCDBRustHandleStatement_finalize(cpp_obj: *mut c_void);
    fn WCDBRustHandleStatement_isDone(cpp_obj: *mut c_void) -> bool;
    fn WCDBRustHandleStatement_bindInteger(cpp_obj: *mut c_void, value: c_long, index: c_size_t);
    fn WCDBRustHandleStatement_bindDouble(cpp_obj: *mut c_void, value: c_double, index: c_size_t);
    fn WCDBRustHandleStatement_bindText(
        cpp_obj: *mut c_void,
        value: *const c_char,
        index: c_size_t,
    );
    fn WCDBRustHandleStatement_bindNull(cpp_obj: *mut c_void, index: c_size_t);
    fn WCDBRustHandleStatement_getInteger(cpp_obj: *mut c_void, index: c_size_t) -> c_long;
    fn WCDBRustHandleStatement_getDouble(cpp_obj: *mut c_void, index: c_size_t) -> c_double;
    fn WCDBRustHandleStatement_getText(cpp_obj: *mut c_void, index: c_size_t) -> *const c_char;
    fn WCDBRustHandleStatement_getColumnType(cpp_obj: *mut c_void, index: c_int) -> c_int;
    fn WCDBRustHandleStatement_getColumnCount(cpp_obj: *mut c_void) -> c_int;

    fn WCDBRustHandleStatement_getBLOB(
        cpp_obj: *mut c_void,
        index: c_size_t,
        data: *mut *const std::ffi::c_uchar,
        data_size: *mut std::ffi::c_longlong,
    );
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
        self.bind_i64(if value { 1 } else { 0 }, index);
    }

    pub fn bind_bool_opt(&self, value: Option<&bool>, index: usize) {
        if let Some(v) = value {
            self.bind_bool(*v, index);
        } else {
            self.bind_null(index);
        }
    }

    pub fn bind_i8(&self, value: i8, index: usize) {
        self.bind_i64(value as i64, index);
    }

    pub fn bind_i8_opt(&self, value: Option<&i8>, index: usize) {
        if let Some(v) = value {
            self.bind_i8(*v, index);
        } else {
            self.bind_null(index);
        }
    }

    pub fn bind_i16(&self, value: i16, index: usize) {
        self.bind_i64(value as i64, index);
    }

    pub fn bind_i16_opt(&self, value: Option<&i16>, index: usize) {
        if let Some(v) = value {
            self.bind_i16(*v, index);
        } else {
            self.bind_null(index);
        }
    }

    pub fn bind_i32(&self, value: i32, index: usize) {
        self.bind_i64(value as i64, index);
    }

    pub fn bind_i32_opt(&self, value: Option<&i32>, index: usize) {
        if let Some(v) = value {
            self.bind_i32(*v, index);
        } else {
            self.bind_null(index);
        }
    }

    pub fn bind_i64(&self, value: i64, index: usize) {
        unsafe { WCDBRustHandleStatement_bindInteger(*self.cpp_obj, value, index) }
    }

    pub fn bind_i64_opt(&self, value: Option<&i64>, index: usize) {
        if let Some(v) = value {
            self.bind_i64(*v, index);
        } else {
            self.bind_null(index);
        }
    }

    pub fn bind_f32(&self, value: f32, index: usize) {
        self.bind_f64(value as f64, index);
    }

    pub fn bind_f32_opt(&self, value: Option<&f32>, index: usize) {
        if let Some(v) = value {
            self.bind_f32(*v, index);
        } else {
            self.bind_null(index);
        }
    }

    pub fn bind_f64(&self, value: f64, index: usize) {
        unsafe { WCDBRustHandleStatement_bindDouble(*self.cpp_obj, value, index) }
    }

    pub fn bind_f64_opt(&self, value: Option<&f64>, index: usize) {
        if let Some(v) = value {
            self.bind_f64(*v, index);
        } else {
            self.bind_null(index);
        }
    }

    pub fn bind_text(&self, value: &str, index: usize) {
        let c_path = CString::new(value).unwrap_or_default();
        unsafe { WCDBRustHandleStatement_bindText(*self.cpp_obj, c_path.as_ptr(), index) }
    }

    pub fn bind_text_opt(&self, value: Option<&String>, index: usize) {
        if let Some(v) = value {
            self.bind_text(v.as_str(), index);
        } else {
            self.bind_null(index);
        }
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
            self.bind_i64(value.get_long(), index);
            return;
        }
        if ColumnType::Float == value_type {
            self.bind_f64(value.get_double(), index);
            return;
        }
        if ColumnType::Text == value_type {
            self.bind_text(&value.get_text(), index);
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

    pub fn get_column_type(&self, index: usize) -> ColumnType {
        let ret = unsafe { WCDBRustHandleStatement_getColumnType(*self.cpp_obj, index as c_int) };
        ColumnType::value_of(ret)
    }

    pub fn get_bool(&self, index: usize) -> bool {
        self.get_i64(index) == 1
    }

    pub fn get_bool_opt(&self, index: usize) -> Option<bool> {
        if self.get_column_type(index) == ColumnType::Null {
            return None;
        }
        Some(self.get_bool(index))
    }

    pub fn get_i8(&self, index: usize) -> i8 {
        self.get_i64(index) as i8
    }

    pub fn get_i8_opt(&self, index: usize) -> Option<i8> {
        if self.get_column_type(index) == ColumnType::Null {
            return None;
        }
        Some(self.get_i8(index))
    }

    pub fn get_i16(&self, index: usize) -> i16 {
        self.get_i64(index) as i16
    }

    pub fn get_i16_opt(&self, index: usize) -> Option<i16> {
        if self.get_column_type(index) == ColumnType::Null {
            return None;
        }
        Some(self.get_i16(index))
    }

    pub fn get_i32(&self, index: usize) -> i32 {
        self.get_i64(index) as i32
    }

    pub fn get_i32_opt(&self, index: usize) -> Option<i32> {
        if self.get_column_type(index) == ColumnType::Null {
            return None;
        }
        Some(self.get_i32(index))
    }

    pub fn get_i64(&self, index: usize) -> i64 {
        unsafe { WCDBRustHandleStatement_getInteger(*self.cpp_obj, index) }
    }

    pub fn get_i64_opt(&self, index: usize) -> Option<i64> {
        if self.get_column_type(index) == ColumnType::Null {
            return None;
        }
        Some(self.get_i64(index))
    }

    pub fn get_f32(&self, index: usize) -> f32 {
        self.get_f64(index) as f32
    }

    pub fn get_f32_opt(&self, index: usize) -> Option<f32> {
        if self.get_column_type(index) == ColumnType::Null {
            return None;
        }
        Some(self.get_f32(index))
    }

    pub fn get_f64(&self, index: usize) -> f64 {
        unsafe { WCDBRustHandleStatement_getDouble(*self.cpp_obj, index) }
    }

    pub fn get_f64_opt(&self, index: usize) -> Option<f64> {
        if self.get_column_type(index) == ColumnType::Null {
            return None;
        }
        Some(self.get_f64(index))
    }

    pub fn get_text(&self, index: usize) -> String {
        let text = unsafe { WCDBRustHandleStatement_getText(*self.cpp_obj, index) };
        text.to_cow().to_string()
    }

    pub fn get_text_opt(&self, index: usize) -> Option<String> {
        if self.get_column_type(index) == ColumnType::Null {
            return None;
        }
        Some(self.get_text(index))
    }

    pub fn get_blob(&self, index: usize) -> Vec<u8> {
        let mut blob_ptr: *const u8 = std::ptr::null();
        let mut blob_len: i64 = 0;

        unsafe {
            WCDBRustHandleStatement_getBLOB(*self.cpp_obj, index, &mut blob_ptr, &mut blob_len)
        };
        if blob_len <= 0 || blob_ptr.is_null() {
            return Vec::new();
        }
        let blob: &[u8] = unsafe { slice::from_raw_parts(blob_ptr, blob_len as usize) };
        blob.to_vec()
    }

    pub fn prepare<T: StatementTrait>(&self, statement: &T) -> WCDBResult<()> {
        if unsafe { WCDBRustHandleStatement_prepare(*self.cpp_obj, CppObject::get(statement)) } {
            Ok(())
        } else {
            Err(self.create_exception())
        }
    }

    pub fn prepare_with_sql(&self, sql: &str) -> WCDBResult<()> {
        let cstr = sql.to_cstring();
        if unsafe { WCDBRustHandleStatement_prepareSQL(*self.cpp_obj, cstr.as_ptr()) } {
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

    pub fn get_all_values(&self) -> WCDBResult<Vec<Vec<Value>>> {
        let count = unsafe { WCDBRustHandleStatement_getColumnCount(*self.cpp_obj) as i32 };
        if count == 0 {
            return Ok(Vec::new());
        }

        let mut rows: Vec<Vec<Value>> = Vec::new();
        self.step()?;
        while !self.is_done() {
            let mut row: Vec<Value> = Vec::new();
            for i in 0..count {
                row.push(self.get_value(i));
            }
            rows.push(row);
            self.step()?;
        }
        Ok(rows)
    }

    pub fn is_done(&self) -> bool {
        unsafe { WCDBRustHandleStatement_isDone(*self.cpp_obj) }
    }

    pub fn get_value(&self, index: i32) -> Value {
        let ret = unsafe { WCDBRustHandleStatement_getColumnType(*self.cpp_obj, index as c_int) };
        if ret == 1 {
            Value::new_long(self.get_i64(index as usize))
        } else if ret == 2 {
            Value::new_double(self.get_f64(index as usize))
        } else if ret == 3 {
            Value::new_string(&*self.get_text(index as usize))
        } else if ret == 4 {
            Value::new_blob(self.get_blob(index as usize))
        } else {
            Value::new()
        }
    }

    pub fn get_one_row(&mut self) -> Vec<Value> {
        let count = self.get_column_count();
        let mut row: Vec<Value> = Vec::new();
        if count == 0 {
            return row;
        }
        for i in 0..count {
            row.push(self.get_value(i));
        }
        row
    }

    pub fn get_multi_rows(&mut self) -> WCDBResult<Vec<Vec<Value>>> {
        let mut rows: Vec<Vec<Value>> = Vec::new();
        self.step()?;
        while !self.is_done() {
            rows.push(self.get_one_row());
            self.step()?;
        }
        Ok(rows)
    }

    pub fn get_column_count(&mut self) -> i32 {
        if self.column_count == -1 {
            self.column_count =
                unsafe { WCDBRustHandleStatement_getColumnCount(*self.cpp_obj) as i32 };
        }
        self.column_count
    }
}
