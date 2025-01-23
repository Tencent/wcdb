use wcdb_core::winq::identifier::IdentifierTrait;

pub struct WinqTool {}

impl WinqTool {
    pub fn winq_equal(winq: &impl IdentifierTrait, sql: &str) {
        assert_eq!(sql, winq.get_description())
    }
}
