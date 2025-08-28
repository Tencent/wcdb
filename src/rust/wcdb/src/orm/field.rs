use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::orm::table_binding::TableBinding;
use crate::winq::column::Column;
use crate::winq::expression::Expression;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::expression_operable::{ExpressionOperableTrait, OperateParam};
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::indexed_column_convertible::IndexedColumnConvertibleTrait;
use crate::winq::result_column_convertible_trait::ResultColumnConvertibleTrait;
use std::ffi::c_void;

pub struct Field<U> {
    column: Column,
    name: String,
    binding: *const dyn TableBinding<U>,
    field_id: usize,
    is_auto_increment: bool,
    is_primary_key: bool,
}

impl<U> CppObjectTrait for Field<U> {
    fn set_cpp_obj(&mut self, _cpp_obj: *mut c_void) {
        self.column.set_cpp_obj(_cpp_obj);
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.column.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.column.release_cpp_object();
    }
}

impl<U> CppObjectConvertibleTrait for Field<U> {
    fn as_cpp_object(&self) -> &CppObject {
        self.column.as_cpp_object()
    }
}

impl<U> IdentifierConvertibleTrait for Field<U> {
    fn as_identifier(&self) -> &Identifier {
        self.column.as_identifier()
    }
}

impl<U> IdentifierTrait for Field<U> {
    fn get_type(&self) -> CPPType {
        self.column.get_type()
    }

    fn get_description(&self) -> String {
        self.column.get_description()
    }
}

impl<U> ExpressionOperableTrait for Field<U> {
    fn is_null(&self) -> Expression {
        self.column.is_null()
    }

    fn not_null(&self) -> Expression {
        self.column.not_null()
    }

    fn or<T: ExpressionConvertibleTrait>(&self, operand: T) -> Expression {
        self.column.or(operand)
    }

    fn and<T: ExpressionConvertibleTrait>(&self, operand: T) -> Expression {
        self.column.and(operand)
    }

    fn multiply<T: OperateParam>(&self, operand: T) -> Expression {
        self.column.multiply(operand)
    }

    fn divide<T: OperateParam>(&self, operand: T) -> Expression {
        self.column.divide(operand)
    }

    fn modulo<T: OperateParam>(&self, operand: T) -> Expression {
        self.column.modulo(operand)
    }

    fn add<T: OperateParam>(&self, operand: T) -> Expression {
        self.column.add(operand)
    }

    fn minus<T: OperateParam>(&self, operand: T) -> Expression {
        self.column.minus(operand)
    }

    fn left_shift<T: OperateParam>(&self, operand: T) -> Expression {
        self.column.left_shift(operand)
    }

    fn right_shift<T: OperateParam>(&self, operand: T) -> Expression {
        self.column.right_shift(operand)
    }

    fn bit_and<T: OperateParam>(&self, operand: T) -> Expression {
        self.column.bit_and(operand)
    }

    fn bit_or<T: OperateParam>(&self, operand: T) -> Expression {
        self.column.bit_or(operand)
    }

    fn lt<T: OperateParam>(&self, operand: T) -> Expression {
        self.column.lt(operand)
    }

    fn le<T: OperateParam>(&self, operand: T) -> Expression {
        self.column.le(operand)
    }

    fn gt<T: OperateParam>(&self, operand: T) -> Expression {
        self.column.gt(operand)
    }

    fn ge<T: OperateParam>(&self, operand: T) -> Expression {
        self.column.ge(operand)
    }

    fn eq<T: OperateParam>(&self, operand: T) -> Expression {
        self.column.eq(operand)
    }

    fn not_eq<T: OperateParam>(&self, operand: T) -> Expression {
        self.column.not_eq(operand)
    }

    fn concat<T: OperateParam>(&self, operand: T) -> Expression {
        self.column.concat(operand)
    }

    fn between<T: OperateParam>(&self, begin: T, end: T) -> Expression {
        self.column.between(begin, end)
    }

    fn not_between<T: OperateParam>(&self, begin: T, end: T) -> Expression {
        self.column.not_between(begin, end)
    }

    fn r#in<T: OperateParam>(&self, operands: &[T]) -> Expression {
        self.column.r#in(operands)
    }

    fn not_in<T: OperateParam>(&self, operands: &[T]) -> Expression {
        self.column.not_in(operands)
    }

    fn in_table(&self, table: &str) -> Expression {
        self.column.in_table(table)
    }

    fn not_in_table(&self, table: &str) -> Expression {
        self.column.not_in_table(table)
    }

    fn collate(&self, collation: &str) -> Expression {
        self.column.collate(collation)
    }

    fn like(&self, content: &str) -> Expression {
        self.column.like(content)
    }

    fn not_like(&self, content: &str) -> Expression {
        self.column.not_like(content)
    }

    fn glob(&self, content: &str) -> Expression {
        self.column.glob(content)
    }

    fn not_glob(&self, content: &str) -> Expression {
        self.column.not_glob(content)
    }

    fn r#match(&self, content: &str) -> Expression {
        self.column.r#match(content)
    }

    fn not_match(&self, content: &str) -> Expression {
        self.column.not_match(content)
    }

    fn regexp(&self, content: &str) -> Expression {
        self.column.regexp(content)
    }

    fn not_regexp(&self, content: &str) -> Expression {
        self.column.not_regexp(content)
    }

    fn is(&self, operand: bool) -> Expression {
        self.column.is(operand)
    }

    fn is_not(&self, operand: bool) -> Expression {
        self.column.is_not(operand)
    }

    fn avg(&self) -> Expression {
        self.column.avg()
    }

    fn count(&self) -> Expression {
        self.column.count()
    }

    fn group_concat(&self) -> Expression {
        self.column.group_concat()
    }

    fn group_concat_string(&self, separator: &str) -> Expression {
        self.column.group_concat_string(separator)
    }

    fn max(&self) -> Expression {
        self.column.max()
    }

    fn min(&self) -> Expression {
        self.column.min()
    }

    fn sum(&self) -> Expression {
        self.column.sum()
    }

    fn total(&self) -> Expression {
        self.column.total()
    }

    fn abs(&self) -> Expression {
        self.column.abs()
    }

    fn hex(&self) -> Expression {
        self.column.hex()
    }

    fn length(&self) -> Expression {
        self.column.length()
    }

    fn lower(&self) -> Expression {
        self.column.lower()
    }

    fn upper(&self) -> Expression {
        self.column.upper()
    }

    fn round(&self) -> Expression {
        self.column.round()
    }

    fn match_info(&self) -> Expression {
        self.column.match_info()
    }

    fn offsets(&self) -> Expression {
        self.column.offsets()
    }

    fn snippet(&self) -> Expression {
        self.column.snippet()
    }

    fn bm25(&self) -> Expression {
        self.column.bm25()
    }

    fn highlight(&self) -> Expression {
        self.column.highlight()
    }

    fn substring_match_info(&self) -> Expression {
        self.column.substring_match_info()
    }
}

impl<U> IndexedColumnConvertibleTrait for Field<U> {}

impl<U> ResultColumnConvertibleTrait for Field<U> {}

impl<U> Field<U> {
    pub fn new(
        name: &str,
        binding: *const dyn TableBinding<U>,
        field_id: usize,
        is_auto_increment: bool,
        is_primary_key: bool,
    ) -> Field<U> {
        let bind = unsafe { &*binding };
        Field {
            column: Column::new(name, Some(bind.base_binding().get_base_binding())),
            name: name.to_string(),
            binding,
            field_id,
            is_auto_increment,
            is_primary_key,
        }
    }

    pub fn get_column(&self) -> &Column {
        &self.column
    }

    pub fn get_name(&self) -> &str {
        &self.name
    }

    pub fn get_field_id(&self) -> usize {
        self.field_id
    }

    pub fn get_table_binding(&self) -> &dyn TableBinding<U> {
        assert!(!self.binding.is_null());
        unsafe { &*self.binding }
    }

    pub fn is_auto_increment(&self) -> bool {
        self.is_auto_increment
    }

    pub fn get_binding_from_field(field: &Field<U>) -> &dyn TableBinding<U> {
        field.get_table_binding()
    }

    pub fn get_binding_from_fields<'a>(fields: &Vec<&'a Field<U>>) -> &'a dyn TableBinding<U> {
        assert!(!fields.is_empty());
        let field = fields[0];
        Self::get_binding_from_field(field)
    }

    pub fn is_primary_key(&self) -> bool {
        self.is_primary_key
    }
}
