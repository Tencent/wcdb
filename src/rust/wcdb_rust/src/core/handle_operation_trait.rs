use crate::core::handle::Handle;

pub trait HandleOperationTrait {
    fn get_handle(&self, write_hint: bool) -> Handle;
    fn auto_invalidate_handle(&self) -> bool;
}
