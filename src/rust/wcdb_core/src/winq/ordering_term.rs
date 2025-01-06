use crate::base::cpp_object::CppObjectTrait;
use crate::winq::expression_operable::ExpressionOperable;
use crate::winq::identifier::{CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait};
use std::ffi::c_void;

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

impl IdentifierStaticTrait for OrderingTerm {
    fn get_type() -> i32 {
        CPPType::OrderingTerm as i32
    }
}

impl OrderingTerm {
    pub fn new(expression: ExpressionOperable) -> Self {
        let identifier = Identifier::new_with_obj(expression.get_cpp_obj());
        OrderingTerm { identifier }
    }
}
