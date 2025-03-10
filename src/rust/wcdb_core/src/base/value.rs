use crate::winq::column_type::ColumnType;
use std::fmt::Display;
use std::hash::Hash;
use std::str::from_utf8;

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

    pub fn new_long(value: i64) -> Self {
        Value {
            value: ValueObject::Long(value),
        }
    }

    pub fn new_double(value: f64) -> Self {
        Value {
            value: ValueObject::Double(value),
        }
    }

    pub fn new_string(value: &str) -> Self {
        Value {
            value: ValueObject::String(value.to_string()),
        }
    }

    pub fn new_blob(value: Vec<u8>) -> Self {
        Value {
            value: ValueObject::BLOB(value),
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

    pub fn get_bool(&self) -> bool {
        self.get_long() != 0
    }

    pub fn get_byte(&self) -> i8 {
        self.get_long() as i8
    }

    pub fn get_short(&self) -> i16 {
        self.get_long() as i16
    }

    pub fn get_int(&self) -> i32 {
        self.get_long() as i32
    }

    pub fn get_long(&self) -> i64 {
        match &self.value {
            ValueObject::Long(val) => *val,
            ValueObject::Double(val) => (*val).round() as i64,
            ValueObject::String(val) => val.parse::<i64>().unwrap_or(0),
            _ => 0,
        }
    }

    pub fn get_float(&self) -> f32 {
        self.get_double() as f32
    }

    pub fn get_double(&self) -> f64 {
        match &self.value {
            ValueObject::Double(val) => *val,
            ValueObject::Long(val) => (*val) as f64,
            ValueObject::String(val) => val.parse::<f64>().unwrap_or(0.0),
            _ => 0.0,
        }
    }

    pub fn get_text(&self) -> String {
        match &self.value {
            ValueObject::String(val) => val.to_string(),
            ValueObject::BLOB(val) => from_utf8(&val).unwrap_or_default().to_string(),
            _ => "".to_string(),
        }
    }

    pub fn get_blob(&self) -> Vec<u8> {
        match &self.value {
            ValueObject::BLOB(val) => val.clone(),
            _ => {
                let string = self.get_text();
                string.as_bytes().to_vec()
            }
        }
    }
}
