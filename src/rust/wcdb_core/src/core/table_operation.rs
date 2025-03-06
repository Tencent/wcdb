use crate::base::basic_types::WCDBBasicTypes;
use crate::base::value::Value;
use crate::base::wcdb_exception::WCDBException;
use crate::core::database::Database;
use crate::core::handle::Handle;
use crate::core::handle_operation::HandleOperationTrait;
use crate::orm::field::Field;
use crate::winq::column_type::ColumnType;
use crate::winq::conflict_action::ConflictAction;
use crate::winq::expression::Expression;
use crate::winq::ordering_term::OrderingTerm;
use crate::winq::statement_delete::StatementDelete;
use crate::winq::statement_insert::StatementInsert;
use crate::winq::statement_select::StatementSelect;
use crate::winq::statement_update::StatementUpdate;

pub struct TableOperation<'a> {
    table_name: String,
    database: &'a Database,
}

impl<'a> TableOperation<'a> {
    pub fn new(table_name: &str, database: &'a Database) -> TableOperation<'a> {
        TableOperation {
            table_name: table_name.to_string(),
            database,
        }
    }

    pub fn get_table_name(&self) -> &str {
        &self.table_name
    }

    pub fn get_database(&self) -> &Database {
        self.database
    }
}

/// Insert
impl<'a> TableOperation<'a> {
    pub fn insert_rows<T>(
        &self,
        rows: Vec<Vec<Value>>,
        columns: &Vec<&Field<T>>,
    ) -> Result<(), WCDBException> {
        self.insert_rows_with_conflict_action(rows, columns, ConflictAction::None)
    }

    pub fn insert_rows_or_replace<T>(
        &self,
        rows: Vec<Vec<Value>>,
        columns: &Vec<&Field<T>>,
    ) -> Result<(), WCDBException> {
        self.insert_rows_with_conflict_action(rows, columns, ConflictAction::Replace)
    }

    pub fn insert_rows_or_ignore<T>(
        &self,
        rows: Vec<Vec<Value>>,
        columns: &Vec<&Field<T>>,
    ) -> Result<(), WCDBException> {
        self.insert_rows_with_conflict_action(rows, columns, ConflictAction::Ignore)
    }

    fn insert_rows_with_conflict_action<T>(
        &self,
        rows: Vec<Vec<Value>>,
        columns: &Vec<&Field<T>>,
        action: ConflictAction,
    ) -> Result<(), WCDBException> {
        if rows.len() == 0 {
            return Ok(());
        }
        let binding = StatementInsert::new();
        let insert = binding
            .insert_into(self.table_name.as_ref())
            .columns(columns)
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
        handle.run_transaction(|handle: Handle| {
            self.insert_rows_with_handle(&rows, &insert, &handle)
                .is_ok()
        })
    }

    fn insert_rows_with_handle(
        &self,
        rows: &Vec<Vec<Value>>,
        insert: &StatementInsert,
        handle: &Handle,
    ) -> Result<(), WCDBException> {
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
    pub fn update_value<V: WCDBBasicTypes, T>(
        &self,
        value: &V,
        column: &Field<T>,
        expression: Option<Expression>,
        order: Option<Vec<OrderingTerm>>,
        limit: Option<i64>,
        offset: Option<i64>,
    ) -> Result<(), WCDBException> {
        let binding = StatementUpdate::new();
        binding
            .update(self.table_name.as_ref())
            .set_columns_to_bind_parameters(&vec![column]);
        let row_item = match value.get_type() {
            ColumnType::Integer => Value::from(value.get_i64()),
            ColumnType::Float => Value::from(value.get_f64()),
            ColumnType::Text => Value::from(value.get_string().as_ref()),
            _ => {
                panic!("basic types not define.")
            }
        };
        self.execute_update(&vec![row_item], &binding, expression, order, limit, offset)
    }

    pub fn update_row<T>(
        &self,
        row: &Vec<Value>,
        columns: &Vec<&Field<T>>,
        expression: Option<Expression>,
        order: Option<Vec<OrderingTerm>>,
        limit: Option<i64>,
        offset: Option<i64>,
    ) -> Result<(), WCDBException> {
        let binding = StatementUpdate::new();
        binding
            .update(self.table_name.as_ref())
            .set_columns_to_bind_parameters(columns);
        self.execute_update(row, &binding, expression, order, limit, offset)
    }

    fn execute_update(
        &self,
        row: &Vec<Value>,
        update: &StatementUpdate,
        expression: Option<Expression>,
        order: Option<Vec<OrderingTerm>>,
        limit: Option<i64>,
        offset: Option<i64>,
    ) -> Result<(), WCDBException> {
        if let Some(order) = order {
            update.order_by(&order);
        }
        if let Some(limit) = limit {
            update.limit(limit);
        }
        if let Some(offset) = offset {
            update.offset(offset);
        }
        if let Some(expression) = expression {
            update.where_expression(expression);
        }
        let handler = self.database.get_handle(true);
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

/// Delete
impl<'a> TableOperation<'a> {
    pub fn delete_value(
        &self,
        expression: Option<Expression>,
        order: Option<OrderingTerm>,
        limit: Option<i64>,
        offset: Option<i64>,
    ) -> Result<(), WCDBException> {
        let binding = StatementDelete::new();
        binding.delete_from(self.table_name.as_ref());
        if let Some(expression) = expression {
            binding.where_expression(expression);
        }
        if let Some(order) = order {
            binding.order_by(&vec![order]);
        }
        if let Some(limit) = limit {
            binding.limit(limit);
        }
        if let Some(offset) = offset {
            binding.offset(offset);
        }
        self.database.get_handle(true).execute(&binding)
    }
}

/// Select
impl TableOperation<'_> {
    pub fn get_values<T>(
        &self,
        columns: Vec<&Field<T>>,
        expression: Option<Expression>,
        order: Option<Vec<OrderingTerm>>,
        limit: Option<i64>,
        offset: Option<i64>,
    ) -> Result<Vec<T>, WCDBException> {
        let handle = self.database.get_handle(false);
        let binding = StatementSelect::new();
        binding.from(self.table_name.as_ref()).select(&columns);
        if let Some(expression) = expression {
            binding.where_expression(&expression);
        }
        if let Some(order) = order {
            binding.order_by(order);
        }
        if let Some(limit) = limit {
            binding.limit(limit);
        }
        if let Some(offset) = offset {
            binding.offset(offset);
        }
        match handle.prepared_with_main_statement(&binding) {
            Ok(statement) => match statement.get_all_objects(&columns) {
                Ok(ret) => Ok(ret),
                Err(err) => Err(err),
            },
            Err(err) => Err(err),
        }
    }
}

impl<'a> TableOperation<'a> {
    pub fn get_handle(&self, write_hint: bool) -> Handle {
        self.database.get_handle(write_hint)
    }
}
