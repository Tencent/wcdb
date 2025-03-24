use crate::base::value::Value;
use crate::winq::identifier::Identifier;
use crate::winq::multi_type_array::ObjectType::String;
use std::any::Any;

#[repr(i32)]
pub enum ObjectType {
    Null,
    Bool,
    Char,
    Byte,
    Short,
    Int,
    Long = 6,
    Float,
    Double,
    String,
    Identifier,
    Value,
    Unknown,
}

pub struct MultiTypeArray {}

impl MultiTypeArray {
    pub fn new() -> Self {
        MultiTypeArray {}
    }

    pub fn task(&self) {}

    pub fn get_object_type(val: Box<dyn Any>) -> ObjectType {
        if val.is::<Identifier>() {
            return ObjectType::Identifier;
        } else if val.is::<&str>() {
            return ObjectType::String;
        } else if val.is::<i32>() {
            return ObjectType::Int;
        } else if val.is::<f32>() {
            return ObjectType::Float;
        } else if val.is::<f64>() {
            return ObjectType::Double;
        } else if val.is::<bool>() {
            return ObjectType::Bool;
        } else if val.is::<i16>() {
            return ObjectType::Short;
        } else if val.is::<i64>() {
            return ObjectType::Long;
        } else if val.is::<char>() {
            return ObjectType::Char;
        } else if val.is::<i8>() {
            return ObjectType::Byte;
        } else if val.is::<Value>() {
            return ObjectType::Value;
        }
        return ObjectType::Unknown;
    }
}
