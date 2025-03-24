use darling::FromMeta;

#[repr(i32)]
#[derive(Debug, Default, FromMeta)]
pub enum FTSVersion {
    #[default]
    NONE,
    FTS3,
    FTS4,
    FTS5,
}
