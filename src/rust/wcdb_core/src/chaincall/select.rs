use crate::base::wcdb_exception::WCDBResult;
use crate::chaincall::chain_call::{ChainCall, ChainCallTrait};
use crate::core::handle::Handle;
use crate::orm::field::Field;
use crate::winq::statement::StatementTrait;
use crate::winq::statement_select::StatementSelect;

pub struct Select<'a, T> {
    chain_call: ChainCall<'a, StatementSelect>,
    fields: Vec<Field<&'a T>>,
}

impl<'a, T> ChainCallTrait for Select<'a, T> {
    fn update_changes(&self) -> WCDBResult<()> {
        self.chain_call.update_changes()
    }

    fn get_statement(&self) -> &dyn StatementTrait {
        self.chain_call.get_statement()
    }
}

impl<'a, T> Select<'a, T> {
    pub fn new(
        handle: Handle<'a>,
        need_changes: bool,
        auto_invalidate_handle: bool,
    ) -> Select<'a, T> {
        Select {
            chain_call: ChainCall::new(
                StatementSelect::new(),
                handle,
                need_changes,
                auto_invalidate_handle,
            ),
            fields: Vec::new(),
        }
    }
}
