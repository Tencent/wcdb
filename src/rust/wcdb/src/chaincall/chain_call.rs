use crate::base::wcdb_exception::WCDBResult;
use crate::core::handle::Handle;
use crate::winq::statement::StatementTrait;
use std::cell::RefCell;

pub(crate) struct ChainCall<'a, T: StatementTrait> {
    pub(crate) handle: RefCell<Handle<'a>>,
    changes: RefCell<i32>,
    statement: T,
    need_changes: RefCell<bool>,
    auto_invalidate_handle: bool,
}

pub trait ChainCallTrait {
    fn update_changes(&self) -> WCDBResult<()>;

    fn get_statement(&self) -> &dyn StatementTrait;
}

impl<'a, T: StatementTrait> ChainCallTrait for ChainCall<'a, T> {
    fn update_changes(&self) -> WCDBResult<()> {
        if *self.need_changes.borrow() {
            *self.changes.borrow_mut() = self.handle.borrow().get_changes()?;
        }
        Ok(())
    }

    fn get_statement(&self) -> &dyn StatementTrait {
        &self.statement
    }
}

impl<'a, T: StatementTrait> ChainCall<'a, T> {
    pub fn new(
        statement: T,
        handle: Handle<'a>,
        need_changes: bool,
        auto_invalidate_handle: bool,
    ) -> ChainCall<'a, T> {
        ChainCall {
            handle: RefCell::new(handle),
            changes: RefCell::new(0),
            statement,
            need_changes: RefCell::new(need_changes),
            auto_invalidate_handle,
        }
    }

    pub fn get_statement(&self) -> &T {
        &self.statement
    }

    pub fn invalidate_handle(&self) {
        self.handle.borrow_mut().invalidate();
    }
}
