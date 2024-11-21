#[cfg(test)]
pub mod test_main {
    use wcdb_rust::core::database::Database;

    #[test]
    fn open_db() {
        let db = Database::try_new("test.db");
    }
}
