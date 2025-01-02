use crate::winq::column_type::ColumnType;
use std::fmt::Display;
use std::hash::Hash;

#[derive(Debug, Clone)]
pub enum ValueObject {
    None,
    Long(i64),
    Double(f64),
    String(String),
    BLOB(Vec<u8>),
}

#[derive(Debug, Clone)]
pub struct Value {
    value: ValueObject,
}

impl From<bool> for Value {
    fn from(value: bool) -> Self {
        Self {
            value: ValueObject::Long(if value { 1 } else { 0 }),
        }
    }
}

impl From<i32> for Value {
    fn from(value: i32) -> Self {
        Self {
            value: ValueObject::Long(value as i64),
        }
    }
}

impl From<&str> for Value {
    fn from(value: &str) -> Self {
        Self {
            value: ValueObject::String(value.to_string()),
        }
    }
}

impl Value {
    pub fn new() -> Self {
        Value {
            value: ValueObject::None,
        }
    }

    pub fn get_type(&self) -> ColumnType {
        match self.value {
            ValueObject::None => ColumnType::Null,
            ValueObject::Long(_) => ColumnType::Integer,
            ValueObject::String(_) => ColumnType::Text,
            ValueObject::BLOB(_) => ColumnType::BLOB,
            ValueObject::Double(_) => ColumnType::Float,
        }
    }
}
