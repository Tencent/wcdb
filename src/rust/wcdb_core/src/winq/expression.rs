use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::winq::column::Column;
use crate::winq::expression_operable::ExpressionOperable;
use crate::winq::identifier::{Identifier, IdentifierTrait};
use crate::winq::literal_value::LiteralValue;
use crate::winq::statement_select::StatementSelect;
use std::ffi::c_void;

extern "C" {
    pub fn WCDBRustExpression_create(value_type: i32, cpp_obj: *mut c_void) -> *mut c_void;
    // pub fn WCDBRustExpression_argument(
    //     cpp_obj: *mut c_void,
    //     type_i: c_int,
    //     int_value: c_long,
    //     double_value: c_double,
    //     string_value: *const c_char,
    // ) -> c_void;
}

#[derive(Debug)]
pub struct Expression {
    expression_operable: ExpressionOperable,
}

impl CppObjectTrait for Expression {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.expression_operable.set_cpp_obj(cpp_obj);
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.expression_operable.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.expression_operable.release_cpp_object();
    }
}

impl IdentifierTrait for Expression {
    fn get_description(&self) -> String {
        self.expression_operable.get_description()
    }
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

    pub fn new_with_column(column: Column) -> Self {
        let cpp_obj = unsafe {
            WCDBRustExpression_create(Identifier::get_cpp_type(&column), CppObject::get(&column))
        };
        Expression {
            expression_operable: ExpressionOperable::new_with_obj(cpp_obj),
        }
    }

    pub fn new_with_statement_select(select: StatementSelect) -> Self {
        let cpp_obj = unsafe {
            WCDBRustExpression_create(Identifier::get_cpp_type(&select), CppObject::get(&select))
        };
        Expression {
            expression_operable: ExpressionOperable::new_with_obj(cpp_obj),
        }
    }

    // pub fn argument_i64(self, value: i64) -> Self {
    //     self.argument(CPPType::Int, value, 0.0, "".to_string());
    //     self
    // }
    //
    // // todo qixinbing: 怎么用？
    // fn argument(&self, type_i: CPPType, int_value: i64, double_value: f64, string_value: String) {
    //     let c_str = CString::new(string_value).unwrap_or_default();
    //     unsafe {
    //         WCDBRustExpression_argument(
    //             self.get_cpp_obj(),
    //             type_i as i32,
    //             int_value,
    //             double_value,
    //             c_str.as_ptr(),
    //         );
    //     }
    // }

    pub fn eq_long(mut self, operand: i64) -> Self {
        self.expression_operable = self.expression_operable.eq_long(operand);
        self
    }

    pub(crate) fn get_expression_operable(&self) -> &ExpressionOperable {
        &self.expression_operable
    }
}
