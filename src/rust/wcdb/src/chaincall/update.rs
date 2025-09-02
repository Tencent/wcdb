use crate::base::value::Value;
use crate::base::wcdb_exception::WCDBResult;
use crate::chaincall::chain_call::{ChainCall, ChainCallTrait};
use crate::core::handle::Handle;
use crate::core::prepared_statement::PreparedStatement;
use crate::orm::field::Field;
use crate::winq::expression::Expression;
use crate::winq::ordering_term::OrderingTerm;
use crate::winq::statement::StatementTrait;
use crate::winq::statement_update::StatementUpdate;
use std::cell::RefCell;

pub struct Update<'a, T> {
    chain_call: ChainCall<'a, StatementUpdate>,
    fields: RefCell<Vec<&'a Field<T>>>,
    object: RefCell<Option<T>>,
    row: RefCell<Vec<Value>>,
}

impl<'a, T> ChainCallTrait for Update<'a, T> {
    fn update_changes(&self) -> WCDBResult<()> {
        self.chain_call.update_changes()
    }

    fn get_statement(&self) -> &dyn StatementTrait {
        self.chain_call.get_statement()
    }
}

impl<'a, T> Update<'a, T> {
    pub fn new(
        handle: Handle<'a>,
        need_changes: bool,
        auto_invalidate_handle: bool,
    ) -> Update<'a, T> {
        Update {
            chain_call: ChainCall::new(
                StatementUpdate::new(),
                handle,
                need_changes,
                auto_invalidate_handle,
            ),
            fields: RefCell::new(Vec::new()),
            object: RefCell::new(None),
            row: RefCell::new(Vec::new()),
        }
    }

    pub fn table(&self, table_name: &str) -> &Self {
        self.chain_call.get_statement().update(table_name);
        self
    }

    pub fn set(&self, fields: Vec<&'a Field<T>>) -> &Self {
        self.fields.replace(fields);
        self.chain_call
            .get_statement()
            .set_columns_to_bind_parameters(&self.fields);
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

    pub fn to_object(&self, object: T) -> &Self {
        self.object.replace(Some(object));
        self
    }

    pub fn execute(&self) -> WCDBResult<&Self> {
        // let binding = Field::get_binding_from_fields(&self.fields);
        let prepared_statement = self
            .chain_call
            .handle
            .prepared_with_main_statement(self.chain_call.get_statement())?;

        if let Some(object) = self.object.take() {
            PreparedStatement::bind_object_by_fields(&prepared_statement, object, &self.fields);
        } else {
            let row_vec = self.row.take();
            if !row_vec.is_empty() {
                prepared_statement.bind_row(&row_vec);
            }
        }
        prepared_statement.step()?;
        self.chain_call.update_changes()?;
        prepared_statement.finalize_statement();
        self.chain_call.invalidate_handle();
        Ok(self)
    }
}
