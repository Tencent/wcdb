use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::param::string_column_trait_param::StringColumnTraitParam;
use crate::base::param::string_table_or_subquery_convertible_param::StringTableOrSubqueryConvertibleParam;
use crate::utils::ToCString;
use crate::winq::expression::Expression;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::table_or_subquery_convertible_trait::TableOrSubqueryConvertibleTrait;
use core::ffi::c_size_t;
use std::ffi::{c_char, c_int, c_void};

extern "C" {
    fn WCDBRustJoin_createCppObj(
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    ) -> *mut c_void;

    fn WCDBRustJoin_configWith(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustJoin_configWithJoin(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustJoin_configWithLeftOuterJoin(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustJoin_configWithLeftJoin(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustJoin_configWithInnerJoin(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustJoin_configWithCrossJoin(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustJoin_configWithNaturalJoin(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustJoin_configWithNaturalLeftOuterJoin(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustJoin_configWithNaturalLeftJoin(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustJoin_configWithNaturalInnerJoin(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustJoin_configWithNaturalCrossJoin(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustJoin_configOn(cpp_obj: *mut c_void, expression: *mut c_void);

    fn WCDBRustJoin_configUsingColumn(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        columns: *const *mut c_void,
        column_names: *const *const c_char,
        vec_len: c_size_t,
    );
}

pub struct Join {
    identifier: Identifier,
}

impl CppObjectTrait for Join {
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

impl CppObjectConvertibleTrait for Join {
    fn as_cpp_object(&self) -> &CppObject {
        self.identifier.as_cpp_object()
    }
}

impl IdentifierTrait for Join {
    fn get_type(&self) -> CPPType {
        self.identifier.get_type()
    }

    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierConvertibleTrait for Join {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

impl TableOrSubqueryConvertibleTrait for Join {}

impl Join {
    pub fn new<'a, S>(value: S) -> Self
    where
        S: Into<StringTableOrSubqueryConvertibleParam<'a>>,
    {
        let cpp_obj = match value.into() {
            StringTableOrSubqueryConvertibleParam::String(table_name) => unsafe {
                let cstr = table_name.to_cstring();
                WCDBRustJoin_createCppObj(CPPType::String as c_int, 0 as *mut c_void, cstr.as_ptr())
            },
            StringTableOrSubqueryConvertibleParam::TableOrSubquery(table_or_subquery) => unsafe {
                WCDBRustJoin_createCppObj(
                    Identifier::get_cpp_type(table_or_subquery) as c_int,
                    CppObject::get(table_or_subquery),
                    std::ptr::null(),
                )
            },
        };
        Join {
            identifier: Identifier::new(CPPType::JoinClause, Some(cpp_obj)),
        }
    }

    pub fn with<'a, S>(&self, value: S) -> &Join
    where
        S: Into<StringTableOrSubqueryConvertibleParam<'a>>,
    {
        match value.into() {
            StringTableOrSubqueryConvertibleParam::String(table_name) => {
                let cstr = table_name.to_cstring();
                unsafe {
                    WCDBRustJoin_configWith(
                        self.get_cpp_obj(),
                        CPPType::String as c_int,
                        0 as *mut c_void,
                        cstr.as_ptr(),
                    );
                }
            }
            StringTableOrSubqueryConvertibleParam::TableOrSubquery(table_or_subquery) => unsafe {
                WCDBRustJoin_configWith(
                    self.get_cpp_obj(),
                    Identifier::get_cpp_type(table_or_subquery) as c_int,
                    CppObject::get(table_or_subquery),
                    std::ptr::null(),
                );
            },
        }
        self
    }

    pub fn join<'a, S>(&self, value: S) -> &Join
    where
        S: Into<StringTableOrSubqueryConvertibleParam<'a>>,
    {
        match value.into() {
            StringTableOrSubqueryConvertibleParam::String(table_name) => {
                let cstr = table_name.to_cstring();
                unsafe {
                    WCDBRustJoin_configWithJoin(
                        self.get_cpp_obj(),
                        CPPType::String as c_int,
                        0 as *mut c_void,
                        cstr.as_ptr(),
                    );
                }
            }
            StringTableOrSubqueryConvertibleParam::TableOrSubquery(table_or_subquery) => unsafe {
                WCDBRustJoin_configWithJoin(
                    self.get_cpp_obj(),
                    Identifier::get_cpp_type(table_or_subquery) as c_int,
                    CppObject::get(table_or_subquery),
                    std::ptr::null(),
                );
            },
        }
        self
    }

    pub fn left_outer_join<'a, S>(&self, value: S) -> &Join
    where
        S: Into<StringTableOrSubqueryConvertibleParam<'a>>,
    {
        match value.into() {
            StringTableOrSubqueryConvertibleParam::String(table_name) => {
                let cstr = table_name.to_cstring();
                unsafe {
                    WCDBRustJoin_configWithLeftOuterJoin(
                        self.get_cpp_obj(),
                        CPPType::String as c_int,
                        0 as *mut c_void,
                        cstr.as_ptr(),
                    );
                }
            }
            StringTableOrSubqueryConvertibleParam::TableOrSubquery(table_or_subquery) => unsafe {
                WCDBRustJoin_configWithLeftOuterJoin(
                    self.get_cpp_obj(),
                    Identifier::get_cpp_type(table_or_subquery) as c_int,
                    CppObject::get(table_or_subquery),
                    std::ptr::null(),
                );
            },
        }
        self
    }

    pub fn left_join<'a, S>(&self, value: S) -> &Join
    where
        S: Into<StringTableOrSubqueryConvertibleParam<'a>>,
    {
        match value.into() {
            StringTableOrSubqueryConvertibleParam::String(table_name) => {
                let cstr = table_name.to_cstring();
                unsafe {
                    WCDBRustJoin_configWithLeftJoin(
                        self.get_cpp_obj(),
                        CPPType::String as c_int,
                        0 as *mut c_void,
                        cstr.as_ptr(),
                    );
                }
            }
            StringTableOrSubqueryConvertibleParam::TableOrSubquery(table_or_subquery) => unsafe {
                WCDBRustJoin_configWithLeftJoin(
                    self.get_cpp_obj(),
                    Identifier::get_cpp_type(table_or_subquery) as c_int,
                    CppObject::get(table_or_subquery),
                    std::ptr::null(),
                );
            },
        }
        self
    }

    pub fn inner_join<'a, S>(&self, value: S) -> &Join
    where
        S: Into<StringTableOrSubqueryConvertibleParam<'a>>,
    {
        match value.into() {
            StringTableOrSubqueryConvertibleParam::String(table_name) => {
                let cstr = table_name.to_cstring();
                unsafe {
                    WCDBRustJoin_configWithInnerJoin(
                        self.get_cpp_obj(),
                        CPPType::String as c_int,
                        0 as *mut c_void,
                        cstr.as_ptr(),
                    );
                }
            }
            StringTableOrSubqueryConvertibleParam::TableOrSubquery(table_or_subquery) => unsafe {
                WCDBRustJoin_configWithInnerJoin(
                    self.get_cpp_obj(),
                    Identifier::get_cpp_type(table_or_subquery) as c_int,
                    CppObject::get(table_or_subquery),
                    std::ptr::null(),
                );
            },
        }
        self
    }

    pub fn cross_join<'a, S>(&self, value: S) -> &Join
    where
        S: Into<StringTableOrSubqueryConvertibleParam<'a>>,
    {
        match value.into() {
            StringTableOrSubqueryConvertibleParam::String(table_name) => {
                let cstr = table_name.to_cstring();
                unsafe {
                    WCDBRustJoin_configWithCrossJoin(
                        self.get_cpp_obj(),
                        CPPType::String as c_int,
                        0 as *mut c_void,
                        cstr.as_ptr(),
                    );
                }
            }
            StringTableOrSubqueryConvertibleParam::TableOrSubquery(table_or_subquery) => unsafe {
                WCDBRustJoin_configWithCrossJoin(
                    self.get_cpp_obj(),
                    Identifier::get_cpp_type(table_or_subquery) as c_int,
                    CppObject::get(table_or_subquery),
                    std::ptr::null(),
                );
            },
        }
        self
    }

    pub fn natural_join<'a, S>(&self, value: S) -> &Join
    where
        S: Into<StringTableOrSubqueryConvertibleParam<'a>>,
    {
        match value.into() {
            StringTableOrSubqueryConvertibleParam::String(table_name) => {
                let cstr = table_name.to_cstring();
                unsafe {
                    WCDBRustJoin_configWithNaturalJoin(
                        self.get_cpp_obj(),
                        CPPType::String as c_int,
                        0 as *mut c_void,
                        cstr.as_ptr(),
                    );
                }
            }
            StringTableOrSubqueryConvertibleParam::TableOrSubquery(table_or_subquery) => unsafe {
                WCDBRustJoin_configWithNaturalJoin(
                    self.get_cpp_obj(),
                    Identifier::get_cpp_type(table_or_subquery) as c_int,
                    CppObject::get(table_or_subquery),
                    std::ptr::null(),
                );
            },
        }
        self
    }

    pub fn natural_left_outer_join<'a, S>(&self, value: S) -> &Join
    where
        S: Into<StringTableOrSubqueryConvertibleParam<'a>>,
    {
        match value.into() {
            StringTableOrSubqueryConvertibleParam::String(table_name) => {
                let cstr = table_name.to_cstring();
                unsafe {
                    WCDBRustJoin_configWithNaturalLeftOuterJoin(
                        self.get_cpp_obj(),
                        CPPType::String as c_int,
                        0 as *mut c_void,
                        cstr.as_ptr(),
                    );
                }
            }
            StringTableOrSubqueryConvertibleParam::TableOrSubquery(table_or_subquery) => unsafe {
                WCDBRustJoin_configWithNaturalLeftOuterJoin(
                    self.get_cpp_obj(),
                    Identifier::get_cpp_type(table_or_subquery) as c_int,
                    CppObject::get(table_or_subquery),
                    std::ptr::null(),
                );
            },
        }
        self
    }

    pub fn natural_left_join<'a, S>(&self, value: S) -> &Join
    where
        S: Into<StringTableOrSubqueryConvertibleParam<'a>>,
    {
        match value.into() {
            StringTableOrSubqueryConvertibleParam::String(table_name) => {
                let cstr = table_name.to_cstring();
                unsafe {
                    WCDBRustJoin_configWithNaturalLeftJoin(
                        self.get_cpp_obj(),
                        CPPType::String as c_int,
                        0 as *mut c_void,
                        cstr.as_ptr(),
                    );
                }
            }
            StringTableOrSubqueryConvertibleParam::TableOrSubquery(table_or_subquery) => unsafe {
                WCDBRustJoin_configWithNaturalLeftJoin(
                    self.get_cpp_obj(),
                    Identifier::get_cpp_type(table_or_subquery) as c_int,
                    CppObject::get(table_or_subquery),
                    std::ptr::null(),
                );
            },
        }
        self
    }

    pub fn natural_inner_join<'a, S>(&self, value: S) -> &Join
    where
        S: Into<StringTableOrSubqueryConvertibleParam<'a>>,
    {
        match value.into() {
            StringTableOrSubqueryConvertibleParam::String(table_name) => {
                let cstr = table_name.to_cstring();
                unsafe {
                    WCDBRustJoin_configWithNaturalInnerJoin(
                        self.get_cpp_obj(),
                        CPPType::String as c_int,
                        0 as *mut c_void,
                        cstr.as_ptr(),
                    );
                }
            }
            StringTableOrSubqueryConvertibleParam::TableOrSubquery(table_or_subquery) => unsafe {
                WCDBRustJoin_configWithNaturalInnerJoin(
                    self.get_cpp_obj(),
                    Identifier::get_cpp_type(table_or_subquery) as c_int,
                    CppObject::get(table_or_subquery),
                    std::ptr::null(),
                );
            },
        }
        self
    }

    pub fn natural_cross_join<'a, S>(&self, value: S) -> &Join
    where
        S: Into<StringTableOrSubqueryConvertibleParam<'a>>,
    {
        match value.into() {
            StringTableOrSubqueryConvertibleParam::String(table_name) => {
                let cstr = table_name.to_cstring();
                unsafe {
                    WCDBRustJoin_configWithNaturalCrossJoin(
                        self.get_cpp_obj(),
                        CPPType::String as c_int,
                        0 as *mut c_void,
                        cstr.as_ptr(),
                    );
                }
            }
            StringTableOrSubqueryConvertibleParam::TableOrSubquery(table_or_subquery) => unsafe {
                WCDBRustJoin_configWithNaturalCrossJoin(
                    self.get_cpp_obj(),
                    Identifier::get_cpp_type(table_or_subquery) as c_int,
                    CppObject::get(table_or_subquery),
                    std::ptr::null(),
                );
            },
        }
        self
    }

    pub fn on(&self, expression: &Expression) -> &Join {
        unsafe {
            WCDBRustJoin_configOn(self.get_cpp_obj(), CppObject::get(expression));
        }
        self
    }

    pub fn using<'a, I, S>(&self, column_vec: I) -> &Join
    where
        I: IntoIterator<Item = S>,
        S: Into<StringColumnTraitParam<'a>>,
    {
        let mut data_vec = column_vec.into_iter().map(Into::into).peekable();
        if data_vec.peek().is_none() {
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
                WCDBRustJoin_configUsingColumn(
                    self.get_cpp_obj(),
                    CPPType::String as c_int,
                    std::ptr::null(),
                    cpp_str_vec.as_ptr(),
                    cpp_str_vec.len(),
                );
            }
        } else {
            unsafe {
                WCDBRustJoin_configUsingColumn(
                    self.get_cpp_obj(),
                    CPPType::Column as c_int,
                    cpp_obj_vec.as_ptr(),
                    std::ptr::null(),
                    cpp_obj_vec.len(),
                );
            }
        }
        self
    }
}
