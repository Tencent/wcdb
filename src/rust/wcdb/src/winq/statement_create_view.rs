use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::base::param::{StringColumnParam, StringIndexedColumnConvertibleParam};
use crate::utils::ToCString;
use crate::winq::column::Column;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::schema::Schema;
use crate::winq::statement::{Statement, StatementTrait};
use crate::winq::statement_select::StatementSelect;
use std::ffi::{c_char, c_int, c_void, CString};

extern "C" {
    fn WCDBRustStatementCreateView_createCppObj() -> *mut c_void;

    fn WCDBRustStatementCreateView_configView(cpp_obj: *mut c_void, name: *const c_char);

    fn WCDBRustStatementCreateView_configTemp(cpp_obj: *mut c_void);

    fn WCDBRustStatementCreateView_configSchema(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *const c_void,
        path: *const c_char,
    );

    fn WCDBRustStatementCreateView_configIfNotExist(cpp_obj: *mut c_void);

    fn WCDBRustStatementCreateView_configColumns(
        cpp_obj: *mut c_void,
        cpp_obj_type: c_int,
        columns: *const *mut c_void,
        columns_string_vec: *const *const c_char,
        vec_len: c_int,
    );

    fn WCDBRustStatementCreateView_configAs(cpp_obj: *mut c_void, select: *const c_void);
}

#[derive(Debug)]
pub struct StatementCreateView {
    statement: Statement,
}

impl CppObjectTrait for StatementCreateView {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.statement.set_cpp_obj(cpp_obj);
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.statement.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.statement.release_cpp_object();
    }
}

impl CppObjectConvertibleTrait for StatementCreateView {
    fn as_cpp_object(&self) -> &CppObject {
        self.statement.as_cpp_object()
    }
}

impl IdentifierTrait for StatementCreateView {
    fn get_type(&self) -> CPPType {
        self.statement.get_type()
    }

    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierConvertibleTrait for StatementCreateView {
    fn as_identifier(&self) -> &Identifier {
        self.statement.as_identifier()
    }
}

impl StatementTrait for StatementCreateView {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementCreateView {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementCreateView_createCppObj() };
        StatementCreateView {
            statement: Statement::new(CPPType::CreateViewSTMT, Some(cpp_obj)),
        }
    }

    pub fn create_view(&self, name: &str) -> &Self {
        let c_str = name.to_string().to_cstring();
        unsafe {
            WCDBRustStatementCreateView_configView(self.get_cpp_obj(), c_str.as_ptr());
        }
        self
    }

    pub fn create_temp_view(&self, name: &str) -> &Self {
        let c_str = name.to_string().to_cstring();
        unsafe {
            WCDBRustStatementCreateView_configView(self.get_cpp_obj(), c_str.as_ptr());
            WCDBRustStatementCreateView_configTemp(self.get_cpp_obj());
        }
        self
    }

    pub fn of_with_string(&self, schema_name: &str) -> &Self {
        let c_str = schema_name.to_string().to_cstring();
        unsafe {
            WCDBRustStatementCreateView_configSchema(
                self.get_cpp_obj(),
                CPPType::String as c_int,
                std::ptr::null(),
                c_str.as_ptr(),
            );
        }
        self
    }

    pub fn of_with_schema(&self, schema: Schema) -> &Self {
        unsafe {
            WCDBRustStatementCreateView_configSchema(
                self.get_cpp_obj(),
                Identifier::get_cpp_type(&schema) as c_int,
                CppObject::get(&schema),
                std::ptr::null(),
            )
        }
        self
    }

    pub fn if_not_exist(&self) -> &Self {
        unsafe {
            WCDBRustStatementCreateView_configIfNotExist(self.get_cpp_obj());
        }
        self
    }

    pub fn with_columns<'a, I, S>(&self, column_vec: I) -> &Self
    where
        I: IntoIterator<Item = S>,
        S: Into<StringColumnParam<'a>>,
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
                StringColumnParam::String(str) => {
                    cpp_str_vec.push(str.as_str().to_cstring().as_ptr());
                }
                StringColumnParam::Column(obj) => {
                    cpp_type = Identifier::get_cpp_type(obj.as_identifier());
                    cpp_obj_vec.push(CppObject::get(obj));
                }
            }
        }
        if !cpp_str_vec.is_empty() {
            unsafe {
                WCDBRustStatementCreateView_configColumns(
                    self.get_cpp_obj(),
                    CPPType::String as c_int,
                    std::ptr::null_mut(),
                    cpp_str_vec.as_ptr(),
                    cpp_obj_vec.len() as c_int,
                )
            }
        } else {
            unsafe {
                WCDBRustStatementCreateView_configColumns(
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

    pub fn as_statement_select(&self, select: &StatementSelect) -> &Self {
        unsafe {
            WCDBRustStatementCreateView_configAs(self.get_cpp_obj(), CppObject::get(select));
        }
        self
    }
}
