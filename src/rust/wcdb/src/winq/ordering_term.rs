use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use std::ffi::{c_int, c_void};

extern "C" {
    fn WCDBRustOrderingTerm_create(cpp_type: c_int, expression: *mut c_void) -> *mut c_void;

    fn WCDBRustOrderingTerm_configOrder(cpp_obj: *mut c_void, order: c_int);
}

pub enum Order {
    Asc,
    Desc,
}

#[derive(Debug)]
pub struct OrderingTerm {
    identifier: Identifier,
}

impl CppObjectTrait for OrderingTerm {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.identifier.set_cpp_obj(cpp_obj);
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.identifier.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.identifier.release_cpp_object();
    }
}

impl CppObjectConvertibleTrait for OrderingTerm {
    fn as_cpp_object(&self) -> &CppObject {}
}

impl IdentifierTrait for OrderingTerm {
    fn get_type(&self) -> CPPType {
        self.identifier.get_type()
    }

    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierConvertibleTrait for OrderingTerm {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

impl OrderingTerm {
    pub fn new<T: ExpressionConvertibleTrait>(expression: &T) -> Self {
        let cpp_obj = unsafe {
            WCDBRustOrderingTerm_create(
                Identifier::get_cpp_type(expression) as c_int,
                CppObject::get(expression),
            )
        };
        OrderingTerm {
            identifier: Identifier::new(CPPType::OrderingTerm, Some(cpp_obj)),
        }
    }
}

impl OrderingTerm {
    pub fn order(self, order: Order) -> Self {
        unsafe {
            WCDBRustOrderingTerm_configOrder(self.get_cpp_obj(), (order as i32) + 1);
        }
        self
    }
}
