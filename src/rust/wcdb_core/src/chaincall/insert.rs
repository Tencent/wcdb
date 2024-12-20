use crate::chaincall::chain_call::ChainCall;
use crate::core::handle::Handle;
use crate::orm::field::Field;
use crate::winq::statement_insert::StatementInsert;

pub struct Insert<'a, T> {
    chain_call: ChainCall<'a, StatementInsert>,
    has_conflict_action: bool,
    fields: Vec<&'a Field<T>>,
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

    pub fn into_table(&mut self, table_name: &str) -> &mut Self {
        self.chain_call.statement.insert_into(table_name);
        self
    }

    pub fn value(&mut self, object: T) -> &mut Self {
        self.values.clear();
        self.values.push(object);
        self
    }

    pub fn on_fields(&mut self, fields: Vec<&'a Field<T>>) -> &mut Self {
        self.fields = fields;
        self.chain_call.statement.columns(&self.fields);
        self
    }

    pub fn execute(&mut self) -> &mut Self {
        if self.values.is_empty() {
            return self;
        }
        assert!(!self.fields.is_empty());
        todo!();
        self
    }
}
