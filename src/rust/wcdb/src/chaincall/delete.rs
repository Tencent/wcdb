use crate::base::wcdb_exception::WCDBResult;
use crate::chaincall::chain_call::{ChainCall, ChainCallTrait};
use crate::core::handle::Handle;
use crate::winq::expression::Expression;
use crate::winq::ordering_term::OrderingTerm;
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
        self.chain_call.get_statement()
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

    pub fn from_table(&self, table_name: &str) -> &Self {
        self.chain_call.get_statement().delete_from(table_name);
        self
    }

    pub fn r#where(&self, condition: &Expression) -> &Self {
        self.chain_call.get_statement().r#where(condition);
        self
    }

    pub fn order_by(&self, orders: &Vec<OrderingTerm>) -> &Self {
        self.chain_call.get_statement().order_by(orders);
        self
    }

    pub fn limit(&self, count: i64) -> &Self {
        self.chain_call.get_statement().limit(count);
        self
    }

    pub fn offset(&self, offset: i64) -> &Self {
        self.chain_call.get_statement().offset(offset);
        self
    }

    pub fn execute(&self) -> WCDBResult<&Self> {
        let ret = self
            .chain_call
            .handle
            .borrow()
            .execute(self.chain_call.get_statement());
        self.chain_call.update_changes()?;
        self.chain_call.invalidate_handle();
        ret.map(|_| self)
    }
}
