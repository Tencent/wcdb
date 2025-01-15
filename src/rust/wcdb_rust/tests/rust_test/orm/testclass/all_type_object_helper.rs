use crate::rust_test::base::random_tool::RandomTool;
use crate::wcdb_orm::orm::testclass::all_type_object::AllTypeObject;
use rand::Rng;

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
