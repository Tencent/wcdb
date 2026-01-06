use crate::base::base_test_case::TestCaseTrait;
use crate::base::database_test_case::{DatabaseTestCase, Expect, TestOperation};
use crate::base::wrapped_value::WrappedValue;
use crate::orm::testclass::auto_add_column_object::{AutoAddColumnObject, DbAutoAddColumnObject};
use crate::orm::testclass::column_rename_object::{
    ColumnRenameObjectOld, DbColumnRenameObjectNew, DbColumnRenameObjectOld,
    DB_COLUMN_RENAME_OBJECT_NEW_INSTANCE, DB_COLUMN_RENAME_OBJECT_OLD_INSTANCE,
};
use crate::orm::testclass::table_primary_key_object::{
    DbTablePrimaryKeyObjectOld, TablePrimaryKeyObjectOld, DB_TABLE_PRIMARY_KEY_OBJECT_NEW_INSTANCE,
    DB_TABLE_PRIMARY_KEY_OBJECT_OLD_INSTANCE,
};
use rand::Rng;
use std::cmp::PartialEq;
use wcdb::base::wcdb_exception::{WCDBException, WCDBResult};
use wcdb::core::handle_operation::HandleOperationTrait;
use wcdb::core::handle_orm_operation::HandleORMOperationTrait;
use wcdb::core::table_orm_operation::TableORMOperationTrait;
use wcdb::orm::field::Field;
use wcdb::orm::table_binding::TableBinding;
use wcdb::winq::column_def::ColumnDef;
use wcdb::winq::column_type::ColumnType;
use wcdb::winq::expression::Expression;
use wcdb::winq::identifier::IdentifierTrait;
use wcdb::winq::statement_alter_table::StatementAlterTable;
use wcdb::winq::statement_create_table::StatementCreateTable;

pub struct OrmTest {
    database_test_case: DatabaseTestCase,
    table_name: String,
}

impl OrmTest {
    pub fn new() -> Self {
        OrmTest {
            database_test_case: DatabaseTestCase::new(),
            table_name: "testTable".to_string(),
        }
    }

    fn do_test_create_table_and_index_sqls_as_expected<CB>(&self, sqls: Vec<String>, operation: CB)
    where
        CB: FnOnce() -> WCDBResult<()>,
    {
        assert!(sqls.len() > 0);
        let mut new_sql_vec = vec![];
        new_sql_vec.push("BEGIN IMMEDIATE".to_string());
        new_sql_vec.extend(sqls);
        new_sql_vec.push("COMMIT".to_string());
        let _ = self
            .database_test_case
            .do_test_sql_vec(new_sql_vec, operation);
    }

    fn do_test_auto_add_column(
        &self,
        fake_table: &str,
        remove_filed: &Field<AutoAddColumnObject>,
        succeed: bool,
        operation: TestOperation,
    ) -> WCDBResult<()> {
        let column_name = remove_filed.get_name();
        let statement = StatementCreateTable::new();
        let create_table = statement.create_table(fake_table);
        let mut column_defs = vec![];
        DbAutoAddColumnObject::all_fields()
            .iter()
            .for_each(|field| {
                if field.get_description().as_str() != column_name {
                    let column_def = ColumnDef::new((field.get_column(), ColumnType::Integer));
                    column_defs.push(column_def);
                }
            });

        let create_table = create_table.define(column_defs.iter().collect());
        let _ = self.database_test_case.execute(create_table); // todo qixinbing 直接 ? 会报错

        let mut added = WrappedValue::new();
        added.bool_value = true;
        self.database_test_case
            .trace_exception(Some(move |exp: WCDBException| {
                if exp.message() != "Auto add column".to_string() {
                    return;
                }
                // added.bool_value = true;
                // todo qixinbing
            }));
        let mut has_error = false;
        let operation_ret = operation();
        match operation_ret {
            Ok(_) => {}
            Err(exp) => {
                let msg = exp.message();
                println!("qxb operation_ret {}", msg);
                has_error = true;
            }
        }
        assert_eq!(succeed, !has_error);
        assert_eq!(succeed, added.bool_value);
        self.database_test_case.drop_table(fake_table)?;
        // self.database_test_case.get_database().read().unwrap().trace_exception(None);

        Ok(())
    }

    fn do_table_update_primary_key(&self, table_name: &str, table_name_back: &str, data_num: i32) {
        // create old table
        self.database_test_case
            .get_database()
            .read()
            .unwrap()
            .create_table(table_name, &*DB_TABLE_PRIMARY_KEY_OBJECT_OLD_INSTANCE)
            .unwrap();

        // insert db to old table
        let obj_vec = TablePrimaryKeyObjectOld::get_old_obj_vec(data_num);

        self.database_test_case
            .get_database()
            .write()
            .unwrap()
            .insert_objects(
                obj_vec,
                DbTablePrimaryKeyObjectOld::all_fields(),
                table_name,
            )
            .unwrap();

        // rename old table
        let statement_alert_table = StatementAlterTable::new();
        self.database_test_case
            .get_database()
            .write()
            .unwrap()
            .execute(
                statement_alert_table
                    .alter_table(table_name)
                    .rename_to(table_name_back),
            )
            .unwrap();

        // create new table
        self.database_test_case
            .get_database()
            .read()
            .unwrap()
            .create_table(table_name, &*DB_TABLE_PRIMARY_KEY_OBJECT_NEW_INSTANCE)
            .unwrap();

        // insert old table to new table
        let sql = format!(
            "INSERT OR REPLACE INTO {} SELECT * FROM {};",
            table_name, table_name_back
        );
        self.database_test_case
            .get_database()
            .write()
            .unwrap()
            .execute_sql(sql.as_str())
            .unwrap();

        // drop old table
        self.database_test_case
            .get_database()
            .write()
            .unwrap()
            .drop_table(table_name_back)
            .unwrap();
    }

    fn do_column_rename(
        &self,
        table_name: &str,
        data_num: i32,
        column_name_old: &str,
        column_name_new: &str,
    ) {
        // create old table
        self.database_test_case
            .get_database()
            .read()
            .unwrap()
            .create_table(table_name, &*DB_COLUMN_RENAME_OBJECT_OLD_INSTANCE)
            .unwrap();

        // insert test date to old table
        let obj_vec = ColumnRenameObjectOld::get_old_obj_vec(data_num);
        self.database_test_case
            .get_database()
            .write()
            .unwrap()
            .insert_objects(obj_vec, DbColumnRenameObjectOld::all_fields(), table_name)
            .unwrap();

        // old table column rename
        let statement_alert_table = StatementAlterTable::new();
        let statement = statement_alert_table
            .alter_table(table_name)
            .rename_column_by_name(column_name_old)
            .to_column_by_name(column_name_new);
        self.database_test_case
            .get_database()
            .write()
            .unwrap()
            .execute(statement)
            .unwrap();

        // open new table
        self.database_test_case
            .get_database()
            .read()
            .unwrap()
            .create_table(table_name, &*DB_COLUMN_RENAME_OBJECT_NEW_INSTANCE)
            .unwrap();

        // check
        let ret = self
            .database_test_case
            .get_database()
            .read()
            .unwrap()
            .get_all_objects(
                DbColumnRenameObjectNew::all_fields(),
                table_name,
                None,
                None,
                None,
                None,
            );
        match ret {
            Ok(new_obj_vec) => {
                assert_eq!(new_obj_vec.len(), data_num as usize);
                for new_obj in new_obj_vec {
                    assert!(new_obj.is_same());
                }
            }
            Err(err) => {
                assert!(false);
            }
        }
    }
}

impl TestCaseTrait for OrmTest {
    fn setup(&self) -> WCDBResult<()> {
        self.database_test_case.setup()?;
        self.database_test_case.set_expect_mode(Expect::SomeSQLs);
        Ok(())
    }

    fn teardown(&self) -> WCDBResult<()> {
        self.database_test_case.teardown()
    }
}

#[cfg(test)]
pub mod orm_test {
    use super::*;
    use crate::orm::testclass::all_type_object::{
        AllTypeObjectHelper, DbAllTypeObject, DB_ALL_TYPE_OBJECT_INSTANCE,
    };
    use crate::orm::testclass::auto_add_column_object::{
        DbAutoAddColumnObject, DB_AUTO_ADD_COLUMN_OBJECT_INSTANCE,
    };
    use crate::orm::testclass::field_object::DbFieldObject;
    use crate::orm::testclass::primary_enable_auto_increment_object::{
        DbPrimaryEnableAutoIncrementObject, PrimaryEnableAutoIncrementObject,
        DB_PRIMARY_ENABLE_AUTO_INCREMENT_OBJECT_INSTANCE,
    };
    use crate::orm::testclass::primary_not_auto_increment_object::{
        DbPrimaryNotAutoIncrementObject, PrimaryNotAutoIncrementObject,
        DB_PRIMARY_NOT_AUTO_INCREMENT_OBJECT_INSTANCE,
    };
    use crate::orm::testclass::table_constraint_object::DB_TABLE_CONSTRAINT_OBJECT_INSTANCE;
    use wcdb::winq::expression_operable::ExpressionOperableTrait;

    fn setup(orm_test: &OrmTest) {
        orm_test.setup().unwrap();
    }

    fn teardown(orm_test: &OrmTest) {
        orm_test.teardown().unwrap();
    }

    #[test]
    fn test_all_field() {
        assert_eq!(DbFieldObject::all_fields().len(), 2);

        let orm_test = OrmTest::new();
        let binding = orm_test.database_test_case.get_database();
        let database_lock = binding.read().unwrap();
        let binding = DbFieldObject::all_fields();
        let first_field = binding.first().unwrap();
        assert_eq!(first_field.get_description(), "field");

        let second_field = binding.last().unwrap();
        assert_eq!(second_field.get_description(), "differentName");
    }

    #[test]
    fn test_all_type() {
        let orm_test = OrmTest::new();
        setup(&orm_test);

        let table_name = "table_all_type".to_string();

        let mut sql_vec = vec![];
        // sql_vec.push("CREATE TABLE IF NOT EXISTS table_all_type(field_type TEXT, a_bool INTEGER, a_byte INTEGER, a_short INTEGER, a_int INTEGER, a_long INTEGER, a_float REAL, a_double REAL, a_string TEXT)".to_string());
        sql_vec.push("CREATE TABLE IF NOT EXISTS table_all_type(field_type TEXT , a_bool INTEGER , a_bool2 INTEGER , a_byte INTEGER , a_byte2 INTEGER , a_short INTEGER , a_short2 INTEGER , a_int INTEGER , a_int2 INTEGER , a_long INTEGER , a_long2 INTEGER , a_float REAL , a_float2 REAL , a_double REAL , a_double2 REAL , a_string TEXT , a_string2 TEXT , a_blob BLOB , a_blob2 BLOB )".to_string());

        orm_test.do_test_create_table_and_index_sqls_as_expected(sql_vec, || {
            orm_test
                .database_test_case
                .create_table(table_name.as_str(), &*DB_ALL_TYPE_OBJECT_INSTANCE)?;
            Ok(())
        });

        let binding = orm_test.database_test_case.get_database();
        let database_lock = binding.read().unwrap();
        let table = database_lock.get_table(table_name.as_str(), &*DB_ALL_TYPE_OBJECT_INSTANCE);

        let max = AllTypeObjectHelper::max_object();
        let min = AllTypeObjectHelper::min_object();
        let random = AllTypeObjectHelper::random_object();
        let empty = AllTypeObjectHelper::empty_object();

        let obj_vec = vec![max.clone(), min.clone(), random.clone(), empty.clone()];
        let _ = table.insert_objects(obj_vec, Some(DbAllTypeObject::all_fields()));

        let exp =
            Expression::new(DbAllTypeObject::field_type().get_column()).eq(max.field_type.as_str());
        let db_max_opt = table
            .get_first_object(Some(DbAllTypeObject::all_fields()), Some(&exp), None, None)
            .unwrap();
        assert!(max == db_max_opt.unwrap());

        let exp =
            Expression::new(DbAllTypeObject::field_type().get_column()).eq(min.field_type.as_str());
        let db_min_opt = table
            .get_first_object(Some(DbAllTypeObject::all_fields()), Some(&exp), None, None)
            .unwrap();
        assert!(min == db_min_opt.unwrap());

        let exp = Expression::new(DbAllTypeObject::field_type().get_column())
            .eq(empty.field_type.as_str());
        let db_empty_opt = table
            .get_first_object(Some(DbAllTypeObject::all_fields()), Some(&exp), None, None)
            .unwrap();
        assert!(empty == db_empty_opt.unwrap());

        let exp = Expression::new(DbAllTypeObject::field_type().get_column())
            .eq(random.field_type.as_str());
        let db_random_opt = table
            .get_first_object(Some(DbAllTypeObject::all_fields()), Some(&exp), None, None)
            .unwrap();
        assert!(random == db_random_opt.unwrap());

        teardown(&orm_test);
    }

    #[test]
    fn test_table_constraint() {
        let orm_test = OrmTest::new();
        setup(&orm_test);

        let table_name = orm_test.table_name.as_str();

        let mut sql_vec = vec![];
        sql_vec.push("CREATE TABLE IF NOT EXISTS testTable(multiPrimary1 INTEGER , multiPrimary2 INTEGER , multiPrimary3 INTEGER , multiUnique1 INTEGER , multiUnique2 INTEGER , multiUnique3 INTEGER , multiIndex1 INTEGER , multiIndex2 INTEGER , multiIndex3 INTEGER , UNIQUE(multiUnique1, multiUnique2, multiUnique3), PRIMARY KEY(multiPrimary1, multiPrimary2, multiPrimary3))".to_string());
        sql_vec.push("CREATE INDEX IF NOT EXISTS specifiedNameIndex ON testTable(multiIndex1, multiIndex2, multiIndex3)".to_string());
        sql_vec.push("CREATE INDEX IF NOT EXISTS testTable_multiIndex1_multiIndex2_index ON testTable(multiIndex1, multiIndex2)".to_string());

        orm_test.do_test_create_table_and_index_sqls_as_expected(sql_vec, || {
            orm_test
                .database_test_case
                .create_table(table_name, &*DB_TABLE_CONSTRAINT_OBJECT_INSTANCE)
                .unwrap();
            Ok(())
        });

        teardown(&orm_test);
    }

    // #[test]
    fn test_auto_add_column() {
        let orm_test = OrmTest::new();
        setup(&orm_test);

        let fake_table = "fakeTable";
        let fake_schema = "notExistSchema";
        orm_test
            .database_test_case
            .create_table(fake_table, &*DB_AUTO_ADD_COLUMN_OBJECT_INSTANCE)
            .unwrap();

        let obj = DbAutoAddColumnObject::default();
        let mut insert_value = unsafe { &*obj.insert_value };
        DbAutoAddColumnObject::all_fields()
            .iter()
            .for_each(|field| {
                if field.get_name() == "insertValue" {
                    insert_value = *field;
                }
            });

        let database_test_case_clone = orm_test.database_test_case.clone();
        orm_test
            .do_test_auto_add_column(
                fake_table,
                insert_value,
                true,
                Box::new(move || {
                    let self_table_name = fake_table.to_string();
                    database_test_case_clone.insert_object(
                        AutoAddColumnObject::new(),
                        DbAutoAddColumnObject::all_fields(),
                        self_table_name.as_str(),
                    )?;
                    Ok(())
                }),
            )
            .unwrap();

        let mut update_value = unsafe { &*obj.update_value };
        DbAutoAddColumnObject::all_fields()
            .iter()
            .for_each(|field| {
                if field.get_name() == "updateValue" {
                    update_value = *field;
                }
            });

        orm_test
            .do_test_auto_add_column(
                fake_table,
                update_value,
                true,
                Box::new(move || {
                    // todo qixinbing
                    Ok(())
                }),
            )
            .unwrap();

        // todo qixinbing
        teardown(&orm_test);
    }

    #[test]
    fn test_primary_key_enable_auto_increment_for_existing_table() {
        let orm_test = OrmTest::new();
        setup(&orm_test);

        let binding = orm_test.database_test_case.get_database();
        let database_lock = binding.read().unwrap();
        // let table_name = orm_test.table_name.as_str(); 见 DatabaseTestCase setup 方法
        let table_name = "testTable2";

        database_lock
            .create_table(table_name, &*DB_PRIMARY_NOT_AUTO_INCREMENT_OBJECT_INSTANCE)
            .unwrap();
        let mut obj1 = PrimaryNotAutoIncrementObject::new();
        obj1.id = 1;
        database_lock
            .insert_object(
                obj1,
                DbPrimaryNotAutoIncrementObject::all_fields(),
                table_name,
            )
            .unwrap();

        database_lock
            .create_table(
                table_name,
                &*DB_PRIMARY_ENABLE_AUTO_INCREMENT_OBJECT_INSTANCE,
            )
            .unwrap();
        database_lock
            .delete_objects(table_name, None, None, None, None)
            .unwrap();

        let obj2 = PrimaryEnableAutoIncrementObject::new();
        database_lock
            .insert_object(
                obj2,
                DbPrimaryEnableAutoIncrementObject::all_fields(),
                table_name,
            )
            .unwrap();
        let obj_vec = database_lock
            .get_all_objects(
                DbPrimaryEnableAutoIncrementObject::all_fields(),
                table_name,
                None,
                None,
                None,
                None,
            )
            .unwrap();
        assert_eq!(obj_vec.last().unwrap().id, 2);

        teardown(&orm_test);
    }

    // 新增的升级单测，Java 没有该用例
    #[test]
    fn test_table_update_primary_key() {
        // todo qixinbing 上线需要配合版本控制
        let orm_test = OrmTest::new();
        orm_test.setup().unwrap();

        let mut sql_vec = vec![];
        sql_vec.push("CREATE TABLE IF NOT EXISTS table_primary_key(id INTEGER , name TEXT , price REAL , desc TEXT , PRIMARY KEY(id))".to_string());
        sql_vec.push("ALTER TABLE table_primary_key RENAME TO table_primary_key_back".to_string());
        sql_vec.push("CREATE TABLE IF NOT EXISTS table_primary_key(id INTEGER , name TEXT , price REAL , desc TEXT , PRIMARY KEY(id, name))".to_string());
        sql_vec.push(
            "INSERT OR REPLACE INTO table_primary_key SELECT * FROM table_primary_key_back;"
                .to_string(),
        );
        sql_vec.push("DROP TABLE IF EXISTS table_primary_key_back".to_string());

        orm_test.do_test_create_table_and_index_sqls_as_expected(sql_vec, || {
            let table_name = "table_primary_key";
            let table_name_back = "table_primary_key_back";
            let data_num = 15;
            orm_test.do_table_update_primary_key(table_name, table_name_back, data_num);
            Ok(())
        });

        orm_test.teardown().unwrap();
    }

    // 新增的升级单测，Java 没有该用例
    #[test]
    fn test_column_rename() {
        let orm_test = OrmTest::new();
        orm_test.setup().unwrap();

        let mut sql_vec = vec![];
        sql_vec.push("CREATE TABLE IF NOT EXISTS column_rename_object(category INTEGER , target_id TEXT , channel_id TEXT )".to_string());
        sql_vec.push(
            "ALTER TABLE column_rename_object RENAME COLUMN category TO conversation_type"
                .to_string(),
        );
        sql_vec.push(
            "SELECT conversation_type, target_id, channel_id FROM column_rename_object".to_string(),
        );

        orm_test.do_test_create_table_and_index_sqls_as_expected(sql_vec, || {
            let table_name = "column_rename_object";
            let column_name_old = "category";
            let column_name_new = "conversation_type";
            let data_num = 8;
            orm_test.do_column_rename(table_name, data_num, column_name_old, column_name_new);
            Ok(())
        });

        orm_test.teardown().unwrap();
    }
}
