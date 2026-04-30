use crate::winq::column_type::ColumnType;
use std::str::from_utf8;

#[derive(Clone, Debug)]
pub enum ValueObject {
    None,
    Int(i64),
    Double(f64),
    String(String),
    BLOB(Vec<u8>),
    // todo qixinbing 处理 struct Value
}

impl From<bool> for ValueObject {
    fn from(value: bool) -> Self {
        ValueObject::Int(if value { 1 } else { 0 })
    }
}

impl From<i8> for ValueObject {
    fn from(value: i8) -> Self {
        ValueObject::Int(value as i64)
    }
}

impl From<i16> for ValueObject {
    fn from(value: i16) -> Self {
        ValueObject::Int(value as i64)
    }
}

impl From<i32> for ValueObject {
    fn from(value: i32) -> Self {
        ValueObject::Int(value as i64)
    }
}

impl From<i64> for ValueObject {
    fn from(value: i64) -> Self {
        ValueObject::Int(value)
    }
}

impl From<f32> for ValueObject {
    fn from(value: f32) -> Self {
        ValueObject::Double(value as f64)
    }
}

impl From<f64> for ValueObject {
    fn from(value: f64) -> Self {
        ValueObject::Double(value)
    }
}

impl From<&str> for ValueObject {
    fn from(value: &str) -> Self {
        ValueObject::String(value.to_string())
    }
}

impl From<String> for ValueObject {
    fn from(value: String) -> Self {
        ValueObject::String(value)
    }
}

impl From<Vec<u8>> for ValueObject {
    fn from(value: Vec<u8>) -> Self {
        ValueObject::BLOB(value)
    }
}

#[derive(Debug, Clone)]
pub struct Value {
    value: ValueObject,
}

impl Value {
    // todo qixinbing 是否支持 None？
    pub fn default() -> Self {
        Value {
            value: ValueObject::None,
        }
    }
    pub fn new<T>(value: T) -> Self
    where
        T: Into<ValueObject>,
    {
        Value {
            value: value.into(),
        }
    }

    pub fn get_type(&self) -> ColumnType {
        match &self.value {
            ValueObject::None => ColumnType::Null,
            ValueObject::Int(_) => ColumnType::Integer,
            ValueObject::Double(_) => ColumnType::Float,
            ValueObject::String(_) => ColumnType::Text,
            ValueObject::BLOB(_) => ColumnType::BLOB,
        }
    }

    pub fn get_bool(&self) -> bool {
        self.get_i64() != 0
    }

    pub fn get_i8(&self) -> i8 {
        self.get_i64() as i8
    }

    pub fn get_i16(&self) -> i16 {
        self.get_i64() as i16
    }

    pub fn get_i32(&self) -> i32 {
        self.get_i64() as i32
    }

    pub fn get_i64(&self) -> i64 {
        match &self.value {
            ValueObject::Int(val) => *val,
            ValueObject::Double(val) => (*val).round() as i64,
            ValueObject::String(val) => val.parse::<i64>().unwrap_or(0),
            _ => 0,
        }
    }

    pub fn get_f32(&self) -> f32 {
        self.get_f64() as f32
    }

    pub fn get_f64(&self) -> f64 {
        match &self.value {
            ValueObject::Double(val) => *val,
            ValueObject::Int(val) => (*val) as f64,
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
