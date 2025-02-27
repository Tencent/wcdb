pub struct DefaultValueInfo {
    pub int_value: i64,
    pub double_value: f64,
    pub string_value: String,
}

impl DefaultValueInfo {
    pub fn new() -> Self {
        DefaultValueInfo {
            int_value: 0,
            double_value: 0.0,
            string_value: String::new(),
        }
    }
}
