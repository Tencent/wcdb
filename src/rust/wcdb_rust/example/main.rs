use table_binding_derive::TableBinding;
use wcdb_rust::core::database::Database;

#[derive(TableBinding)]
struct TableMessage {}

fn main() {
    let db = Database::new("test.db");
}
