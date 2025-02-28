use crate::base::random_tool::RandomTool;
use rand::Rng;
use table_coding::WCDBTableCoding;

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
