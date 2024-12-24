use crate::chaincall::chain_call::ChainCall;
use crate::core::handle::Handle;
use crate::core::handle_operation::HandleOperationTrait;
use crate::orm::field::Field;
use crate::orm::table_binding::TableBinding;
use crate::wcdb_error::WCDBResult;
use crate::winq::statement_insert::StatementInsert;
use std::cell::RefCell;

pub struct Insert<'a, T> {
    chain_call: ChainCall<'a, StatementInsert>,
    has_conflict_action: bool,
    fields: Vec<&'a Field<T>>,
    values: RefCell<Vec<T>>,
    last_insert_row_id: RefCell<i64>,
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
            values: RefCell::new(Vec::new()),
            last_insert_row_id: RefCell::new(0),
        }
    }

    pub fn into_table(&mut self, table_name: &str) -> &mut Self {
        self.chain_call.statement.insert_into(table_name);
        self
    }

    pub fn value(&mut self, object: T) -> &mut Self {
        self.values.borrow_mut().clear();
        self.values.borrow_mut().push(object);
        self
    }

    pub fn on_fields(&mut self, fields: Vec<&'a Field<T>>) -> &mut Self {
        self.fields = fields;
        self.chain_call.statement.columns(&self.fields);
        self
    }

    pub fn execute(&mut self) -> WCDBResult<&mut Self> {
        if self.values.borrow().is_empty() {
            return Ok(self);
        }
        assert!(!self.fields.is_empty());
        if self.values.borrow().len() > 1 {
            self.chain_call.handle.run_transaction(|handle| {
                self.real_execute();
                return true;
            })?;
        } else {
            self.real_execute();
        }
        Ok(self)
    }

    pub fn real_execute(&self) {
        let binding: &dyn TableBinding<T> = Field::get_binding_from_fields(&self.fields);
        let prepared_statement = self
            .chain_call
            .handle
            .prepared_with_main_statement(&self.chain_call.statement);
        *self.last_insert_row_id.borrow_mut() = 0;
        // let mut values = self.values.borrow_mut();
        // for value in values.iter_mut() {
        //     // binding.set_last_insert_row_id(value, self.chain_call.handle.ge)
        // }
    }
}
