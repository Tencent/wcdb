use crate::orm::field::Field;

pub struct Update<'a, T> {
    fields: Vec<Field<&'a T>>,
    // row:Vec<Value>
}

impl<'a, T> Update<'a, T> {}
