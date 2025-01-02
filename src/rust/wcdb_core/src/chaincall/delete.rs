use crate::chaincall::chain_call::{ChainCall, ChainCallTrait};
use crate::core::handle::Handle;
use crate::wcdb_error::WCDBResult;
use crate::winq::expression::Expression;
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

    pub fn from_table(self, table_name: &str) -> Self {
        self.chain_call.statement.delete_from(table_name);
        self
    }

    /// where 是 RUST 关键字，所以用 where_expression
    pub fn where_expression(self, condition: Expression) -> Self {
        self.chain_call.statement.where_expression(condition);
        self
    }

    pub fn execute(mut self) -> WCDBResult<Self> {
        self.chain_call.handle.execute(&self.chain_call.statement)?;
        self.chain_call.update_changes()?;
        self.chain_call.invalidate_handle();
        Ok(self)
    }
}
