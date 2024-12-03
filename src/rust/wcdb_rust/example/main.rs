use table_binding::TableBinding;
use wcdb_core::core::database::Database;

#[derive(TableBinding)]
pub struct TableMessage {
    #[WCDBField]
    pub multi_primary1: i32,
    #[WCDBField]
    pub multi_primary2: String,
}

// pub struct DbTableMessage {
//     pub multi_primary1: wcdb_core::orm::field::Field<TableMessage>,
// }

fn main() {
    let db = Database::new("test.db");
}
