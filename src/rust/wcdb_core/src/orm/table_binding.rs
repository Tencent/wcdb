use crate::core::prepared_statement::PreparedStatement;
use crate::orm::binding::Binding;
use crate::orm::field::Field;
use std::any::TypeId;

pub trait TableBinding<T> {
    fn binding_type(&self) -> TypeId;

    fn all_binding_fields(&self) -> Vec<&Field<T>>;

    fn base_binding(&self) -> &Binding;

    fn extract_object(
        &self,
        fields: Vec<Field<T>>,
        prepared_statement: &PreparedStatement,
    ) -> T;

    fn bind_field(
        &self,
        object: &T,
        field: &Field<T>,
        index: usize,
        prepared_statement: &mut PreparedStatement,
    );

    fn is_auto_increment(&self, object: &T) -> bool;

    fn set_last_insert_row_id(&self, object: &mut T, last_insert_row_id: i64);
}
