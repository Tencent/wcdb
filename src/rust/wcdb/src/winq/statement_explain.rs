use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::statement::{Statement, StatementTrait};
use std::ffi::c_void;

extern "C" {
    fn WCDBRustStatementExplain_createCppObj() -> *mut c_void;

    fn WCDBRustStatementExplain_explain(
        cpp_obj: *mut c_void,
        statement: *mut c_void,
        query_plan: bool,
    );
}

#[derive(Debug)]
pub struct StatementExplain {
    statement: Statement,
}

impl CppObjectTrait for StatementExplain {
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

impl CppObjectConvertibleTrait for StatementExplain {
    fn as_cpp_object(&self) -> &CppObject {
        self.statement.as_cpp_object()
    }
}

impl IdentifierTrait for StatementExplain {
    fn get_type(&self) -> CPPType {
        self.statement.get_type()
    }

    fn get_description(&self) -> String {
        self.statement.get_description()
    }
}

impl IdentifierConvertibleTrait for StatementExplain {
    fn as_identifier(&self) -> &Identifier {
        self.statement.as_identifier()
    }
}

impl StatementTrait for StatementExplain {
    fn is_write_statement(&self) -> bool {
        self.statement.is_write_statement()
    }
}

impl StatementExplain {
    pub fn new() -> Self {
        let cpp_obj = unsafe { WCDBRustStatementExplain_createCppObj() };
        StatementExplain {
            statement: Statement::new(CPPType::ExplainSTMT, Some(cpp_obj)),
        }
    }

    pub fn explain<T: CppObjectConvertibleTrait + ?Sized>(&self, statement: &T) -> &Self {
        unsafe {
            WCDBRustStatementExplain_explain(self.get_cpp_obj(), CppObject::get(statement), false);
        }
        self
    }

    pub fn explain_query_plan<T: CppObjectConvertibleTrait + ?Sized>(
        &self,
        statement: &T,
    ) -> &Self {
        unsafe {
            WCDBRustStatementExplain_explain(self.get_cpp_obj(), CppObject::get(statement), true);
        }
        self
    }
}
