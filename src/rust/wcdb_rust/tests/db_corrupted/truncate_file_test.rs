use crate::db_corrupted::corrupted_base_test_case::CorruptedBaseTestCase;
use wcdb_core::core::handle_orm_operation::HandleORMOperationTrait;
use wcdb_core::core::table_orm_operation::TableORMOperationTrait;

struct TruncateFileTest {
    test_case: CorruptedBaseTestCase,
}

impl TruncateFileTest {
    pub fn new(db_name: &str, auto_backup: bool) -> Self {
        let test_case = CorruptedBaseTestCase::new(db_name, auto_backup);
        TruncateFileTest { test_case }
    }

    pub fn test_case(&self) -> &CorruptedBaseTestCase {
        &self.test_case
    }

    pub fn truncate_file(&self) {}
}

#[cfg(test)]
pub mod truncate_file_exception_test_case {
    #[test]
    pub fn test_truncate_file_then_backup_exception() {
        let db_name = "truncate_file_then_backup_exception_db";
    }
}
