use crate::base::value::Value;
use crate::winq::identifier::Identifier;

#[derive(Debug)]
pub enum Object {
    Null,
    Bool(bool),
    Byte(i8),
    Char(char),
    Short(i16),
    Int(i32),
    Long(i64),
    Float(f32),
    Double(f64),
    String(String),
    Identifier(Identifier),
    Value(Value),
}
