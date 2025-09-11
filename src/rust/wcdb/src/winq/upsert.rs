use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::param::expression_convertible_param::ExpressionConvertibleParam;
use crate::base::param::string_column_trait_param::StringColumnTraitParam;
use crate::base::param::string_indexed_column_convertible_param::StringIndexedColumnConvertibleParam;
use crate::utils::ToCString;
use crate::winq::expression::Expression;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use std::ffi::{c_char, c_double, c_int, c_longlong, c_void};

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
        int_value: c_longlong,
        double_value: c_double,
        string_value: *const c_char,
    );
}

pub struct Upsert {
    identifier: Identifier,
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

impl CppObjectConvertibleTrait for Upsert {
    fn as_cpp_object(&self) -> &CppObject {
        self.identifier.as_cpp_object()
    }
}

impl IdentifierTrait for Upsert {
    fn get_type(&self) -> CPPType {
        self.identifier.get_type()
    }

    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierConvertibleTrait for Upsert {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

impl Upsert {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustUpsert_createCppObj() };
        Upsert {
            identifier: Identifier::new(CPPType::UpsertClause, Some(cpp_obj)),
        }
    }

    pub fn on_conflict(&self) -> &Self {
        self
    }

    pub fn indexed_by<'a, I, S>(&self, column_vec: I) -> &Self
    where
        I: IntoIterator<Item = S>,
        S: Into<StringIndexedColumnConvertibleParam<'a>>,
    {
        let data_vec = column_vec.into_iter().map(Into::into).collect::<Vec<_>>();
        if data_vec.is_empty() {
            return self;
        }
        let mut cpp_type = CPPType::String;
        let mut cpp_str_vec = vec![];
        let mut cpp_obj_vec = vec![];
        for item in data_vec {
            match item {
                StringIndexedColumnConvertibleParam::String(str) => {
                    cpp_str_vec.push(str.as_str().to_cstring().as_ptr());
                }
                StringIndexedColumnConvertibleParam::IndexedColumnConvertible(obj) => {
                    cpp_type = Identifier::get_cpp_type(obj.as_identifier());
                    cpp_obj_vec.push(CppObject::get(obj));
                }
            }
        }
        if !cpp_str_vec.is_empty() {
            unsafe {
                WCDBRustUpsert_configIndexedColumn(
                    self.get_cpp_obj(),
                    CPPType::String as c_int,
                    std::ptr::null_mut(),
                    cpp_str_vec.as_ptr(),
                    cpp_str_vec.len() as c_int,
                );
            }
        } else {
            unsafe {
                WCDBRustUpsert_configIndexedColumn(
                    self.get_cpp_obj(),
                    cpp_type as c_int,
                    cpp_obj_vec.as_ptr(),
                    std::ptr::null(),
                    cpp_obj_vec.len() as c_int,
                );
            }
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

    pub fn set<'a, I, S>(&self, column_vec: I) -> &Self
    where
        I: IntoIterator<Item = S>,
        S: Into<StringColumnTraitParam<'a>>,
    {
        let data_vec = column_vec.into_iter().map(Into::into).collect::<Vec<_>>();
        if data_vec.is_empty() {
            return self;
        }
        let mut cpp_type = CPPType::String;
        let mut cpp_str_vec = vec![];
        let mut cpp_obj_vec = vec![];
        for item in data_vec {
            match item {
                StringColumnTraitParam::String(str) => {
                    cpp_str_vec.push(str.as_str().to_cstring().as_ptr());
                }
                StringColumnTraitParam::Column(obj) => {
                    cpp_type = Identifier::get_cpp_type(obj.as_identifier());
                    cpp_obj_vec.push(CppObject::get(obj));
                }
            }
        }
        if !cpp_str_vec.is_empty() {
            unsafe {
                WCDBRustUpsert_configSetColumns(
                    self.get_cpp_obj(),
                    CPPType::String as c_int,
                    std::ptr::null_mut(),
                    cpp_str_vec.as_ptr(),
                    cpp_str_vec.len() as c_int,
                );
            }
        } else {
            unsafe {
                WCDBRustUpsert_configSetColumns(
                    self.get_cpp_obj(),
                    cpp_type as c_int,
                    cpp_obj_vec.as_ptr(),
                    std::ptr::null_mut(),
                    cpp_obj_vec.len() as c_int,
                )
            }
        }
        self
    }

    pub fn to<'a, V>(&self, value: V) -> &Self
    where
        V: Into<ExpressionConvertibleParam<'a>>,
    {
        let (cpp_type, int_value, double_value, string_value_opt) = value.into().get_params();
        let string_ptr: *const c_char = match string_value_opt.as_ref() {
            Some(s) => s.as_ptr(),
            None => std::ptr::null(),
        };
        unsafe {
            WCDBRustUpsert_configToValue(
                self.get_cpp_obj(),
                cpp_type as c_int,
                int_value,
                double_value,
                string_ptr,
            );
        }
        self
    }
}
