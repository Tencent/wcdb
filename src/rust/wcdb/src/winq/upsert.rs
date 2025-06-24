use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::column::Column;
use crate::winq::expression::Expression;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::identifier::{CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::indexed_column_convertible::IndexedColumnConvertibleTrait;
use std::ffi::{c_char, c_double, c_int, c_void};
use std::ptr::{null, null_mut};

extern "C" {
    fn WCDBRustUpsert_createCppObj() -> *mut c_void;

    fn WCDBRustUpsert_configIndexedColumn(
        cpp_obj: *mut c_void,
        cpp_obj_type: c_int,
        columns: *const *mut c_void,
        columns_string_vec: *const *const c_char,
        vec_len: c_int,
    );

    fn WCDBRustUpsert_configWhere(cpp_obj: *mut c_void, condition: *mut c_void);

    fn WCDBRustUpsert_configDoNothing(cpp_obj: *mut c_void);
    fn WCDBRustUpsert_configDoUpdate(cpp_obj: *mut c_void);

    fn WCDBRustUpsert_configSetColumns(
        cpp_obj: *mut c_void,
        cpp_obj_type: c_int,
        columns: *const *mut c_void,
        columns_string_vec: *const *const c_char,
        vec_len: c_int,
    );

    fn WCDBRustUpsert_configToValue(
        cpp_obj: *mut c_void,
        cpp_obj_type: c_int,
        int_value: *mut c_void,
        double_value: c_double,
        string_value: *const c_char,
    );
}
pub struct Upsert {
    identifier: Identifier,
}

impl CppObjectConvertibleTrait for Upsert {
    fn as_cpp_object(&self) -> *mut c_void {
        self.identifier.as_cpp_object()
    }
}

impl IdentifierConvertibleTrait for Upsert {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

impl IdentifierTrait for Upsert {
    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierStaticTrait for Upsert {
    fn get_type() -> i32 {
        CPPType::UpsertClause as i32
    }
}

impl CppObjectTrait for Upsert {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.identifier.set_cpp_obj(cpp_obj)
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.identifier.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.identifier.release_cpp_object()
    }
}

impl Upsert {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustUpsert_createCppObj() };
        Upsert {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn on_conflict(&self) -> &Self {
        self
    }

    pub fn indexed_by_column_names(&self, column_names: &Vec<String>) -> &Self {
        let len = column_names.len();
        let mut c_char_vec: Vec<*const c_char> = Vec::with_capacity(len);
        for x in column_names {
            c_char_vec.push(x.to_cstring().as_ptr());
        }
        unsafe {
            WCDBRustUpsert_configIndexedColumn(
                self.get_cpp_obj(),
                CPPType::String as c_int,
                null_mut(),
                c_char_vec.as_ptr(),
                len as c_int,
            );
        }
        self
    }

    pub fn indexed_by_indexed_column_convertible_trait<T>(&self, indexed_columns: Vec<&T>) -> &Self
    where
        T: IndexedColumnConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    {
        if indexed_columns.is_empty() {
            return self;
        }
        let len = indexed_columns.len();
        let mut i64_vec: Vec<*mut c_void> = Vec::with_capacity(len);
        let cpp_type = Identifier::get_cpp_type(indexed_columns[0]);
        for x in indexed_columns {
            i64_vec.push(CppObject::get(x));
        }
        unsafe {
            WCDBRustUpsert_configIndexedColumn(
                self.get_cpp_obj(),
                cpp_type,
                i64_vec.as_ptr(),
                null(),
                len as c_int,
            );
        }
        self
    }

    pub fn where_(&self, condition: &Expression) -> &Self {
        unsafe {
            WCDBRustUpsert_configWhere(self.get_cpp_obj(), CppObject::get(condition));
        }
        self
    }

    pub fn do_no_thing(&self) -> &Self {
        unsafe {
            WCDBRustUpsert_configDoNothing(self.get_cpp_obj());
        }
        self
    }

    pub fn do_update(&self) -> &Self {
        unsafe {
            WCDBRustUpsert_configDoUpdate(self.get_cpp_obj());
        }
        self
    }

    pub fn set_with_column_names(&self, column_names: &Vec<String>) -> &Self {
        if column_names.is_empty() {
            return self;
        }
        let len = column_names.len();
        let mut c_char_vec: Vec<*const c_char> = Vec::with_capacity(len);
        for x in column_names {
            c_char_vec.push(x.to_cstring().as_ptr());
        }
        unsafe {
            WCDBRustUpsert_configSetColumns(
                self.get_cpp_obj(),
                CPPType::String as c_int,
                null_mut(),
                c_char_vec.as_ptr(),
                len as c_int,
            )
        }
        self
    }

    pub fn set_with_columns(&self, columns: &Vec<Column>) -> &Self {
        let cpp_type = Identifier::get_cpp_type(&columns[0]);
        let len = columns.len();
        let mut i64_vec: Vec<*mut c_void> = Vec::with_capacity(len);
        for x in columns {
            i64_vec.push(CppObject::get(x));
        }
        unsafe {
            WCDBRustUpsert_configSetColumns(
                self.get_cpp_obj(),
                cpp_type,
                i64_vec.as_ptr(),
                null_mut(),
                len as c_int,
            )
        }
        self
    }

    pub fn to_bool(&self, value: bool) -> &Self {
        let value = if value { 1 } else { 0 };
        unsafe {
            WCDBRustUpsert_configToValue(
                self.get_cpp_obj(),
                CPPType::Bool as c_int,
                value as *mut c_void,
                0.0,
                null_mut(),
            );
        }
        self
    }

    pub fn to_u8(&self, value: u8) -> &Self {
        unsafe {
            WCDBRustUpsert_configToValue(
                self.get_cpp_obj(),
                CPPType::Int as c_int,
                value as *mut c_void,
                0.0,
                null_mut(),
            );
        }
        self
    }

    pub fn to_u16(&self, value: u16) -> &Self {
        unsafe {
            WCDBRustUpsert_configToValue(
                self.get_cpp_obj(),
                CPPType::Int as c_int,
                value as *mut c_void,
                0.0,
                null_mut(),
            );
        }
        self
    }

    pub fn to_i32(&self, value: i32) -> &Self {
        unsafe {
            WCDBRustUpsert_configToValue(
                self.get_cpp_obj(),
                CPPType::Int as c_int,
                value as *mut c_void,
                0.0,
                null_mut(),
            );
        }
        self
    }

    pub fn to_i64(&self, value: i64) -> &Self {
        unsafe {
            WCDBRustUpsert_configToValue(
                self.get_cpp_obj(),
                CPPType::Int as c_int,
                value as *mut c_void,
                0.0,
                null_mut(),
            );
        }
        self
    }

    pub fn to_f32(&self, value: f32) -> &Self {
        unsafe {
            WCDBRustUpsert_configToValue(
                self.get_cpp_obj(),
                CPPType::Double as c_int,
                0 as *mut c_void,
                value as c_double,
                null_mut(),
            );
        }
        self
    }

    pub fn to_f64(&self, value: f64) -> &Self {
        unsafe {
            WCDBRustUpsert_configToValue(
                self.get_cpp_obj(),
                CPPType::Double as c_int,
                0 as *mut c_void,
                value as c_double,
                null_mut(),
            );
        }
        self
    }

    pub fn to_string(&self, value: String) -> &Self {
        if !value.is_empty() {
            let c_str = value.to_cstring();
            unsafe {
                WCDBRustUpsert_configToValue(
                    self.get_cpp_obj(),
                    CPPType::String as c_int,
                    0 as *mut c_void,
                    0.0,
                    c_str.as_ptr(),
                );
            }
        } else {
            unsafe {
                WCDBRustUpsert_configToValue(
                    self.get_cpp_obj(),
                    CPPType::Null as c_int,
                    0 as *mut c_void,
                    0.0,
                    null_mut(),
                );
            }
        }
        self
    }

    pub fn to_expression_convertible_trait<T>(&self, value: Option<T>) -> &Self
    where
        T: ExpressionConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    {
        match value {
            None => unsafe {
                WCDBRustUpsert_configToValue(
                    self.get_cpp_obj(),
                    CPPType::Null as c_int,
                    0 as *mut c_void,
                    0.0,
                    null_mut(),
                );
            },
            Some(value) => unsafe {
                WCDBRustUpsert_configToValue(
                    self.get_cpp_obj(),
                    Identifier::get_cpp_type(&value) as c_int,
                    CppObject::get(&value),
                    0.0,
                    null_mut(),
                );
            },
        }
        self
    }
}
