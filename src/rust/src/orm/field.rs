use crate::orm::table_binding::TableBinding;
use crate::winq::column::Column;

pub struct Field<T> {
    column: Column,
    name: String,
    binding: Option<Box<dyn TableBinding<T>>>,
    field_id: usize,
    is_auto_increment: bool,
    is_primary_key: bool,
}
