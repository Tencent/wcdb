use crate::base::wcdb_exception::WCDBException;
use crate::core::handle::Handle;

pub trait Transaction {
    fn inside_transaction(&self, handle: &Handle) -> Result<bool, WCDBException>;
}
