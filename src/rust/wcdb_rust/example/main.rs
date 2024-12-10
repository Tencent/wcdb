use once_cell::sync::Lazy;
use std::any::TypeId;
use wcdb_core::core::database::Database;
use wcdb_core::core::prepared_statement::PreparedStatement;
use wcdb_core::orm::binding::Binding;
use wcdb_core::orm::field::Field;
use wcdb_core::orm::table_binding::TableBinding;
use wcdb_core::winq::column_def::ColumnDef;
use wcdb_core::winq::column_type::ColumnType;

pub struct TableMessage {
    pub multi_primary1: i32,
}

impl Default for TableMessage {
    fn default() -> Self {
        Self { multi_primary1: 0 }
    }
}

pub struct DbTableMessage {
    pub multi_primary1: *const Field<TableMessage>,
}

static DBTABLEMESSAGE_BINDING: Lazy<Binding> = Lazy::new(|| Binding::new());

static DBTABLEMESSAGE_INSTANCE: Lazy<DbTableMessage> = Lazy::new(|| {
    let mut instance = DbTableMessage {
        multi_primary1: std::ptr::null(),
    };
    let instance_raw = unsafe { &instance as *const DbTableMessage };
    let field = Box::new(Field::new("multi_primary1", instance_raw, 1, false, false));
    let multi_primary1_def =
        ColumnDef::new_with_column_type(&field.get_column(), ColumnType::Integer);
    instance.multi_primary1 = unsafe { Box::into_raw(field) };
    DBTABLEMESSAGE_BINDING.add_column_def(multi_primary1_def);
    instance
});

unsafe impl Send for DbTableMessage {}
unsafe impl Sync for DbTableMessage {}

impl TableBinding<TableMessage> for DbTableMessage {
    fn binding_type(&self) -> TypeId {
        TypeId::of::<TableMessage>()
    }

    fn all_binding_fields(&self) -> Vec<&Field<TableMessage>> {
        unsafe { vec![&*self.multi_primary1] }
    }

    fn base_binding(&self) -> &Binding {
        &*DBTABLEMESSAGE_BINDING
    }

    fn extract_object(
        &self,
        fields: Vec<Field<TableMessage>>,
        prepared_statement: &PreparedStatement,
    ) -> TableMessage {
        let mut new_one = TableMessage::default();
        let mut index = 0;
        for field in fields {
            match field.get_field_id() {
                1 => new_one.multi_primary1 = prepared_statement.get_int(index),
                _ => unreachable!("Unknown field id"),
            }
            index += 1;
        }
        new_one
    }

    fn bind_field(
        &self,
        object: &TableMessage,
        field: &Field<TableMessage>,
        index: usize,
        prepared_statement: &mut PreparedStatement,
    ) {
        match field.get_field_id() {
            1 => {
                prepared_statement.bind_integer(object.multi_primary1, index);
            }
            _ => unreachable!("Unknown field id"),
        }
    }

    fn is_auto_increment(&self, object: &TableMessage) -> bool {
        false
    }

    fn set_last_insert_row_id(&self, object: &mut TableMessage, last_insert_row_id: i64) {}
}

fn main() {
    let db = Database::new("/Users/zhanglei/Downloads/test.db");
    db.create_table("rct_message", &*DBTABLEMESSAGE_INSTANCE);
}
