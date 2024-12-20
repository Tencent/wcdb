use crate::core::handle::Handle;
use crate::winq::statement::StatementTrait;

pub struct ChainCall<'a, T: StatementTrait> {
    handle: Handle<'a>,
    changes: i32,
    statement: T,
    need_changes: bool,
    auto_invalidate_handle: bool,
}

impl<'a, T: StatementTrait> ChainCall<'a, T> {
    pub fn new(statement: T, handle: Handle<'a>, need_changes: bool, auto_invalidate_handle: bool) -> ChainCall<'a, T> {
        ChainCall {
            handle,
            changes: 0,
            statement,
            need_changes,
            auto_invalidate_handle,
        }
    }
}
