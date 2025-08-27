use crate::winq::identifier::CPPType;

#[derive(PartialEq)]
pub enum ColumnType {
    Null = 0,
    Integer = 1,
    Float = 2,
    Text = 3,
    BLOB = 4,
}

impl ColumnType {
    pub fn cpp_type(&self) -> CPPType {
        match self {
            ColumnType::Null => CPPType::Null,
            ColumnType::Integer => CPPType::Int,
            ColumnType::Float => CPPType::Double,
            ColumnType::Text => CPPType::String,
            ColumnType::BLOB => CPPType::BindParameter,
        }
    }

    pub fn value_of(value: i32) -> Self {
        match value {
            1 => ColumnType::Integer,
            2 => ColumnType::Float,
            3 => ColumnType::Text,
            4 => ColumnType::BLOB,
            _ => ColumnType::Null,
        }
    }
}
