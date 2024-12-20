use crate::chaincall::chain_call::ChainCall;
use crate::core::handle::Handle;
use crate::orm::field::Field;
use crate::winq::statement::StatementTrait;
use crate::winq::statement_insert::StatementInsert;

pub struct Insert<'a, T> {
    chain_call: ChainCall<'a, StatementInsert>,
    has_conflict_action: bool,
    fields: Vec<Field<T>>,
    values: Vec<T>,
    last_insert_row_id: i64,
}

impl<'a, T> Insert<'a, T> {
    pub fn new(
        handle: Handle<'a>,
        need_changes: bool,
        auto_invalidate_handle: bool,
    ) -> Insert<'a, T> {
        Insert {
            chain_call: ChainCall::new(
                StatementInsert::new(),
                handle,
                need_changes,
                auto_invalidate_handle,
            ),
            has_conflict_action: false,
            fields: Vec::new(),
            values: Vec::new(),
            last_insert_row_id: 0,
        }
    }
}
