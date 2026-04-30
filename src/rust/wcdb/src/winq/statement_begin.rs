use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::{c_int, c_void};

pub struct TransactionType;
impl TransactionType {
    pub const DEFERRED: i32 = 0;
    pub const IMMEDIATE: i32 = 1;
    pub const EXCLUSIVE: i32 = 2;
}

extern "C" {
    fn WCDBRustStatementBegin_create(type_i: c_int) -> *mut c_void;
}

#[derive(Debug)]
pub struct StatementBegin {
    statement: Statement,
}

impl CppObjectTrait for StatementBegin {
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

impl CppObjectConvertibleTrait for StatementBegin {
    fn as_cpp_object(&self) -> &CppObject {
        self.statement.as_cpp_object()
    }
}

impl IdentifierTrait for StatementBegin {
    fn get_type(&self) -> CPPType {
        self.statement.get_type()
    }

    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierConvertibleTrait for StatementBegin {
    fn as_identifier(&self) -> &Identifier {
        self.statement.as_identifier()
    }
}

impl StatementTrait for StatementBegin {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementBegin {
    pub fn new(cpp_type: Option<i32>) -> Self {
        let transaction_type: i32 = cpp_type.unwrap_or_else(|| TransactionType::DEFERRED);
        let cpp_obj = unsafe { WCDBRustStatementBegin_create(transaction_type) };
        StatementBegin {
            statement: Statement::new(CPPType::CommitSTMT, Some(cpp_obj)),
        }
    }

    pub fn begin_deferred() -> StatementBegin {
        StatementBegin::new(Some(TransactionType::DEFERRED))
    }

    pub fn begin_immediate() -> StatementBegin {
        StatementBegin::new(Some(TransactionType::IMMEDIATE))
    }

    pub fn begin_exclusive() -> StatementBegin {
        StatementBegin::new(Some(TransactionType::EXCLUSIVE))
    }
}
