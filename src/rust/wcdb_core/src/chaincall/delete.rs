use crate::chaincall::chain_call::{ChainCall, ChainCallTrait};
use crate::core::handle::Handle;
use crate::wcdb_error::WCDBResult;
use crate::winq::statement::StatementTrait;
use crate::winq::statement_delete::StatementDelete;
use std::fmt::Debug;

pub struct Delete<'a> {
    chain_call: ChainCall<'a, StatementDelete>,
}

impl<'a> ChainCallTrait for Delete<'a> {
    fn update_changes(&self) -> WCDBResult<()> {
        self.chain_call.update_changes()
    }

    fn get_statement(&self) -> &dyn StatementTrait {
        &self.chain_call.statement
    }
}

impl<'a> Delete<'a> {
    pub fn new(handle: Handle<'a>, need_changes: bool, auto_invalidate_handle: bool) -> Self {
        Delete {
            chain_call: ChainCall::new(
                StatementDelete::new(),
                handle,
                need_changes,
                auto_invalidate_handle,
            ),
        }
    }
}
