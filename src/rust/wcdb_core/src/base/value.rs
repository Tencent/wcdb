use crate::winq::column_type::ColumnType;
use std::fmt;
use std::fmt::Display;
use std::hash::{Hash, Hasher};

#[derive(Debug, Clone)]
pub struct Value {
    value: Option<ValueType>,
}

#[derive(Debug, Clone)]
pub enum ValueType {
    Long(i64),
    Double(f64),
    String(String),
    Blob(Vec<u8>),
}

impl Value {
    pub fn new() -> Self {
        Value { value: None }
    }

    pub fn from_bool(value: bool) -> Value {
        Value {
            value: Some(ValueType::Long(if value { 1 } else { 0 })),
        }
    }

    pub fn from_byte(value: i8) -> Value {
        Value {
            value: Some(ValueType::Long(value as i64)),
        }
    }
    pub fn from_i32(value: i32) -> Value {
        Value {
            value: Some(ValueType::Long(value as i64)),
        }
    }
    pub fn from_long(value: i64) -> Value {
        Value {
            value: Some(ValueType::Long(value)),
        }
    }
    pub fn from_f32(value: f32) -> Value {
        Value {
            value: Some(ValueType::Double(value as f64)),
        }
    }
    pub fn from_f64(value: f64) -> Value {
        Value {
            value: Some(ValueType::Double(value)),
        }
    }
    pub fn from_string(value: Option<String>) -> Value {
        Value {
            value: value.map(ValueType::String),
        }
    }
    pub fn from_blob(value: Option<Vec<u8>>) -> Value {
        Value {
            value: value.map(ValueType::Blob),
        }
    }

    pub fn from_any(value: Option<Box<dyn std::any::Any>>) -> Self {
        if let Some(value) = value {
            if let Some(val) = value.downcast_ref::<i64>() {
                return Value::from_long(*val);
            } else if let Some(val) = value.downcast_ref::<f64>() {
                return Value::from_f64(*val);
            } else if let Some(val) = value.downcast_ref::<String>() {
                return Value::from_string(Some(val.clone()));
            } else if let Some(val) = value.downcast_ref::<Vec<u8>>() {
                return Value::from_blob(Some(val.clone()));
            }
        }
        Value::new()
    }

    pub fn get_type(&self) -> ColumnType {
        match self.value {
            None => ColumnType::Null,
            Some(ValueType::Long(_)) => ColumnType::Integer,
            Some(ValueType::Double(_)) => ColumnType::Float,
            Some(ValueType::String(_)) => ColumnType::Text,
            Some(ValueType::Blob(_)) => ColumnType::BLOB,
        }
    }

    pub fn get_bool(&self) -> bool {
        match self.value {
            None => false,
            Some(ValueType::Long(val)) => val != 0,
            _ => false,
        }
    }

    pub fn get_byte(&self) -> i8 {
        self.get_long() as i8
    }
    pub fn get_short(&self) -> i16 {
        self.get_long() as i16
    }

    pub fn get_i32(&self) -> i32 {
        self.get_long() as i32
    }

    pub fn get_long(&self) -> i64 {
        match &self.value {
            None => 0,
            Some(ValueType::Long(val)) => *val,
            Some(ValueType::Double(val)) => *val as i64,
            Some(ValueType::String(val)) => val.parse().unwrap_or(0),
            _ => 0,
        }
    }

    pub fn get_float(&self) -> f32 {
        self.get_long() as f32
    }
    pub fn get_double(&self) -> f64 {
        match &self.value {
            None => 0.0,
            Some(ValueType::Double(val)) => *val,
            Some(ValueType::Long(val)) => *val as f64,
            Some(ValueType::String(val)) => val.parse().unwrap_or(0.0),
            _ => 0.0,
        }
    }

    pub fn get_text(&self) -> Option<String> {
        match &self.value {
            None => Option::None,
            Some(ValueType::String(val)) => Some(val.clone()),
            Some(ValueType::Blob(val)) => {
                Some(String::from_utf8(val.to_vec()).unwrap().to_string())
            }
            _ => self.value.as_ref().map(|v| v.to_string()),
        }
    }

    pub fn get_blob(&self) -> Option<Vec<u8>> {
        match &self.value {
            None => Some(Vec::new()),
            Some(ValueType::Blob(val)) => Some(val.clone()),
            _ => self.value.as_ref().map(|v| v.to_string().into_bytes()),
        }
    }
}

impl PartialEq for Value {
    fn eq(&self, other: &Self) -> bool {
        match (self.get_type(), other.get_type()) {
            (ColumnType::Null, ColumnType::Null) => true,
            (ColumnType::Integer, ColumnType::Integer) => self.get_long() == other.get_long(),
            (ColumnType::Float, ColumnType::Float) => self.get_double() == other.get_double(),
            (ColumnType::Text, ColumnType::Text) => self.get_text() == other.get_text(),
            (ColumnType::BLOB, ColumnType::BLOB) => self.get_blob() == other.get_blob(),
            _ => false,
        }
    }
}

impl Eq for Value {}

impl Display for Value {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match &self.value {
            Some(ValueType::Long(val)) => write!(f, "{}", val),
            Some(ValueType::Double(val)) => write!(f, "{}", val),
            Some(ValueType::String(val)) => write!(f, "{}", val),
            Some(ValueType::Blob(val)) => write!(f, "{:?}", val),
            None => write!(f, "NULL"),
        }
    }
}

impl Hash for ValueType {
    fn hash<H: Hasher>(&self, state: &mut H) {
        match self {
            ValueType::Long(val) => val.hash(state),
            ValueType::Double(val) => val.to_bits().hash(state),
            ValueType::String(val) => val.hash(state),
            ValueType::Blob(val) => val.hash(state),
        }
    }
}

impl fmt::Display for ValueType {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            ValueType::Long(val) => write!(f, "{}", val),
            ValueType::Double(val) => write!(f, "{}", val),
            ValueType::String(val) => write!(f, "{}", val),
            ValueType::Blob(val) => write!(f, "{:?}", val),
        }
    }
}
