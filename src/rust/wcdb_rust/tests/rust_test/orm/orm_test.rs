use crate::rust_test::base::base_test_case::TestCaseTrait;
use crate::rust_test::base::database_test_case::{DatabaseTestCase, Expect};
use crate::rust_test::orm::testclass::all_type_object_helper::AllTypeObjectHelper;
use crate::wcdb_orm::orm::testclass::all_type_object::{DbAllTypeObject, DBALLTYPEOBJECT_INSTANCE};
use std::cmp::PartialEq;
use std::sync::MutexGuard;
use wcdb_core::base::wcdb_exception::WCDBResult;
use wcdb_core::core::database::Database;
use wcdb_core::core::handle_orm_operation::HandleORMOperationTrait;
use wcdb_core::orm::table_binding::TableBinding;
use wcdb_core::winq::column::Column;
use wcdb_core::winq::expression::Expression;
use wcdb_core::winq::identifier::IdentifierTrait;

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
        let table_name = self.table_name.clone();
        let _ = self.database_test_case.do_test_sql_vec(new_sql_vec, || {
            self.database_test_case
                .create_table(table_name.as_str(), &*DBALLTYPEOBJECT_INSTANCE)?;
            Ok(())
        });

        // let table = self
        //     .database_test_case
        //     .get_database_lock()
        //     .get_table(table_name.as_str(), &*DBALLTYPEOBJECT_INSTANCE);

        let max = AllTypeObjectHelper::max_object();
        let min = AllTypeObjectHelper::min_object();
        let random = AllTypeObjectHelper::random_object();
        let empty = AllTypeObjectHelper::empty_object();

        let obj_vec = vec![max.clone(), min.clone(), random.clone(), empty.clone()];
        let database_lock: MutexGuard<Database> = self.database_test_case.get_database_lock();

        let _ = database_lock.insert_objects(obj_vec, DbAllTypeObject::all_fields(), "testTable");

        let exp =
            Expression::new_with_column(Column::new("field_type")).eq_text(max.field_type.as_str());
        assert!(
            max == database_lock
                .get_first_object_by_expression(DbAllTypeObject::all_fields(), "testTable", exp)
                .unwrap()
        );

        let exp =
            Expression::new_with_column(Column::new("field_type")).eq_text(min.field_type.as_str());
        assert!(
            min == database_lock
                .get_first_object_by_expression(DbAllTypeObject::all_fields(), "testTable", exp)
                .unwrap()
        );

        let exp = Expression::new_with_column(Column::new("field_type"))
            .eq_text(empty.field_type.as_str());
        assert!(
            empty
                == database_lock
                    .get_first_object_by_expression(DbAllTypeObject::all_fields(), "testTable", exp)
                    .unwrap()
        );

        let exp = Expression::new_with_column(Column::new("field_type"))
            .eq_text(random.field_type.as_str());
        assert!(
            random
                == database_lock
                    .get_first_object_by_expression(DbAllTypeObject::all_fields(), "testTable", exp)
                    .unwrap()
        );

        //  todo qixinbing  table orm 待实现
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

    fn set_up(orm_test: &OrmTest) {
        orm_test.setup().unwrap();
    }

    fn teardown(orm_test: &OrmTest) {
        orm_test.teardown().unwrap();
    }

    #[test]
    fn test_all_type() {
        let orm_test = OrmTest::new();
        set_up(&orm_test);

        let mut sql_vec = vec![];
        sql_vec.push("CREATE TABLE IF NOT EXISTS testTable(type TEXT, aBoolean INTEGER, aBoolean2 INTEGER, aByte INTEGER, aByte2 INTEGER, aShort INTEGER, aShort2 INTEGER, anInt INTEGER, integer INTEGER, aLong INTEGER, aLong2 INTEGER, aFloat REAL, aFloat2 REAL, aDouble REAL, aDouble2 REAL, string TEXT, bytes BLOB)".to_string());

        orm_test.do_test_create_table_and_index_sqls_as_expected(sql_vec, || Ok(()));

        teardown(&orm_test);
    }
}
