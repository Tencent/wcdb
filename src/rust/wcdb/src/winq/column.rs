use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::column_def::{ColumnDef, ColumnDefParam};
use crate::winq::column_type::ColumnType;
use crate::winq::expression::Expression;
use crate::winq::expression_convertible::ExpressionConvertibleTrait;
use crate::winq::expression_operable::{ExpressionOperable, ExpressionOperableTrait, OperateParam};
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::indexed_column_convertible::IndexedColumnConvertibleTrait;
use crate::winq::ordering_term::{Order, OrderingTerm};
use crate::winq::result_column::ResultColumn;
use crate::winq::result_column_convertible_trait::ResultColumnConvertibleTrait;
use crate::winq::schema::Schema;
use std::ffi::{c_char, c_int, c_void};

extern "C" {
    fn WCDBRustColumn_createWithName(name: *const c_char, binding: *mut c_void) -> *mut c_void;

    // copy()

    fn WCDBRustColumn_inTable(column: *mut c_void, table: *const c_char);

    fn WCDBRustColumn_ofSchema(
        column: *mut c_void,
        r#type: c_int,
        schema: *mut c_void,
        schema_name: *const c_char,
    );

    fn WCDBRustColumn_createAll() -> *mut c_void;

    fn WCDBRustColumn_createRowId() -> *mut c_void;

    fn WCDBRustColumn_configAlias(cpp_obj: *mut c_void, alias: *const c_char) -> *mut c_void;
}

pub struct Column {
    expression_operable: ExpressionOperable,
}

impl CppObjectTrait for Column {
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

impl CppObjectConvertibleTrait for Column {
    fn as_cpp_object(&self) -> &CppObject {
        self.expression_operable.as_cpp_object()
    }
}

impl IdentifierTrait for Column {
    fn get_type(&self) -> CPPType {
        self.expression_operable.get_type()
    }

    fn get_description(&self) -> String {
        self.expression_operable.get_description()
    }
}

impl IdentifierConvertibleTrait for Column {
    fn as_identifier(&self) -> &Identifier {
        self.expression_operable.as_identifier()
    }
}

impl ExpressionConvertibleTrait for Column {}

impl ExpressionOperableTrait for Column {
    fn is_null(&self) -> Expression {
        self.expression_operable.is_null()
    }

    fn not_null(&self) -> Expression {
        self.expression_operable.not_null()
    }

    fn or<T: ExpressionConvertibleTrait>(&self, operand: T) -> Expression {
        self.expression_operable.or(operand)
    }

    fn and<T: ExpressionConvertibleTrait>(&self, operand: T) -> Expression {
        self.expression_operable.and(operand)
    }

    fn multiply<T: OperateParam>(&self, operand: T) -> Expression {
        self.expression_operable.multiply(operand)
    }

    fn divide<T: OperateParam>(&self, operand: T) -> Expression {
        self.expression_operable.divide(operand)
    }

    fn r#mod<T: OperateParam>(&self, operand: T) -> Expression {
        self.expression_operable.r#mod(operand)
    }

    fn add<T: OperateParam>(&self, operand: T) -> Expression {
        self.expression_operable.add(operand)
    }

    fn minus<T: OperateParam>(&self, operand: T) -> Expression {
        self.expression_operable.minus(operand)
    }

    fn left_shift<T: OperateParam>(&self, operand: T) -> Expression {
        self.expression_operable.left_shift(operand)
    }

    fn right_shift<T: OperateParam>(&self, operand: T) -> Expression {
        self.expression_operable.right_shift(operand)
    }

    fn bit_and<T: OperateParam>(&self, operand: T) -> Expression {
        self.expression_operable.bit_and(operand)
    }

    fn bit_or<T: OperateParam>(&self, operand: T) -> Expression {
        self.expression_operable.bit_or(operand)
    }

    fn lt<T: OperateParam>(&self, operand: T) -> Expression {
        self.expression_operable.lt(operand)
    }

    fn le<T: OperateParam>(&self, operand: T) -> Expression {
        self.expression_operable.le(operand)
    }

    fn gt<T: OperateParam>(&self, operand: T) -> Expression {
        self.expression_operable.gt(operand)
    }

    fn ge<T: OperateParam>(&self, operand: T) -> Expression {
        self.expression_operable.ge(operand)
    }

    fn eq<T: OperateParam>(&self, operand: T) -> Expression {
        self.expression_operable.eq(operand)
    }

    fn not_eq<T: OperateParam>(&self, operand: T) -> Expression {
        self.expression_operable.not_eq(operand)
    }

    fn concat<T: OperateParam>(&self, operand: T) -> Expression {
        self.expression_operable.concat(operand)
    }

    fn between<T: OperateParam>(&self, begin: T, end: T) -> Expression {
        self.expression_operable.between(begin, end)
    }

    fn not_between<T: OperateParam>(&self, begin: T, end: T) -> Expression {
        self.expression_operable.not_between(begin, end)
    }

    fn r#in<T: OperateParam>(&self, operands: &[T]) -> Expression {
        self.expression_operable.r#in(operands)
    }

    fn not_in<T: OperateParam>(&self, operands: &[T]) -> Expression {
        self.expression_operable.not_in(operands)
    }

    fn in_table(&self, table: &str) -> Expression {
        self.expression_operable.in_table(table)
    }

    fn not_in_table(&self, table: &str) -> Expression {
        self.expression_operable.not_in_table(table)
    }

    fn collate(&self, collation: &str) -> Expression {
        self.expression_operable.collate(collation)
    }

    fn like(&self, content: &str) -> Expression {
        self.expression_operable.like(content)
    }

    fn not_like(&self, content: &str) -> Expression {
        self.expression_operable.not_like(content)
    }

    fn glob(&self, content: &str) -> Expression {
        self.expression_operable.glob(content)
    }

    fn not_glob(&self, content: &str) -> Expression {
        self.expression_operable.not_glob(content)
    }

    fn r#match(&self, content: &str) -> Expression {
        self.expression_operable.r#match(content)
    }

    fn not_match(&self, content: &str) -> Expression {
        self.expression_operable.not_match(content)
    }

    fn regexp(&self, content: &str) -> Expression {
        self.expression_operable.regexp(content)
    }

    fn not_regexp(&self, content: &str) -> Expression {
        self.expression_operable.not_regexp(content)
    }

    fn is(&self, operand: bool) -> Expression {
        self.expression_operable.is(operand)
    }

    fn is_not(&self, operand: bool) -> Expression {
        self.expression_operable.is_not(operand)
    }

    fn avg(&self) -> Expression {
        self.expression_operable.avg()
    }

    fn count(&self) -> Expression {
        self.expression_operable.count()
    }

    fn group_concat(&self) -> Expression {
        self.expression_operable.group_concat()
    }

    fn group_concat_string(&self, separator: &str) -> Expression {
        self.expression_operable.group_concat_string(separator)
    }

    fn max(&self) -> Expression {
        self.expression_operable.max()
    }

    fn min(&self) -> Expression {
        self.expression_operable.min()
    }

    fn sum(&self) -> Expression {
        self.expression_operable.sum()
    }

    fn total(&self) -> Expression {
        self.expression_operable.total()
    }

    fn abs(&self) -> Expression {
        self.expression_operable.abs()
    }

    fn hex(&self) -> Expression {
        self.expression_operable.hex()
    }

    fn length(&self) -> Expression {
        self.expression_operable.length()
    }

    fn lower(&self) -> Expression {
        self.expression_operable.lower()
    }

    fn upper(&self) -> Expression {
        self.expression_operable.upper()
    }

    fn round(&self) -> Expression {
        self.expression_operable.round()
    }

    fn match_info(&self) -> Expression {
        self.expression_operable.match_info()
    }

    fn offsets(&self) -> Expression {
        self.expression_operable.offsets()
    }

    fn snippet(&self) -> Expression {
        self.expression_operable.snippet()
    }

    fn bm25(&self) -> Expression {
        self.expression_operable.bm25()
    }

    fn highlight(&self) -> Expression {
        self.expression_operable.highlight()
    }

    fn substring_match_info(&self) -> Expression {
        self.expression_operable.substring_match_info()
    }
}

impl IndexedColumnConvertibleTrait for Column {}

impl ResultColumnConvertibleTrait for Column {}

pub trait ColumnOfParam {
    fn call_of_schema(&self, column: &Column);
}

impl ColumnOfParam for Schema {
    fn call_of_schema(&self, column: &Column) {
        unsafe {
            WCDBRustColumn_ofSchema(
                column.get_cpp_obj(),
                Identifier::get_cpp_type(self) as c_int,
                CppObject::get(self),
                std::ptr::null_mut(),
            )
        }
    }
}

impl ColumnOfParam for &str {
    fn call_of_schema(&self, column: &Column) {
        let c_name = self.to_cstring();
        unsafe {
            WCDBRustColumn_ofSchema(
                column.get_cpp_obj(),
                CPPType::String as c_int,
                std::ptr::null_mut(),
                c_name.as_ptr(),
            )
        }
    }
}

impl Column {
    pub fn new(name: &str, table_binding_opt: Option<*mut c_void>) -> Self {
        let c_name = name.to_cstring();
        let cpp_obj = match table_binding_opt {
            Some(table_binding) => unsafe {
                WCDBRustColumn_createWithName(c_name.as_ptr(), table_binding)
            },
            None => unsafe { WCDBRustColumn_createWithName(c_name.as_ptr(), std::ptr::null_mut()) },
        };
        Self {
            expression_operable: ExpressionOperable::new(CPPType::Column, Some(cpp_obj)),
        }
    }

    pub fn table(&self, table: &str) -> &Self {
        let c_table = table.to_cstring();
        unsafe { WCDBRustColumn_inTable(self.get_cpp_obj(), c_table.as_ptr()) };
        self
    }

    pub fn of<T: ColumnOfParam>(&self, param: T) -> &Self {
        param.call_of_schema(self);
        self
    }

    pub fn r#as(&self, alias: &str) -> ResultColumn {
        let c_alias = alias.to_cstring();
        let cpp_obj = unsafe { WCDBRustColumn_configAlias(self.get_cpp_obj(), c_alias.as_ptr()) };
        ResultColumn::new(cpp_obj)
    }

    pub fn all() -> Column {
        let cpp_obj = unsafe { WCDBRustColumn_createAll() };
        Column {
            expression_operable: ExpressionOperable::new(CPPType::Column, Some(cpp_obj)),
        }
    }

    pub fn row_id() -> Column {
        let cpp_obj = unsafe { WCDBRustColumn_createRowId() };
        Column {
            expression_operable: ExpressionOperable::new(CPPType::Column, Some(cpp_obj)),
        }
    }

    pub fn order(&self, order: Order) -> OrderingTerm {
        OrderingTerm::new(self).order(order)
    }

    pub fn as_def(&self, column_type: ColumnType) -> ColumnDef {
        ColumnDef::new(ColumnDefParam::Column(self, Some(column_type)))
    }
}
