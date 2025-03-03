use crate::base::base_test_case::TestCaseTrait;
use crate::base::database_test_case::{DatabaseTestCase, Expect, TestOperation};
use crate::base::wrapped_value::WrappedValue;
use crate::orm::testclass::auto_add_column_object::{AutoAddColumnObject, DbAutoAddColumnObject};
use rand::Rng;
use std::cmp::PartialEq;
use wcdb_core::base::wcdb_exception::{WCDBException, WCDBResult};
use wcdb_core::core::handle_orm_operation::HandleORMOperationTrait;
use wcdb_core::core::table_orm_operation::TableORMOperationTrait;
use wcdb_core::orm::field::Field;
use wcdb_core::orm::table_binding::TableBinding;
use wcdb_core::winq::column::Column;
use wcdb_core::winq::column_def::ColumnDef;
use wcdb_core::winq::column_type::ColumnType;
use wcdb_core::winq::expression::Expression;
use wcdb_core::winq::expression_operable_trait::ExpressionOperableTrait;
use wcdb_core::winq::identifier::IdentifierTrait;
use wcdb_core::winq::statement_create_table::StatementCreateTable;

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
                    let column_def =
                        ColumnDef::new_with_column_type(field.get_column(), ColumnType::Integer);
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
}

impl TestCaseTrait for OrmTest {
    fn setup(&self) -> WCDBResult<()> {
        self.database_test_case.setup()?;
        self.database_test_case.set_expect_mode(Expect::SomeSQLs);
        Ok(())
    }

    fn teardown(&self) -> WCDBResult<()> {
        Ok(())
    }
}

#[cfg(test)]
pub mod orm_test {
    use super::*;
    use crate::orm::testclass::all_type_object::{
        AllTypeObjectHelper, DbAllTypeObject, DBALLTYPEOBJECT_INSTANCE,
    };
    use crate::orm::testclass::auto_add_column_object::{
        DbAutoAddColumnObject, DBAUTOADDCOLUMNOBJECT_INSTANCE,
    };
    use crate::orm::testclass::field_object::DbFieldObject;
    use crate::orm::testclass::primary_enable_auto_increment_object::{
        DbPrimaryEnableAutoIncrementObject, PrimaryEnableAutoIncrementObject,
        DBPRIMARYENABLEAUTOINCREMENTOBJECT_INSTANCE,
    };
    use crate::orm::testclass::primary_not_auto_increment_object::{
        DbPrimaryNotAutoIncrementObject, PrimaryNotAutoIncrementObject,
        DBPRIMARYNOTAUTOINCREMENTOBJECT_INSTANCE,
    };
    use crate::orm::testclass::table_constraint_object::DBTABLECONSTRAINTOBJECT_INSTANCE;

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
        sql_vec.push("CREATE TABLE IF NOT EXISTS table_all_type(field_type TEXT, a_bool INTEGER, a_byte INTEGER, a_short INTEGER, a_int INTEGER, a_long INTEGER, a_float REAL, a_double REAL, a_string TEXT)".to_string());

        orm_test.do_test_create_table_and_index_sqls_as_expected(sql_vec, || {
            orm_test
                .database_test_case
                .create_table(table_name.as_str(), &*DBALLTYPEOBJECT_INSTANCE)?;
            Ok(())
        });

        let binding = orm_test.database_test_case.get_database();
        let database_lock = binding.read().unwrap();
        let table = database_lock.get_table(table_name.as_str(), &*DBALLTYPEOBJECT_INSTANCE);

        let max = AllTypeObjectHelper::max_object();
        let min = AllTypeObjectHelper::min_object();
        let random = AllTypeObjectHelper::random_object();
        let empty = AllTypeObjectHelper::empty_object();

        let obj_vec = vec![max.clone(), min.clone(), random.clone(), empty.clone()];
        let _ = table.insert_objects(obj_vec, DbAllTypeObject::all_fields());

        let exp = Expression::new_with_column(Column::new("field_type"))
            .eq_string(max.field_type.as_str());
        assert!(
            max == table
                .get_first_object_by_expression(DbAllTypeObject::all_fields(), exp)
                .unwrap()
        );

        let exp = Expression::new_with_column(Column::new("field_type"))
            .eq_string(min.field_type.as_str());
        assert!(
            min == table
                .get_first_object_by_expression(DbAllTypeObject::all_fields(), exp)
                .unwrap()
        );

        let exp = Expression::new_with_column(Column::new("field_type"))
            .eq_string(empty.field_type.as_str());
        assert!(
            empty
                == table
                    .get_first_object_by_expression(DbAllTypeObject::all_fields(), exp)
                    .unwrap()
        );

        let exp = Expression::new_with_column(Column::new("field_type"))
            .eq_string(random.field_type.as_str());
        assert!(
            random
                == table
                    .get_first_object_by_expression(DbAllTypeObject::all_fields(), exp)
                    .unwrap()
        );

        teardown(&orm_test);
    }

    #[test]
    fn test_table_constraint() {
        let orm_test = OrmTest::new();
        setup(&orm_test);

        let table_name = orm_test.table_name.as_str();

        let mut sql_vec = vec![];
        sql_vec.push("CREATE TABLE IF NOT EXISTS testTable(multiPrimary1 INTEGER, multiPrimary2 INTEGER, multiPrimary3 INTEGER, multiUnique1 INTEGER, multiUnique2 INTEGER, multiUnique3 INTEGER, multiIndex1 INTEGER, multiIndex2 INTEGER, multiIndex3 INTEGER)".to_string());
        sql_vec.push("CREATE INDEX IF NOT EXISTS specifiedNameIndex ON testTable(multiIndex1, multiIndex2, multiIndex3)".to_string());
        sql_vec.push("CREATE INDEX IF NOT EXISTS testTable_multiIndex1_multiIndex2_index ON testTable(multiIndex1, multiIndex2)".to_string());

        orm_test.do_test_create_table_and_index_sqls_as_expected(sql_vec, || {
            orm_test
                .database_test_case
                .create_table(table_name, &*DBTABLECONSTRAINTOBJECT_INSTANCE)
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
            .create_table(fake_table, &*DBAUTOADDCOLUMNOBJECT_INSTANCE)
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
            .create_table(table_name, &*DBPRIMARYNOTAUTOINCREMENTOBJECT_INSTANCE)
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
            .create_table(table_name, &*DBPRIMARYENABLEAUTOINCREMENTOBJECT_INSTANCE)
            .unwrap();
        database_lock.delete_objects(table_name).unwrap();

        let obj2 = PrimaryEnableAutoIncrementObject::new();
        database_lock
            .insert_object(
                obj2,
                DbPrimaryEnableAutoIncrementObject::all_fields(),
                table_name,
            )
            .unwrap();
        let obj_vec = database_lock
            .get_all_objects(DbPrimaryEnableAutoIncrementObject::all_fields(), table_name)
            .unwrap();
        assert_eq!(obj_vec.last().unwrap().id, 2);

        teardown(&orm_test);
    }
}
