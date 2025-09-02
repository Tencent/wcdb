use crate::base::wcdb_exception::WCDBResult;
use crate::chaincall::chain_call::{ChainCall, ChainCallTrait};
use crate::core::handle::Handle;
use crate::core::handle_operation::HandleOperationTrait;
use crate::orm::field::Field;
use crate::orm::table_binding::TableBinding;
use crate::winq::statement::StatementTrait;
use crate::winq::statement_insert::StatementInsert;
use std::cell::RefCell;
use std::fmt::Debug;

pub struct Insert<'a, T> {
    chain_call: ChainCall<'a, StatementInsert>,
    has_conflict_action: RefCell<bool>,
    fields: RefCell<Vec<&'a Field<T>>>,
    values: RefCell<Vec<T>>,
    last_insert_row_id: RefCell<i64>,
}

impl<'a, T> Drop for Insert<'a, T> {
    fn drop(&mut self) {
        self.values.borrow_mut().clear();
    }
}

impl<'a, T> ChainCallTrait for Insert<'a, T> {
    fn update_changes(&self) -> WCDBResult<()> {
        self.chain_call.update_changes()
    }

    fn get_statement(&self) -> &dyn StatementTrait {
        self.chain_call.get_statement()
    }
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
            has_conflict_action: RefCell::new(false),
            fields: RefCell::new(Vec::new()),
            values: RefCell::new(Vec::new()),
            last_insert_row_id: RefCell::new(0),
        }
    }

    pub fn or_replace(&self) -> &Self {
        self.has_conflict_action.replace(true);
        self.chain_call.get_statement().or_replace();
        self
    }

    pub fn or_ignore(&self) -> &Self {
        self.has_conflict_action.replace(true);
        self.chain_call.get_statement().or_ignore();
        self
    }

    pub fn into_table(&self, table_name: &str) -> &Self {
        self.chain_call.get_statement().insert_into(table_name);
        self
    }

    pub fn on_fields(&self, fields: Vec<&'a Field<T>>) -> &Self {
        let fields_clone = fields.clone();
        self.fields.replace(fields);
        self.chain_call
            .get_statement()
            .columns(&fields_clone)
            .values_with_bind_parameters(fields_clone.len());
        self
    }

    pub fn value(&self, object: T) -> &Self {
        self.values.borrow_mut().clear();
        self.values.borrow_mut().push(object);
        self
    }

    pub fn values(&self, objects: Vec<T>) -> &Self {
        self.values.borrow_mut().clear();
        self.values.borrow_mut().extend(objects);
        self
    }

    pub fn execute(&self) -> WCDBResult<&Self> {
        if self.values.borrow().is_empty() {
            return Ok(self);
        }
        assert!(!self.fields.borrow().is_empty());
        if self.values.borrow().len() > 1 {
            self.chain_call
                .handle
                .run_transaction(|handle| self.real_execute().is_ok())?;
        } else {
            self.real_execute()?;
        }
        Ok(self)
    }

    pub fn get_last_insert_row_id(&self) -> i64 {
        *self.last_insert_row_id.borrow()
    }

    pub fn real_execute(&self) -> WCDBResult<()> {
        let binding: &dyn TableBinding<T> = Field::get_binding_from_fields(&self.fields.borrow());
        let prepared_statement = self
            .chain_call
            .handle
            .prepared_with_main_statement(self.chain_call.get_statement())?;
        *self.last_insert_row_id.borrow_mut() = 0;
        let mut values = self.values.borrow_mut();
        for object in values.iter_mut() {
            prepared_statement.reset();
            let mut index: usize = 1;
            let has_conflict_action = *self.has_conflict_action.borrow();
            let is_auto_increment = !has_conflict_action && binding.is_auto_increment(object);
            for field in &*self.fields.borrow() {
                if is_auto_increment && field.is_auto_increment() {
                    prepared_statement.bind_null(index);
                } else {
                    binding.bind_field(object, field, index, &prepared_statement);
                }
                index += 1;
            }
            prepared_statement.step()?;
            if is_auto_increment {
                binding.set_last_insert_row_id(
                    object,
                    self.chain_call.handle.get_last_inserted_row_id()?,
                );
            }
        }
        if values.len() > 0 {
            *self.last_insert_row_id.borrow_mut() =
                self.chain_call.handle.get_last_inserted_row_id()?;
        }
        self.update_changes()?;
        prepared_statement.finalize_statement();
        Ok(())
    }
}
