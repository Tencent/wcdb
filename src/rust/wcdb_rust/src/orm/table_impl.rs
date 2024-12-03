use super::table_operation::TableORMOperation;
use crate::database::Database;
use std::marker::PhantomData;

pub struct TableImpl<T> {
    db: Database,
    phantom: PhantomData<T>,
}

impl<T: TableBinding> TableImpl<T> {
    pub fn new(db: Database) -> Self {
        Self {
            db,
            phantom: PhantomData,
        }
    }
}

impl<T: TableBinding> TableORMOperation<T> for TableImpl<T> {
    fn insert(&self, object: &T) -> Result<i64, Box<dyn Error>> {
        let sql = object.get_insert_sql();
        let values = object.get_insert_values();
        
        let stmt = self.db.prepare(&sql)?;
        stmt.bind_parameters(&values)?;
        let id = stmt.execute()?;
        
        Ok(id)
    }

    fn insert_or_replace(&self, object: &T) -> Result<i64, Box<dyn Error>> {
        let sql = object.get_insert_or_replace_sql();
        let values = object.get_insert_values();
        
        let stmt = self.db.prepare(&sql)?;
        stmt.bind_parameters(&values)?;
        let id = stmt.execute()?;
        
        Ok(id)
    }

    fn delete(&self, where_clause: &str) -> Result<i32, Box<dyn Error>> {
        let sql = format!("DELETE FROM {} WHERE {}", T::table_name(), where_clause);
        let affected = self.db.execute(&sql)?;
        Ok(affected)
    }

    fn update(&self, object: &T, where_clause: &str) -> Result<i32, Box<dyn Error>> {
        let (sql, values) = object.get_update_sql(where_clause);
        let stmt = self.db.prepare(&sql)?;
        stmt.bind_parameters(&values)?;
        let affected = stmt.execute()?;
        Ok(affected)
    }

    fn query_all(&self) -> Result<Vec<T>, Box<dyn Error>> {
        self.query_by_where("")
    }

    fn query_by_where(&self, where_clause: &str) -> Result<Vec<T>, Box<dyn Error>> {
        let mut sql = format!("SELECT * FROM {}", T::table_name());
        if !where_clause.is_empty() {
            sql.push_str(&format!(" WHERE {}", where_clause));
        }
        
        let stmt = self.db.prepare(&sql)?;
        let rows = stmt.query_map(|row| T::from_row(row))?;
        
        Ok(rows.collect())
    }

    fn query_by_limit(&self, limit: i32, offset: i32) -> Result<Vec<T>, Box<dyn Error>> {
        let sql = format!(
            "SELECT * FROM {} LIMIT {} OFFSET {}", 
            T::table_name(), 
            limit, 
            offset
        );
        
        let stmt = self.db.prepare(&sql)?;
        let rows = stmt.query_map(|row| T::from_row(row))?;
        
        Ok(rows.collect())
    }

    fn begin_transaction(&self) -> Result<(), Box<dyn Error>> {
        self.db.execute("BEGIN TRANSACTION")?;
        Ok(())
    }

    fn end_transaction(&self) -> Result<(), Box<dyn Error>> {
        self.db.execute("COMMIT")?;
        Ok(())
    }

    fn mark_successful(&self) -> Result<(), Box<dyn Error>> {
        // 在 Rust 中，我们可以通过 RAII 模式来处理事务
        Ok(())
    }

    fn create_table(&self, table_name: &str, binding: &impl TableBinding) -> Result<(), Box<dyn Error>> {
        // 获取创建表的 SQL 语句
        let create_sql = binding.get_create_table_sql(table_name);
        
        // 执行创建表操作
        self.db.execute(&create_sql)?;
        
        // 创建索引等其他操作
        for index_sql in binding.get_create_index_sqls(table_name) {
            self.db.execute(&index_sql)?;
        }
        
        Ok(())
    }
} 