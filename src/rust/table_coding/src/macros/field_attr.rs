use crate::macros::wcdb_default::WCDBDefault;
use crate::macros::wcdb_index::WCDBIndex;
use darling::FromMeta;

#[derive(Debug, FromMeta)]
pub(crate) struct FieldAttr {
    #[darling(default)]
    index: Option<WCDBIndex>,
    #[darling(default)]
    default: Option<WCDBDefault>,
}

impl FieldAttr {
    pub fn index(&self) -> &Option<WCDBIndex> {
        &self.index
    }

    pub fn default(&self) -> &Option<WCDBDefault> {
        &self.default
    }
}
