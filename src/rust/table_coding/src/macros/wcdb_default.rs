use darling::FromMeta;

#[derive(FromMeta, Debug)]
pub struct WCDBDefault {
    #[darling(default)]
    i32_value: i64,
    #[darling(default)]
    f64_value: f64,
    #[darling(default)]
    text_value: String,
}

impl WCDBDefault {
    pub fn new() -> Self {
        WCDBDefault {
            i32_value: 0,
            f64_value: 0.0,
            text_value: "".to_string(),
        }
    }

    pub fn i32_value(&self) -> i64 {
        self.i32_value
    }

    pub fn f64_value(&self) -> f64 {
        self.f64_value
    }

    pub fn text_value(&self) -> &str {
        &self.text_value
    }
}
