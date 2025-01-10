use crate::base::cpp_object::CppObjectTrait;
use crate::base::wcdb_exception::{WCDBException, WCDBResult};
use crate::chaincall::chain_call::{ChainCall, ChainCallTrait};
use crate::core::handle::{Handle, WCDBRustHandle_getError};
use crate::core::prepared_statement::PreparedStatement;
use crate::orm::field::Field;
use crate::winq::expression::Expression;
use crate::winq::statement::StatementTrait;
use crate::winq::statement_select::StatementSelect;
use std::sync::Arc;

pub struct Select<'a, T> {
    chain_call: ChainCall<'a, StatementSelect>,
    fields: Vec<&'a Field<T>>,
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

    pub fn select(mut self, fields: Vec<&'a Field<T>>) -> Self {
        self.fields = fields;
        self.chain_call.statement.select(&self.fields);
        self
    }

    pub fn where_expression(self, condition: Expression) -> Self {
        self.chain_call.statement.where_expression(&condition);
        self
    }

    pub fn from(self, table_name: &str) -> Self {
        self.chain_call.statement.from(table_name);
        self
    }

    pub fn first_object(&self) -> WCDBResult<T> {
        self.first_object_by_class()
    }

    pub fn first_object_by_class(&self) -> WCDBResult<T> {
        let prepared_statement = self.prepare_statement()?;
        prepared_statement.step()?;
        let ret: WCDBResult<T> = if !prepared_statement.is_done() {
            prepared_statement.get_one_object(&self.fields)
        } else {
            Err(WCDBException::create_exception(
                prepared_statement.get_cpp_obj(),
            ))
        };
        prepared_statement.finalize_statement();
        self.chain_call.invalidate_handle();
        ret
    }

    pub fn all_objects(&self) -> WCDBResult<Vec<T>> {
        self.all_objects_by_class()
    }

    pub fn all_objects_by_class(&self) -> WCDBResult<Vec<T>> {
        let prepared_statement = self.prepare_statement()?;
        let ret = prepared_statement.get_all_objects(&self.fields);
        prepared_statement.finalize_statement();
        self.chain_call.invalidate_handle();
        ret
    }

    fn prepare_statement(&self) -> WCDBResult<Arc<PreparedStatement>> {
        self.chain_call
            .handle
            .prepared_with_main_statement(&self.chain_call.statement)
    }
}
