use crate::base::value::{Value, ValueObject};
use crate::base::wcdb_exception::WCDBResult;
use crate::core::database::Database;
use crate::core::handle::Handle;
use crate::core::handle_operation::HandleOperationTrait;
use crate::winq::column::Column;
use crate::winq::conflict_action::ConflictAction;
use crate::winq::expression::Expression;
use crate::winq::identifier::IdentifierTrait;
use crate::winq::ordering_term::OrderingTerm;
use crate::winq::statement_delete::StatementDelete;
use crate::winq::statement_insert::StatementInsert;
use crate::winq::statement_select::StatementSelect;
use crate::winq::statement_update::StatementUpdate;

pub struct TableOperation<'a> {
    table_name: String,
    database: &'a Database,
}

pub trait TableOperationTrait {
    fn get_table_name(&self) -> &str;

    fn get_database(&self) -> &Database;

    fn insert_rows(&self, rows: Vec<Vec<Value>>, columns: Vec<Column>) -> WCDBResult<()>;

    fn insert_or_replace_rows(&self, rows: Vec<Vec<Value>>, columns: Vec<Column>)
        -> WCDBResult<()>;

    fn insert_or_ignore_rows(&self, rows: Vec<Vec<Value>>, columns: Vec<Column>) -> WCDBResult<()>;

    fn update_value<V: Into<ValueObject>>(
        &self,
        value: V,
        column: Column,
        condition_opt: Option<&Expression>,
        order_opt: Option<&OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()>;

    fn update_row(
        &self,
        row: &Vec<Value>,
        columns: &Vec<Column>,
        condition_opt: Option<&Expression>,
        order_opt: Option<&OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()>;

    fn delete_value(
        &self,
        condition_opt: Option<&Expression>,
        order_opt: Option<&OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()>;

    fn get_values(
        &self,
        columns: Vec<&Column>,
        condition_opt: Option<&Expression>,
        order_opt: Option<Vec<OrderingTerm>>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<Vec<Vec<Value>>>;
}

impl<'a> TableOperationTrait for TableOperation<'a> {
    fn get_table_name(&self) -> &str {
        &self.table_name
    }

    fn get_database(&self) -> &Database {
        self.database
    }

    fn insert_rows(&self, rows: Vec<Vec<Value>>, columns: Vec<Column>) -> WCDBResult<()> {
        self.insert_rows_with_conflict_action(rows, columns, ConflictAction::None)
    }

    fn insert_or_replace_rows(
        &self,
        rows: Vec<Vec<Value>>,
        columns: Vec<Column>,
    ) -> WCDBResult<()> {
        self.insert_rows_with_conflict_action(rows, columns, ConflictAction::Replace)
    }

    fn insert_or_ignore_rows(&self, rows: Vec<Vec<Value>>, columns: Vec<Column>) -> WCDBResult<()> {
        self.insert_rows_with_conflict_action(rows, columns, ConflictAction::Ignore)
    }

    fn update_value<V: Into<ValueObject>>(
        &self,
        value: V,
        column: Column,
        condition_opt: Option<&Expression>,
        order_opt: Option<&OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()> {
        let row_item = Value::new(value);
        self.update_row(
            &vec![row_item],
            &vec![column],
            condition_opt,
            order_opt,
            limit_opt,
            offset_opt,
        )
    }

    fn update_row(
        &self,
        row: &Vec<Value>,
        columns: &Vec<Column>,
        condition_opt: Option<&Expression>,
        order_opt: Option<&OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()> {
        let binding = StatementUpdate::new();
        binding
            .update(self.table_name.as_str())
            .set_columns_to_bind_parameters(columns);
        self.execute_update(
            row,
            &binding,
            condition_opt,
            order_opt,
            limit_opt,
            offset_opt,
        )
    }

    fn delete_value(
        &self,
        condition_opt: Option<&Expression>,
        order_opt: Option<&OrderingTerm>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<()> {
        let binding = StatementDelete::new();
        binding.delete_from(self.table_name.as_ref());
        if let Some(expression) = condition_opt {
            binding.r#where(&expression);
        }
        if let Some(order) = order_opt {
            binding.order_by(vec![order]);
        }
        if let Some(limit) = limit_opt {
            binding.limit(limit);
        }
        if let Some(offset) = offset_opt {
            binding.offset(offset);
        }
        self.database.get_handle(true).execute(&binding)
    }

    fn get_values(
        &self,
        columns: Vec<&Column>,
        condition_opt: Option<&Expression>,
        order_opt: Option<Vec<OrderingTerm>>,
        limit_opt: Option<i64>,
        offset_opt: Option<i64>,
    ) -> WCDBResult<Vec<Vec<Value>>> {
        let handle = self.database.get_handle(false);
        let binding = StatementSelect::new();
        binding.from(vec![self.table_name.as_str()]);
        binding.select(columns);
        if let Some(expression) = condition_opt {
            binding.r#where(&expression);
        }
        if let Some(order) = order_opt {
            binding.order_by(&order);
        }
        if let Some(limit) = limit_opt {
            binding.limit(limit);
        }
        if let Some(offset) = offset_opt {
            binding.offset(offset);
        }
        match handle.prepared_with_main_statement(&binding) {
            Ok(statement) => match statement.get_all_values() {
                Ok(ret) => Ok(ret),
                Err(err) => Err(err),
            },
            Err(err) => Err(err),
        }
    }
}

impl<'a> TableOperation<'a> {
    pub fn new(table_name: &str, database: &'a Database) -> TableOperation<'a> {
        TableOperation {
            table_name: table_name.to_string(),
            database,
        }
    }
}

impl<'a> TableOperation<'a> {
    fn insert_rows_with_conflict_action(
        &self,
        rows: Vec<Vec<Value>>,
        columns: Vec<Column>,
        action: ConflictAction,
    ) -> WCDBResult<()> {
        if rows.len() == 0 {
            return Ok(());
        }
        let binding = StatementInsert::new();
        let insert = binding
            .insert_into(self.table_name.as_ref())
            .column_objs(&columns)
            .values_with_bind_parameters(columns.len());
        match action {
            ConflictAction::Replace => {
                insert.or_replace();
            }
            ConflictAction::Ignore => {
                insert.or_ignore();
            }
            _ => {}
        }
        let handle = self.database.get_handle(true);
        if rows.len() == 1 {
            return self.insert_rows_with_handle(&rows, &insert, &handle);
        }
        handle.run_transaction(|handle: &Handle| {
            self.insert_rows_with_handle(&rows, &insert, handle).is_ok()
        })
    }

    fn insert_rows_with_handle(
        &self,
        rows: &Vec<Vec<Value>>,
        insert: &StatementInsert,
        handle: &Handle,
    ) -> WCDBResult<()> {
        println!("statement: {:?}", insert.get_description());
        match handle.prepared_with_main_statement(insert) {
            Ok(prepared_stmt) => {
                for row in rows {
                    prepared_stmt.reset();
                    prepared_stmt.bind_row(row);
                    prepared_stmt.step()?;
                }
                prepared_stmt.finalize_statement();
                Ok(())
            }
            Err(err) => Err(err),
        }
    }
}

/// Update
impl<'a> TableOperation<'a> {
    fn execute_update(
        &self,
        row: &Vec<Value>,
        update: &StatementUpdate,
        expression: Option<&Expression>,
        order: Option<&OrderingTerm>,
        limit: Option<i64>,
        offset: Option<i64>,
    ) -> WCDBResult<()> {
        if let Some(order) = order {
            update.order_by(vec![order]);
        }
        if let Some(limit) = limit {
            update.limit(limit, None);
        }
        if let Some(offset) = offset {
            update.offset(offset);
        }
        if let Some(expression) = expression {
            update.r#where(&expression);
        }
        let mut handler = self.database.get_handle(true);
        let ret = match handler.prepared_with_main_statement(update) {
            Ok(statement) => {
                statement.bind_row(row);
                let step_ret = statement.step();
                statement.finalize_statement();
                step_ret
            }
            Err(err) => Err(err),
        };
        handler.invalidate();
        ret
    }
}

impl<'a> TableOperation<'a> {
    pub fn get_handle(&self, write_hint: bool) -> Handle {
        self.database.get_handle(write_hint)
    }
}
