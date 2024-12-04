use crate::orm::table_binding::TableBinding;
use crate::winq::column::Column;

pub struct Field<'a, T> {
    column: Column,
    name: String,
    binding: &'a dyn TableBinding<T>,
    field_id: usize,
    is_auto_increment: bool,
    is_primary_key: bool,
}

impl<'a, T> Field<'a, T> {
    pub fn new(
        name: &str,
        binding: &'a dyn TableBinding<T>,
        field_id: usize,
        is_auto_increment: bool,
        is_primary_key: bool,
    ) -> Field<'a, T> {
        Field {
            column: Column::new_with_binding(name, binding.base_binding().get_base_binding()),
            name: name.to_string(),
            binding,
            field_id,
            is_auto_increment,
            is_primary_key,
        }
    }
}
