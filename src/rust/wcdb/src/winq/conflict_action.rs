#[repr(i32)]
pub enum ConflictAction {
    None,
    Replace,
    Rollback,
    Abort,
    Fail,
    Ignore,
}
