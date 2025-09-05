use crate::winq::schema::Schema;

pub enum StringSchemaParam<'a> {
    String(String),
    Schema(Option<&'a Schema>),
}

impl<'a> From<String> for StringSchemaParam<'a> {
    fn from(value: String) -> Self {
        StringSchemaParam::String(value)
    }
}

impl<'a> From<&str> for StringSchemaParam<'a> {
    fn from(value: &str) -> Self {
        StringSchemaParam::String(value.to_string())
    }
}

impl<'a> From<Option<&'a Schema>> for StringSchemaParam<'a> {
    fn from(value: Option<&'a Schema>) -> Self {
        StringSchemaParam::Schema(value)
    }
}
