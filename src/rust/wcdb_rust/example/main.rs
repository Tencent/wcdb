use once_cell::sync::Lazy;
use std::any::TypeId;
use wcdb_core::base::result_code::ResultCode;
use wcdb_core::core::database::Database;
use wcdb_core::core::prepared_statement::PreparedStatement;
use wcdb_core::orm::binding::Binding;
use wcdb_core::orm::field::Field;
use wcdb_core::orm::table_binding::TableBinding;

pub struct DbTableMessage<'a> {
    pub multi_primary1: Field<'a, DbTableMessage<'a>>,
}

static DBTABLEMESSAGE_BINDING: Lazy<Binding> = Lazy::new(|| {
    Binding::new()
});

static DBTABLEMESSAGE_INSTANCE: Lazy<DbTableMessage> = Lazy::new(|| {
    DbTableMessage {
        multi_primary1: Field::new("multi_primary1", &*DBTABLEMESSAGE_INSTANCE, 1, false, false),
    }
});

unsafe impl Send for DbTableMessage<'_> {}
unsafe impl<'a> Sync for DbTableMessage<'a> {}

impl<'a> TableBinding<DbTableMessage<'a>> for DbTableMessage<'a> {
    fn binding_type(&self) -> TypeId {
        TypeId::of::<DbTableMessage>()
    }

    fn all_binding_fields(&self) -> Vec<Field<'a, DbTableMessage<'a>>> {
        todo!()
    }

    fn base_binding(&self) -> &Binding {
        &*DBTABLEMESSAGE_BINDING
    }

    fn extract_object(
        &self,
        fields: Vec<Field<'_, DbTableMessage<'_>>>,
        prepared_statement: &PreparedStatement,
    ) -> Result<DbTableMessage<'a>, ResultCode> {
        todo!()
    }

    fn bind_field(&self, object: &DbTableMessage, field: &Field<DbTableMessage>, index: usize, prepared_statement: &mut PreparedStatement) {
        todo!()
    }

    fn is_auto_increment(&self, object: &DbTableMessage) -> bool {
        false
    }

    fn set_last_insert_row_id(&self, object: &mut DbTableMessage, last_insert_row_id: i64) {}
}

fn main() {
    let db = Database::new("test.db");
    db.create_table("rct_message", &*DBTABLEMESSAGE_INSTANCE);
}
