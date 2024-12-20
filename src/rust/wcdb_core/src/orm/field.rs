use std::ffi::c_void;
use crate::base::cpp_object::CppObjectTrait;
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

impl<T> CppObjectTrait for Field<T> {
    fn set_cpp_obj(&mut self, _cpp_obj: *mut c_void) {
        self.column.set_cpp_obj(_cpp_obj);
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.column.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.column.release_cpp_object();
    }
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

    pub fn get_column(&self) -> &Column {
        &self.column
    }

    pub fn get_field_id(&self) -> usize {
        self.field_id
    }
}
