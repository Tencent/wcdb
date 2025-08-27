use crate::winq::column_type::ColumnType;
use std::any::Any;

/// support : i8、i16、i32、i64、f32、f64、bool、String、&str
pub trait WCDBBasicTypes: 'static {
    fn get_value(&self) -> Self;

    fn get_bool(&self) -> bool;

    fn get_i64(&self) -> i64;

    fn get_f64(&self) -> f64;

    fn get_string(&self) -> String;

    fn get_type(&self) -> ColumnType;
}

impl WCDBBasicTypes for i8 {
    fn get_value(&self) -> i8 {
        let any_value = self as &dyn Any;
        match any_value.downcast_ref::<i8>() {
            None => 0,
            Some(value) => *value,
        }
    }

    fn get_bool(&self) -> bool {
        let value = self.get_value();
        if value == 0 {
            false
        } else {
            true
        }
    }

    fn get_i64(&self) -> i64 {
        let value = self.get_value();
        value as i64
    }

    fn get_f64(&self) -> f64 {
        let value = self.get_value();
        value as f64
    }

    fn get_string(&self) -> String {
        let value = self.get_value();
        format!("{}", value)
    }

    fn get_type(&self) -> ColumnType {
        ColumnType::Integer
    }
}

impl WCDBBasicTypes for i16 {
    fn get_value(&self) -> i16 {
        let any_value = self as &dyn Any;
        match any_value.downcast_ref::<i16>() {
            None => 0,
            Some(value) => *value,
        }
    }

    fn get_bool(&self) -> bool {
        let value = self.get_value();
        if value == 0 {
            false
        } else {
            true
        }
    }

    fn get_i64(&self) -> i64 {
        let value = self.get_value();
        value as i64
    }

    fn get_f64(&self) -> f64 {
        let value = self.get_value();
        value as f64
    }

    fn get_string(&self) -> String {
        let value = self.get_value();
        format!("{}", value)
    }

    fn get_type(&self) -> ColumnType {
        ColumnType::Integer
    }
}

impl WCDBBasicTypes for i32 {
    fn get_value(&self) -> i32 {
        let any_value = self as &dyn Any;
        match any_value.downcast_ref::<i32>() {
            None => 0,
            Some(value) => *value,
        }
    }

    fn get_bool(&self) -> bool {
        let value = self.get_value();
        if value == 0 {
            false
        } else {
            true
        }
    }

    fn get_i64(&self) -> i64 {
        let value = self.get_value();
        value as i64
    }

    fn get_f64(&self) -> f64 {
        let value = self.get_value();
        value as f64
    }

    fn get_string(&self) -> String {
        let value = self.get_value();
        format!("{}", value)
    }

    fn get_type(&self) -> ColumnType {
        ColumnType::Integer
    }
}

impl WCDBBasicTypes for i64 {
    fn get_value(&self) -> i64 {
        let any_value = self as &dyn Any;
        match any_value.downcast_ref::<i64>() {
            None => 0i64,
            Some(value) => *value,
        }
    }

    fn get_bool(&self) -> bool {
        let value = self.get_value();
        if value == 0 {
            false
        } else {
            true
        }
    }

    fn get_i64(&self) -> i64 {
        let value = self.get_value();
        value as i64
    }

    fn get_f64(&self) -> f64 {
        let value = self.get_value();
        value as f64
    }

    fn get_string(&self) -> String {
        let value = self.get_value();
        format!("{}", value)
    }

    fn get_type(&self) -> ColumnType {
        ColumnType::Integer
    }
}

impl WCDBBasicTypes for f32 {
    fn get_value(&self) -> f32 {
        let any_value = self as &dyn Any;
        match any_value.downcast_ref::<f32>() {
            None => 0f32,
            Some(value) => *value,
        }
    }

    fn get_bool(&self) -> bool {
        let value = self.get_value();
        if value == 0f32 {
            false
        } else {
            true
        }
    }

    fn get_i64(&self) -> i64 {
        let value = self.get_value();
        value as i64
    }

    fn get_f64(&self) -> f64 {
        let value = self.get_value();
        value as f64
    }

    fn get_string(&self) -> String {
        let value = self.get_value();
        format!("{}", value)
    }

    fn get_type(&self) -> ColumnType {
        ColumnType::Float
    }
}
impl WCDBBasicTypes for f64 {
    fn get_value(&self) -> f64 {
        let any_value = self as &dyn Any;
        match any_value.downcast_ref::<f64>() {
            None => 0f64,
            Some(value) => *value,
        }
    }

    fn get_bool(&self) -> bool {
        let value = self.get_value();
        if value == 0f64 {
            false
        } else {
            true
        }
    }

    fn get_i64(&self) -> i64 {
        let value = self.get_value();
        value as i64
    }

    fn get_f64(&self) -> f64 {
        let value = self.get_value();
        value as f64
    }

    fn get_string(&self) -> String {
        let value = self.get_value();
        format!("{}", value)
    }

    fn get_type(&self) -> ColumnType {
        ColumnType::Float
    }
}
impl WCDBBasicTypes for bool {
    fn get_value(&self) -> bool {
        let any_value = self as &dyn Any;
        match any_value.downcast_ref::<bool>() {
            None => false,
            Some(value) => *value,
        }
    }

    fn get_bool(&self) -> bool {
        self.get_value()
    }

    fn get_i64(&self) -> i64 {
        let value = self.get_value();
        if value {
            1
        } else {
            0
        }
    }

    fn get_f64(&self) -> f64 {
        let value = self.get_value();
        if value {
            1f64
        } else {
            0f64
        }
    }

    fn get_string(&self) -> String {
        let value = self.get_value();
        if value {
            "true".to_string()
        } else {
            "false".to_string()
        }
    }

    fn get_type(&self) -> ColumnType {
        ColumnType::Integer
    }
}
impl WCDBBasicTypes for String {
    fn get_value(&self) -> String {
        let any_value = self as &dyn Any;
        match any_value.downcast_ref::<String>() {
            None => String::new(),
            Some(value) => value.clone(),
        }
    }

    fn get_bool(&self) -> bool {
        eprintln!("WCDB BasicTypes: String can't convert to bool");
        return false;
    }

    fn get_i64(&self) -> i64 {
        eprintln!("WCDB BasicTypes: String can't convert to i64");
        return -1;
    }

    fn get_f64(&self) -> f64 {
        eprintln!("WCDB BasicTypes: String can't convert to f64");
        return -1f64;
    }

    fn get_string(&self) -> String {
        self.get_value()
    }

    fn get_type(&self) -> ColumnType {
        ColumnType::Text
    }
}

impl WCDBBasicTypes for &'static str {
    fn get_value(&self) -> &'static str {
        let any_value = self as &dyn Any;
        match any_value.downcast_ref::<&'static str>() {
            None => "",
            Some(value) => *value,
        }
    }

    fn get_bool(&self) -> bool {
        eprintln!("WCDB BasicTypes: &'static str can't convert to bool");
        false
    }

    fn get_i64(&self) -> i64 {
        eprintln!("WCDB BasicTypes: &'static str can't convert to i64");
        -1i64
    }

    fn get_f64(&self) -> f64 {
        eprintln!("WCDB BasicTypes: &'static str can't convert to f64");
        -1f64
    }

    fn get_string(&self) -> String {
        let value = self.get_value();
        value.to_string()
    }

    fn get_type(&self) -> ColumnType {
        ColumnType::Text
    }
}
