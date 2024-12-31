use crate::base::wcdb_exception::WCDBException;

pub type WCDBResult<T> = Result<T, WCDBException>;
