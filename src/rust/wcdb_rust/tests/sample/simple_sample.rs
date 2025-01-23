use table_coding::WCDBTableCoding;

// #[derive(WCDBTableCoding)]
// #[WCDBTable(
//     multi_indexes(name = "specifiedNameIndex", columns = ["id"]),
// )]
// pub struct RCT_MESSAGE {
//     #[WCDBField(is_auto_increment = true, is_primary = true, is_primary = true)]
//     id: i32,
//     #[WCDBField(is_not_null = true)]
//     target_id: String,
//     #[WCDBField]
//     category_id: String,
//     #[WCDBField]
//     message_direction: bool,
//     #[WCDBField]
//     read_status: i16,
//     #[WCDBField]
//     receive_time: i64,
//     #[WCDBField]
//     send_time: i64,
//     #[WCDBField]
//     clazz_name: String,
//     #[WCDBField]
//     content: String,
//     #[WCDBField]
//     send_status: i16,
//     #[WCDBField]
//     sender_id: String,
//     #[WCDBField]
//     extra_content: String,
//     #[WCDBField]
//     extra_column1: i32,
//     #[WCDBField]
//     extra_column2: i32,
//     #[WCDBField]
//     extra_column3: i32,
//     #[WCDBField]
//     extra_column4: i32,
//     #[WCDBField]
//     extra_column5: String,
//     #[WCDBField]
//     extra_column6: String,
//     #[WCDBField]
//     delete_time: i64,
//     #[WCDBField]
//     source: String,
//     #[WCDBField]
//     msg_cuid: i64,
//     #[WCDBField]
//     mute: i32,
//     #[WCDBField]
//     ext_support: i32,
//     #[WCDBField]
//     ext_msg: String,
//     #[WCDBField]
//     channel_id: String,
//     #[WCDBField]
//     has_more: bool,
//     #[WCDBField]
//     deliver_time: i64,
//     #[WCDBField]
//     has_changed: bool,
//     #[WCDBField]
//     mention_me: i32,
//     #[WCDBField]
//     msg_flag: i32,
//     #[WCDBField]
//     disable_update_last_message: bool,
// }
//
// impl RCT_MESSAGE {
//     pub fn new() ->Self {
//         RCT_MESSAGE{
//             id: 0,
//             target_id: "".to_string(),
//             category_id: "".to_string(),
//             message_direction: false,
//             read_status: 0,
//             receive_time: 0,
//             send_time: 0,
//             clazz_name: "".to_string(),
//             content: "".to_string(),
//             send_status: 0,
//             sender_id: "".to_string(),
//             extra_content: "".to_string(),
//             extra_column1: 0,
//             extra_column2: 0,
//             extra_column3: 0,
//             extra_column4: 0,
//             extra_column5: "".to_string(),
//             extra_column6: "".to_string(),
//             delete_time: 0,
//             source: "".to_string(),
//             msg_cuid: 0,
//             mute: 0,
//             ext_support: 0,
//             ext_msg: "".to_string(),
//             channel_id: "".to_string(),
//             has_more: false,
//             deliver_time: 0,
//             has_changed: false,
//             mention_me: 0,
//             msg_flag: 0,
//             disable_update_last_message: false,
//         }
//     }
//
//     pub fn new_with_insert() ->Self {
//         RCT_MESSAGE{
//             id: 0,
//             target_id: "".to_string(),
//             category_id: "".to_string(),
//             message_direction: false,
//             read_status: 0,
//             receive_time: 0,
//             send_time: 0,
//             clazz_name: "".to_string(),
//             content: "".to_string(),
//             send_status: 0,
//             sender_id: "".to_string(),
//             extra_content: "".to_string(),
//             extra_column1: 0,
//             extra_column2: 0,
//             extra_column3: 0,
//             extra_column4: 0,
//             extra_column5: "".to_string(),
//             extra_column6: "".to_string(),
//             delete_time: 0,
//             source: "".to_string(),
//             msg_cuid: 0,
//             mute: 0,
//             ext_support: 0,
//             ext_msg: "".to_string(),
//             channel_id: "".to_string(),
//             has_more: false,
//             deliver_time: 0,
//             has_changed: false,
//             mention_me: 0,
//             msg_flag: 0,
//             disable_update_last_message: false,
//         }
//     }
// }

#[cfg(test)]
pub mod simple_sample {
    use wcdb_core::core::database::Database;
    use wcdb_core::core::handle_orm_operation::HandleORMOperationTrait;
    use wcdb_core::core::table::Table;
    use wcdb_core::core::table_orm_operation::TableORMOperationTrait;

    #[test]
    pub fn sample() { //TableMessageBox {
                      // let database = Database::new("./target/tmp/sample.db");
                      // // database.setCipherKey
                      // // database.setConfig
                      // let table = RCT_MESSAGE::new();
                      // database.create_table("RCT_MESSAGE", &table).unwrap_or(false);
                      // let table = database.get_table("RCT_MESSAGE", &table);
                      //
                      // let table2 = RCT_MESSAGE::new_with_insert();
                      // table.insert_object(table2, RCT_MESSAGE);
    }
}
