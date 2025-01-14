use std::time::SystemTime;

pub struct WrappedValue {
    pub bool_value: bool,
    pub int_value: i64,
    pub string_value: String,
    pub double_value: f64,
}

impl WrappedValue {
    pub fn new() -> Self {
        WrappedValue {
            bool_value: false,
            int_value: 0,
            string_value: "".to_string(),
            double_value: 0.0,
        }
    }

    pub fn current_time() -> Self {
        let mut value = WrappedValue::new();
        value.int_value = SystemTime::now()
            .duration_since(SystemTime::UNIX_EPOCH)
            .unwrap()
            .as_millis() as i64;
        value
    }
}
