use crate::base::cpp_object::CppObject;
use crate::base::value::Value;
use crate::winq::column_type::ColumnType;
use crate::winq::identifier::{CPPType, Identifier};
use crate::winq::object::Object;
use std::any::Any;
use std::ffi::c_double;

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

pub struct MultiTypeArray {
    pub(crate) types: Vec<i32>,
    pub(crate) long_values: Vec<i64>,
    pub(crate) double_values: Vec<c_double>,
    pub(crate) string_values: Option<Vec<String>>,
}

impl MultiTypeArray {
    pub fn new_with_objects(values: &Vec<Object>) -> Self {
        let value_count = values.len();

        let mut types: Vec<i32> = vec![0; value_count];
        let mut long_values: Vec<i64> = vec![0; value_count];
        let mut double_values = vec![0.0; value_count];
        let mut string_values = vec![String::new(); value_count];

        let mut long_index = 0;
        let mut double_index = 0;
        let mut string_index = 0;

        for (i, obj) in values.iter().enumerate() {
            match obj {
                Object::Null => {
                    types[i] = CPPType::Null as i32;
                    long_index += 1;
                }
                Object::Bool(b) => {
                    types[i] = CPPType::Bool as i32;
                    long_values[long_index] = if *b { 1 } else { 0 } as i64;
                    long_index += 1;
                }
                Object::Byte(b) => {
                    types[i] = CPPType::Int as i32;
                    long_values[long_index] = *b as i64;
                    long_index += 1;
                }
                Object::Char(c) => {
                    types[i] = CPPType::Int as i32;
                    long_values[long_index] = *c as i64;
                    long_index += 1;
                }
                Object::Short(s) => {
                    types[i] = CPPType::Int as i32;
                    long_values[long_index] = *s as i64;
                    long_index += 1;
                }
                Object::Int(int) => {
                    types[i] = CPPType::Int as i32;
                    long_values[long_index] = *int as i64;
                    long_index += 1;
                }
                Object::Long(l) => {
                    types[i] = CPPType::Int as i32;
                    long_values[long_index] = *l as i64;
                    long_index += 1;
                }
                Object::Float(f) => {
                    types[i] = CPPType::Double as i32;
                    double_values[double_index] = *f as c_double;
                    double_index += 1;
                }
                Object::Double(d) => {
                    types[i] = CPPType::Double as i32;
                    double_values[double_index] = *d as c_double;
                    double_index += 1;
                }
                Object::String(s) => {
                    types[i] = CPPType::String as i32;
                    string_values[string_index] = s.clone();
                    string_index += 1;
                }
                Object::Identifier(identifier) => {
                    types[i] = Identifier::get_cpp_type(identifier) as i32;
                    long_values[long_index] = CppObject::get(identifier) as i64;
                    long_index += 1;
                }
                Object::Value(value_obj) => match value_obj.get_type() {
                    ColumnType::Null => {
                        types[i] = CPPType::Null as i32;
                        long_index += 1;
                    }
                    ColumnType::Integer => {
                        types[i] = CPPType::Int as i32;
                        long_values[long_index] = value_obj.get_i64() as i64;
                        long_index += 1;
                    }
                    ColumnType::Float => {
                        types[i] = CPPType::Double as i32;
                        double_values[double_index] = value_obj.get_f64() as c_double;
                        double_index += 1;
                    }
                    ColumnType::Text => {
                        types[i] = CPPType::String as i32;
                        string_values[string_index] = value_obj.get_text();
                        string_index += 1;
                    }
                    _ => {}
                },
            }
        }

        let string_values = if string_values.len() as f64 * 0.75 > string_index as f64 {
            if string_index == 0 {
                None
            } else {
                Some(string_values[0..string_index].to_vec())
            }
        } else {
            Some(string_values)
        };

        MultiTypeArray {
            types,
            long_values,
            double_values,
            string_values,
        }
    }

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
        ObjectType::Unknown
    }

    pub fn types(&self) -> &Vec<i32> {
        &self.types
    }

    pub fn long_values(&self) -> &Vec<i64> {
        &self.long_values
    }

    pub fn double_values(&self) -> &Vec<c_double> {
        &self.double_values
    }

    pub fn string_values(&self) -> &Option<Vec<String>> {
        &self.string_values
    }
}
