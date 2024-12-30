#[derive(Debug)]
pub enum WCDBError {
    Exception,
}

pub type WCDBResult<T> = Result<T, WCDBError>;
