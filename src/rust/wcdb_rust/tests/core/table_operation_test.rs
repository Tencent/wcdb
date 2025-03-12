use crate::base::base_test_case::TestCaseTrait;
use crate::base::table_test_case::TableTestCase;
use crate::core::table_operation_object::TableOperationObject;
use lazy_static::lazy_static;
use std::sync::{Arc, RwLock};
use wcdb_core::base::wcdb_exception::WCDBResult;

static TABLE_NAME: &'static str = "table_option_test_case";
static TABLE_NAME_X: &'static str = "table_option_test_case_x";

pub struct TableOperationTest {
    table_test_case: TableTestCase,
}

impl TestCaseTrait for TableOperationTest {
    fn setup(&self) -> WCDBResult<()> {
        self.table_test_case.setup()
    }

    fn teardown(&self) -> WCDBResult<()> {
        self.table_test_case.teardown()
    }
}

impl TableOperationTest {
    pub fn new() -> Self {
        TableOperationTest {
            table_test_case: TableTestCase::new_with_table_name(TABLE_NAME),
        }
    }

    pub fn get_table_test_case(&self) -> &TableTestCase {
        &self.table_test_case
    }

    pub fn get_mut_table_test_case(&mut self) -> &mut TableTestCase {
        &mut self.table_test_case
    }
}

lazy_static! {
    static ref TABLE_OPERATION_TEST: Arc<RwLock<TableOperationTest>> =
        Arc::new(RwLock::new(TableOperationTest::new()));
    static ref PRE_INSERT_OBJECTS: Vec<TableOperationObject> = TableOperationObject::get_obj_vec(2);
}

#[cfg(test)]
pub mod table_operation_test_case {
    use crate::base::base_test_case::TestCaseTrait;
    use crate::core::table_operation_object::{
        DbTableOperationObject, TableOperationObject, DBTABLEOPERATIONOBJECT_INSTANCE,
    };
    use crate::core::table_operation_test::{TABLE_NAME, TABLE_OPERATION_TEST};
    use std::sync::{Arc, RwLock};
    use wcdb_core::base::value::Value;
    use wcdb_core::core::database::Database;
    use wcdb_core::core::handle_orm_operation::HandleORMOperationTrait;
    use wcdb_core::core::table_operation::TableOperation;
    use wcdb_core::winq::expression_operable_trait::ExpressionOperableTrait;

    pub fn setup() {
        let arc_clone = Arc::clone(&TABLE_OPERATION_TEST);
        let arc_test = arc_clone.read().expect("test read failure");
        arc_test.setup().expect("setup failure");
    }
    pub fn teardown() {
        println!("test_insert: 111");
        let arc_clone = Arc::clone(&TABLE_OPERATION_TEST);
        let arc_test = arc_clone.read().expect("test read failure");
        arc_test.teardown().expect("teardown failure");
        println!("test_insert: 112");
    }

    pub fn get_arc_database() -> Arc<RwLock<Database>> {
        let ret = Arc::clone(&TABLE_OPERATION_TEST)
            .read()
            .unwrap()
            .get_table_test_case()
            .get_database();
        Arc::clone(&ret)
    }

    // #[test]
    pub fn test_insert() {
        setup();
        let database_arc = get_arc_database();
        let database = database_arc.read().unwrap();

        let ret = database.create_table(TABLE_NAME, &*DBTABLEOPERATIONOBJECT_INSTANCE);
        assert!(ret.is_ok());

        let operation = TableOperation::new(TABLE_NAME, &database);
        let obj = TableOperationObject::get_obj();
        let field_value = unsafe { DBTABLEOPERATIONOBJECT_INSTANCE.value.read() };
        let field_channel_id = unsafe { DBTABLEOPERATIONOBJECT_INSTANCE.channel_id.read() };

        // insert row
        let ret = operation.insert_rows(
            vec![obj.get_values_vec()],
            &DbTableOperationObject::all_fields(),
        );
        assert!(ret.is_ok());

        // insert row
        let ret = operation.insert_rows(
            vec![obj.get_values_vec()],
            &DbTableOperationObject::all_fields(),
        );
        assert!(!ret.is_ok());

        // insert or replace
        let ret = operation.insert_rows_or_replace(
            vec![obj.get_values_vec()],
            &DbTableOperationObject::all_fields(),
        );
        assert!(ret.is_ok());

        // insert or ignore
        let objs = TableOperationObject::get_obj_vec(2);
        let values = objs.iter().map(|v| v.get_values_vec()).collect();
        let ret = operation.insert_rows_or_ignore(values, &DbTableOperationObject::all_fields());
        assert!(ret.is_ok());

        teardown();
    }

    // #[test]
    pub fn test_update() {
        setup();

        let database_arc = get_arc_database();
        let database = database_arc.read().unwrap();

        let ret = database.create_table(TABLE_NAME, &*DBTABLEOPERATIONOBJECT_INSTANCE);
        assert!(ret.is_ok());

        let operation = TableOperation::new(TABLE_NAME, &database);
        let obj = TableOperationObject::get_obj();
        let field_value = unsafe { DBTABLEOPERATIONOBJECT_INSTANCE.value.read() };
        let field_channel_id = unsafe { DBTABLEOPERATIONOBJECT_INSTANCE.channel_id.read() };

        // insert row
        let ret = operation.insert_rows(
            vec![obj.get_values_vec()],
            &DbTableOperationObject::all_fields(),
        );
        assert!(ret.is_ok());

        // update row
        let updated_text = "updated_row";
        let updated_value = Value::from(updated_text);
        let expression = field_channel_id
            .get_column()
            .eq_string(obj.channel_id.as_str());
        let ret = operation.update_row(
            &vec![updated_value],
            &vec![&field_value],
            Some(expression),
            None,
            None,
            None,
        );
        assert!(ret.is_ok());

        // select value
        let expression = field_channel_id
            .get_column()
            .eq_string(obj.channel_id.as_str());
        let ret = operation.get_values(vec![&field_value], Some(expression), None, None, None);
        assert!(ret.is_ok());

        let ret_value = ret.unwrap();
        let item = ret_value.first().unwrap();
        assert_eq!(item.value, updated_text);

        teardown();
    }

    // #[test]
    pub fn test_delete() {
        setup();

        let database_arc = get_arc_database();
        let database = database_arc.read().unwrap();

        let ret = database.create_table(TABLE_NAME, &*DBTABLEOPERATIONOBJECT_INSTANCE);
        assert!(ret.is_ok());

        let operation = TableOperation::new(TABLE_NAME, &database);
        let obj = TableOperationObject::get_obj();
        let field_value = unsafe { DBTABLEOPERATIONOBJECT_INSTANCE.value.read() };
        let field_channel_id = unsafe { DBTABLEOPERATIONOBJECT_INSTANCE.channel_id.read() };

        // insert row
        let ret = operation.insert_rows(
            vec![obj.get_values_vec()],
            &DbTableOperationObject::all_fields(),
        );
        assert!(ret.is_ok());

        // select value
        let expression = field_channel_id
            .get_column()
            .eq_string(obj.channel_id.as_str());
        let ret = operation.get_values(vec![&field_value], Some(expression), None, None, None);
        assert!(ret.is_ok());

        let ret_value = ret.unwrap();
        let item = ret_value.first().unwrap();
        assert_eq!(item.value, obj.value);

        // delete row
        let expression = field_channel_id
            .get_column()
            .eq_string(obj.channel_id.as_str());
        let ret = operation.delete_value(Some(expression), None, None, None);
        assert!(ret.is_ok());

        // select value
        let expression = field_channel_id
            .get_column()
            .eq_string(obj.channel_id.as_str());
        let ret = operation.get_values(vec![&field_value], Some(expression), None, None, None);
        assert!(ret.is_ok());
        assert_eq!(ret.unwrap().len(), 0);

        teardown();
    }
}
