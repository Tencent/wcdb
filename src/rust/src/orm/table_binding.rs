use crate::orm::binding::Binding;

pub trait TableBinding {
    fn base_binding(&self) -> Binding;
}
