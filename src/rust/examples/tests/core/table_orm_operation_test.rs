use crate::base::base_test_case::TestCaseTrait;
use crate::base::table_test_case::TableTestCase;
use crate::core::table_operation_object::TableOperationObject;
use lazy_static::lazy_static;
use std::sync::{Arc, RwLock};
use wcdb::base::wcdb_exception::WCDBResult;

static TABLE_NAME: &'static str = "table_orm_operation_test_case";
static TABLE_NAME_X: &'static str = "table_orm_operation_test_case_x";

pub struct TableORMOperationTest {
    table_test_case: TableTestCase,
}

impl TestCaseTrait for TableORMOperationTest {
    fn setup(&self) -> WCDBResult<()> {
        self.table_test_case.setup()
    }

    fn teardown(&self) -> WCDBResult<()> {
        self.table_test_case.teardown()
    }
}

impl TableORMOperationTest {
    pub fn new() -> Self {
        TableORMOperationTest {
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
    static ref table_orm_operation_TEST: Arc<RwLock<TableORMOperationTest>> =
        Arc::new(RwLock::new(TableORMOperationTest::new()));
    static ref PRE_INSERT_OBJECTS: Vec<TableOperationObject> = TableOperationObject::get_obj_vec(2);
}

#[cfg(test)]
pub mod table_orm_operation_test_case {
    use crate::base::base_test_case::TestCaseTrait;
    use crate::core::table_operation_object::{
        DbTableOperationObject, TableOperationObject, DB_TABLE_OPERATION_OBJECT_INSTANCE,
    };
    use crate::core::table_orm_operation_test::{table_orm_operation_TEST, TABLE_NAME};
    use core::clone::Clone;
    use core::{assert, assert_eq};
    use std::sync::{Arc, RwLock};
    use wcdb::core::database::Database;
    use wcdb::core::handle_orm_operation::HandleORMOperationTrait;
    use wcdb::core::table_orm_operation::{TableORMOperation, TableORMOperationTrait};
    use wcdb::winq::expression_operable_trait::ExpressionOperableTrait;

    pub fn setup() {
        let arc_clone = Arc::clone(&table_orm_operation_TEST);
        let arc_test = arc_clone.read().expect("test read failure");
        arc_test.setup().expect("setup failure");
    }
    pub fn teardown() {
        let arc_clone = Arc::clone(&table_orm_operation_TEST);
        let arc_test = arc_clone.read().expect("test read failure");
        arc_test.teardown().expect("teardown failure");
    }

    pub fn get_arc_database() -> Arc<RwLock<Database>> {
        let ret = Arc::clone(&table_orm_operation_TEST)
            .read()
            .unwrap()
            .get_table_test_case()
            .get_database();
        Arc::clone(&ret)
    }

    // #[test]
    pub fn test() {
        setup();

        // 获取数据库
        let database_arc = get_arc_database();
        let database = database_arc.read().unwrap();
        // 删除表
        let _ = database.drop_table(TABLE_NAME);

        let ret = database.create_table(TABLE_NAME, &*DB_TABLE_OPERATION_OBJECT_INSTANCE);
        assert!(ret.is_ok());

        let operation =
            TableORMOperation::new(TABLE_NAME, &*DB_TABLE_OPERATION_OBJECT_INSTANCE, &database);
        let obj = TableOperationObject::get_obj();

        // 测试插入数据。
        // insert row
        let ret = operation.insert_objects(vec![obj.clone()], DbTableOperationObject::all_fields());
        assert!(ret.is_ok());

        // insert row
        let ret = operation.insert_objects(vec![obj.clone()], DbTableOperationObject::all_fields());
        assert!(!ret.is_ok());

        // insert or replace
        let ret = operation
            .insert_or_replace_objects(vec![obj.clone()], DbTableOperationObject::all_fields());
        assert!(ret.is_ok());

        // insert or ignore
        let ret = operation
            .insert_or_ignore_objects(vec![obj.clone()], DbTableOperationObject::all_fields());
        assert!(ret.is_ok());

        let field_channel_id = unsafe { DB_TABLE_OPERATION_OBJECT_INSTANCE.channel_id.read() };
        let field_value = unsafe { DB_TABLE_OPERATION_OBJECT_INSTANCE.value.read() };

        // 测试更新数据。
        // update row
        let updated_text = "updated_row";
        let expression = field_channel_id
            .get_column()
            .eq_string(obj.channel_id.as_str());
        let update_obj = TableOperationObject {
            value: updated_text.to_string(),
            ..obj.clone()
        };
        let ret =
            operation.update_object_by_field_expression(update_obj, &field_value, &expression);
        assert!(ret.is_ok());

        // select value
        let expression = field_channel_id
            .get_column()
            .eq_string(obj.channel_id.as_str());
        let ret = operation.get_first_object_by_expression(vec![&field_value], &expression);
        assert!(ret.is_ok());

        let ret_value_opt = ret.unwrap();
        assert_eq!(ret_value_opt.unwrap().value, updated_text);

        // 测试删除数据。
        // delete row
        let expression = field_channel_id
            .get_column()
            .eq_string(obj.channel_id.as_str());
        let ret = operation.delete_objects_by_expression(&expression);
        assert!(ret.is_ok());

        // select value
        let expression = field_channel_id
            .get_column()
            .eq_string(obj.channel_id.as_str());
        let ret = operation.get_all_objects_by_expression(&expression);
        assert!(ret.unwrap().is_empty());

        teardown();
    }
}
