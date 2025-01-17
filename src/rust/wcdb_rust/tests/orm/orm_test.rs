use crate::base::base_test_case::TestCaseTrait;
use crate::base::database_test_case::{DatabaseTestCase, Expect};
use crate::base::random_tool::RandomTool;
use rand::Rng;
use std::cmp::PartialEq;
use std::sync::MutexGuard;
use table_coding::WCDBTableCoding;
use wcdb_core::base::wcdb_exception::WCDBResult;
use wcdb_core::core::database::Database;
use wcdb_core::core::handle_orm_operation::HandleORMOperationTrait;
use wcdb_core::core::table_orm_operation::TableORMOperationTrait;
use wcdb_core::orm::table_binding::TableBinding;
use wcdb_core::winq::column::Column;
use wcdb_core::winq::expression::Expression;
use wcdb_core::winq::identifier::IdentifierTrait;

#[derive(WCDBTableCoding, PartialEq, Clone)]
pub struct AllTypeObject {
    #[WCDBField]
    pub field_type: String,

    // Integer
    #[WCDBField]
    pub a_bool: bool,
    #[WCDBField]
    pub a_byte: i8,
    #[WCDBField]
    pub a_short: i16,
    #[WCDBField]
    pub a_int: i32,
    #[WCDBField]
    pub a_long: i64,

    // Float
    #[WCDBField]
    pub a_float: f32,
    #[WCDBField]
    pub a_double: f64,

    // String
    #[WCDBField]
    pub a_string: String,
    // BLOB
    // #[WCDBField] todo qixinbing 待实现
    // a_blob : Vec<u8>,
}

impl AllTypeObject {
    pub fn new() -> Self {
        AllTypeObject {
            field_type: "".to_string(),
            a_bool: false,
            a_byte: 0,
            a_short: 0,
            a_int: 0,
            a_long: 0,
            a_float: 0.0,
            a_double: 0.0,
            a_string: "".to_string(),
            // a_blob : Vec::new(),
        }
    }

    pub fn equals(&self, other: &AllTypeObject) -> bool {
        self.a_bool == other.a_bool
            && self.a_byte == other.a_byte
            && self.a_short == other.a_short
            && self.a_int == other.a_int
            && self.a_long == other.a_long
            && self.a_float == other.a_float
            && self.a_double == other.a_double
            && self.a_string == other.a_string
    }
}

pub struct AllTypeObjectHelper {}

impl AllTypeObjectHelper {
    pub fn max_object() -> AllTypeObject {
        AllTypeObject {
            field_type: "max".to_string(),
            a_bool: true,
            a_byte: i8::MAX,
            a_short: i16::MAX,
            a_int: i32::MAX,
            a_long: i64::MAX,
            a_float: f32::MAX,
            a_double: f64::MAX,
            a_string: RandomTool::string(),
        }
    }

    pub fn min_object() -> AllTypeObject {
        AllTypeObject {
            field_type: "min".to_string(),
            a_bool: false,
            a_byte: i8::MIN,
            a_short: i16::MIN,
            a_int: i32::MIN,
            a_long: i64::MIN,
            a_float: f32::MIN,
            a_double: f64::MIN,
            a_string: RandomTool::string(),
        }
    }

    pub fn random_object() -> AllTypeObject {
        let mut rng = rand::thread_rng();
        AllTypeObject {
            field_type: "random".to_string(),
            a_bool: rng.gen::<bool>(),
            a_byte: rng.gen::<i8>(),
            a_short: rng.gen::<i16>(),
            a_int: rng.gen::<i32>(),
            a_long: rng.gen::<i64>(),
            a_float: rng.gen::<f32>(),
            a_double: rng.gen::<f64>(),
            a_string: RandomTool::string(),
        }
    }

    pub fn empty_object() -> AllTypeObject {
        AllTypeObject {
            field_type: "empty".to_string(),
            a_bool: false,
            a_byte: 0,
            a_short: 0,
            a_int: 0,
            a_long: 0,
            a_float: 0.0,
            a_double: 0.0,
            a_string: RandomTool::string(),
        }
    }
}

#[derive(WCDBTableCoding)]
pub struct FieldObject {
    #[WCDBField]
    field: i32,
    #[WCDBField(column_name = "differentName")]
    field_with_different_name: i32,
}

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

        let database_lock: MutexGuard<Database> = self.database_test_case.get_database_lock();
        let table = database_lock.get_table(table_name.as_str(), &*DBALLTYPEOBJECT_INSTANCE);

        let max = AllTypeObjectHelper::max_object();
        let min = AllTypeObjectHelper::min_object();
        let random = AllTypeObjectHelper::random_object();
        let empty = AllTypeObjectHelper::empty_object();

        let obj_vec = vec![max.clone(), min.clone(), random.clone(), empty.clone()];
        let _ = table.insert_objects(obj_vec, DbAllTypeObject::all_fields());

        let exp =
            Expression::new_with_column(Column::new("field_type")).eq_text(max.field_type.as_str());
        assert!(
            max == table
                .get_first_object_by_expression(DbAllTypeObject::all_fields(), exp)
                .unwrap()
        );

        let exp =
            Expression::new_with_column(Column::new("field_type")).eq_text(min.field_type.as_str());
        assert!(
            min == table
                .get_first_object_by_expression(DbAllTypeObject::all_fields(), exp)
                .unwrap()
        );

        let exp = Expression::new_with_column(Column::new("field_type"))
            .eq_text(empty.field_type.as_str());
        assert!(
            empty
                == table
                    .get_first_object_by_expression(DbAllTypeObject::all_fields(), exp)
                    .unwrap()
        );

        let exp = Expression::new_with_column(Column::new("field_type"))
            .eq_text(random.field_type.as_str());
        assert!(
            random
                == table
                    .get_first_object_by_expression(DbAllTypeObject::all_fields(), exp)
                    .unwrap()
        );
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
    fn test_all_field() {
        assert_eq!(DbFieldObject::all_fields().len(), 2);

        let binding = DbFieldObject::all_fields();
        let first_field = binding.first().unwrap();
        assert_eq!(first_field.get_description(), "field");

        let second_field = binding.last().unwrap();
        assert_eq!(second_field.get_description(), "differentName");
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
