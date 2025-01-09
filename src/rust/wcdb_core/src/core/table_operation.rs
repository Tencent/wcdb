use crate::base::value::Value;
use crate::base::wcdb_exception::ExceptionCode::OK;
use crate::base::wcdb_exception::{WCDBException, WCDBResult};
use crate::core::database::Database;
use crate::core::handle::Handle;
use crate::core::handle_operation::HandleOperationTrait;
use crate::core::prepared_statement::PreparedStatement;
use crate::orm::field::Field;
use crate::winq::column::Column;
use crate::winq::conflict_action::ConflictAction;
use crate::winq::statement_insert::StatementInsert;
use std::sync::Arc;

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

    fn insert_rows_with_conflict_action<T>(
        &self,
        rows: Vec<Vec<Value>>,
        columns: &Vec<&Field<T>>,
        action: ConflictAction,
    ) -> Result<(), WCDBException> {
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
        if rows.len() > 1 {
            handle.run_transaction(|handle: Handle| {
                self.insert_rows_with_handle(&rows, &insert, &handle)
                    .is_ok()
            })?;
        } else {
            self.insert_rows_with_handle(&rows, &insert, &handle)?;
        }
        Ok(())
    }

    fn insert_rows_with_handle(
        &self,
        rows: &Vec<Vec<Value>>,
        insert: &StatementInsert,
        handle: &Handle,
    ) -> Result<(), WCDBException> {
        let prepared_statement: WCDBResult<Arc<PreparedStatement>> =
            handle.prepared_with_main_statement(insert);
        for row in rows {
            match &prepared_statement {
                Ok(prepared_stmt) => {
                    prepared_stmt.reset();
                    prepared_stmt.bind_row(row);
                    prepared_stmt.step()?
                }
                Err(err) => {}
            }
        }
        match &prepared_statement {
            Ok(prepared_stmt) => {
                prepared_stmt.finalize_statement();
            }
            Err(err) => {}
        }
        Ok(())
    }
}
