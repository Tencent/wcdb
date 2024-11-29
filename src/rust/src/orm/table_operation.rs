use std::error::Error;

pub trait TableORMOperation<T> {
    // 添加创建表接口
    fn create_table(&self, table_name: &str, binding: &impl TableBinding) -> Result<(), Box<dyn Error>>;
    
    // 基础 CRUD 操作
    fn insert(&self, object: &T) -> Result<i64, Box<dyn Error>>;
    fn insert_or_replace(&self, object: &T) -> Result<i64, Box<dyn Error>>;
    fn delete(&self, where_clause: &str) -> Result<i32, Box<dyn Error>>;
    fn update(&self, object: &T, where_clause: &str) -> Result<i32, Box<dyn Error>>;
    
    // 查询操作
    fn query_all(&self) -> Result<Vec<T>, Box<dyn Error>>;
    fn query_by_where(&self, where_clause: &str) -> Result<Vec<T>, Box<dyn Error>>;
    fn query_by_limit(&self, limit: i32, offset: i32) -> Result<Vec<T>, Box<dyn Error>>;
    
    // 事务操作
    fn begin_transaction(&self) -> Result<(), Box<dyn Error>>;
    fn end_transaction(&self) -> Result<(), Box<dyn Error>>;
    fn mark_successful(&self) -> Result<(), Box<dyn Error>>;
} 