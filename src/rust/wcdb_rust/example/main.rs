use table_coding::WCDBTableCoding;
use wcdb_core::core::database::Database;

#[derive(WCDBTableCoding)]
#[WCDBTable(
    multi_indexes(name = "specifiedNameIndex", columns = ["multiIndex1", "multiIndex2", "multiIndex3"]),
    multi_indexes(columns = ["multiIndex1", "multiIndex2"]),
    multi_primaries = ["multiPrimary1", "multiPrimary2", "multiPrimary3"],
    multi_unique = ["multiUnique1", "multiUnique2", "multiUnique3"],
)]
pub struct TableMessage {
    #[WCDBField]
    multi_primary1: i32,
    #[WCDBField]
    multi_primary2: i32,
    #[WCDBField(column_name = "multiPrimary3")]
    multi_primary: i32,
    #[WCDBField]
    multi_unique1: i32,
    #[WCDBField]
    multi_unique2: i32,
    #[WCDBField(column_name = "multiUnique3")]
    multi_unique: i32,
    #[WCDBField]
    multi_index1: i32,
    #[WCDBField]
    multi_index2: i32,
    #[WCDBField(column_name = "multiIndex3")]
    multi_index: i32,
}

// static DBTABLEMESSAGE_BINDING: Lazy<Binding> = Lazy::new(|| Binding::new());
// 
// static DBTABLEMESSAGE_INSTANCE: Lazy<DbTableMessage> = Lazy::new(|| {
//     let mut instance = DbTableMessage {
//         multi_primary1: std::ptr::null(),
//     };
//     let instance_raw = unsafe { &instance as *const DbTableMessage };
//     let field = Box::new(Field::new("multi_primary1", instance_raw, 1, false, false));
//     let multi_primary1_def =
//         ColumnDef::new_with_column_type(&field.get_column(), ColumnType::Integer);
//     instance.multi_primary1 = unsafe { Box::into_raw(field) };
//     DBTABLEMESSAGE_BINDING.add_column_def(multi_primary1_def);
//     instance
// });
// 
// pub struct TableMessage {
//     pub multi_primary1: i32,
// }
// 
// impl Default for TableMessage {
//     fn default() -> Self {
//         Self { multi_primary1: 0 }
//     }
// }
// 
// pub struct DbTableMessage {
//     pub multi_primary1: *const Field<TableMessage>,
// }
// 
// unsafe impl Send for DbTableMessage {}
// unsafe impl Sync for DbTableMessage {}
// 
// impl TableBinding<TableMessage> for DbTableMessage {
//     fn binding_type(&self) -> TypeId {
//         TypeId::of::<TableMessage>()
//     }
// 
//     fn all_binding_fields(&self) -> Vec<&Field<TableMessage>> {
//         unsafe { vec![&*self.multi_primary1] }
//     }
// 
//     fn base_binding(&self) -> &Binding {
//         &*DBTABLEMESSAGE_BINDING
//     }
// 
//     fn extract_object(
//         &self,
//         fields: Vec<Field<TableMessage>>,
//         prepared_statement: &PreparedStatement,
//     ) -> TableMessage {
//         let mut new_one = TableMessage::default();
//         let mut index = 0;
//         for field in fields {
//             match field.get_field_id() {
//                 1 => new_one.multi_primary1 = prepared_statement.get_int(index),
//                 _ => unreachable!("Unknown field id"),
//             }
//             index += 1;
//         }
//         new_one
//     }
// 
//     fn bind_field(
//         &self,
//         object: &TableMessage,
//         field: &Field<TableMessage>,
//         index: usize,
//         prepared_statement: &mut PreparedStatement,
//     ) {
//         match field.get_field_id() {
//             1 => {
//                 prepared_statement.bind_integer(object.multi_primary1, index);
//             }
//             _ => unreachable!("Unknown field id"),
//         }
//     }
// 
//     fn is_auto_increment(&self, object: &TableMessage) -> bool {
//         false
//     }
// 
//     fn set_last_insert_row_id(&self, object: &mut TableMessage, last_insert_row_id: i64) {}
// }

fn main() {
    let db = Database::new("/Users/zhanglei/Downloads/test.db");
    // db.create_table("rct_message", &*DBTABLEMESSAGE_INSTANCE);
}
