use crate::base::cpp_object::CppObject;
use crate::winq::expression_operable::ExpressionOperable;
use crate::winq::identifier::Identifier;
use crate::winq::literal_value::LiteralValue;
use std::ffi::c_void;

extern "C" {
    pub fn WCDBRustExpression_create(value_type: i32, cpp_obj: *mut c_void) -> *mut c_void;
}

pub struct Expression {
    expression_operable: ExpressionOperable,
}

impl Expression {
    pub fn new() -> Self {
        Expression {
            expression_operable: ExpressionOperable::new(),
        }
    }

    pub fn new_with_literal_value(value: LiteralValue) -> Self {
        let cpp_obj = unsafe {
            WCDBRustExpression_create(Identifier::get_cpp_type(&value), CppObject::get(&value))
        };
        Expression {
            expression_operable: ExpressionOperable::new_with_obj(cpp_obj),
        }
    }
}
