use crate::winq::window_def::WindowDef;

/// 支持 String, &str, &WindowDef
pub enum StringWindowDef<'a> {
    String(String),
    WindowDef(&'a WindowDef),
}

impl<'a> From<String> for StringWindowDef<'a> {
    fn from(value: String) -> Self {
        StringWindowDef::String(value)
    }
}

impl<'a> From<&str> for StringWindowDef<'a> {
    fn from(value: &str) -> Self {
        StringWindowDef::String(value.to_string())
    }
}

impl<'a> From<&'a WindowDef> for StringWindowDef<'a> {
    fn from(value: &'a WindowDef) -> Self {
        StringWindowDef::WindowDef(value)
    }
}
