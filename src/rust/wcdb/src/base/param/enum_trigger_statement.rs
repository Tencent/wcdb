use crate::winq::statement_delete::StatementDelete;
use crate::winq::statement_insert::StatementInsert;
use crate::winq::statement_select::StatementSelect;
use crate::winq::statement_update::StatementUpdate;

pub enum TriggerStatement<'a> {
    Insert(&'a StatementInsert),
    Delete(&'a StatementDelete),
    Update(&'a StatementUpdate),
    Select(&'a StatementSelect),
}

impl<'a> From<&'a StatementInsert> for TriggerStatement<'a> {
    fn from(s: &'a StatementInsert) -> Self {
        TriggerStatement::Insert(s)
    }
}

impl<'a> From<&'a StatementDelete> for TriggerStatement<'a> {
    fn from(s: &'a StatementDelete) -> Self {
        TriggerStatement::Delete(s)
    }
}

impl<'a> From<&'a StatementUpdate> for TriggerStatement<'a> {
    fn from(s: &'a StatementUpdate) -> Self {
        TriggerStatement::Update(s)
    }
}

impl<'a> From<&'a StatementSelect> for TriggerStatement<'a> {
    fn from(s: &'a StatementSelect) -> Self {
        TriggerStatement::Select(s)
    }
}
