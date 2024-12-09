use crate::orm::table_binding::TableBinding;
use crate::winq::column::Column;

pub struct Field<T> {
    column: Column,
    name: String,
    binding: *const dyn TableBinding<T>,
    field_id: usize,
    is_auto_increment: bool,
    is_primary_key: bool,
}

impl<T> Field<T> {
    pub fn new(
        name: &str,
        binding: *const dyn TableBinding<T>,
        field_id: usize,
        is_auto_increment: bool,
        is_primary_key: bool,
    ) -> Field<T> {
        let bind = unsafe { &*binding };
        Field {
            column: Column::new_with_binding(name, bind.base_binding().get_base_binding()),
            name: name.to_string(),
            binding,
            field_id,
            is_auto_increment,
            is_primary_key,
        }
    }

    pub fn get_field_id(&self) -> usize {
        self.field_id
    }
}
