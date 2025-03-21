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
    pub a_bool2: Option<bool>,
    #[WCDBField]
    pub a_byte: i8,
    #[WCDBField]
    pub a_byte2: Option<i8>,
    #[WCDBField]
    pub a_short: i16,
    #[WCDBField]
    pub a_short2: Option<i16>,
    #[WCDBField]
    pub a_int: i32,
    #[WCDBField]
    pub a_int2: Option<i32>,
    #[WCDBField]
    pub a_long: i64,
    #[WCDBField]
    pub a_long2: Option<i64>,

    // Float
    #[WCDBField]
    pub a_float: f32,
    #[WCDBField]
    pub a_float2: Option<f32>,
    #[WCDBField]
    pub a_double: f64,
    #[WCDBField]
    pub a_double2: Option<f64>,

    // String
    #[WCDBField]
    pub a_string: String,
    #[WCDBField]
    pub a_string2: Option<String>,

    // BLOB
    #[WCDBField]
    a_blob: Vec<u8>,
    #[WCDBField]
    pub a_blob2: Option<Vec<u8>>,
}

impl AllTypeObject {
    pub fn new() -> Self {
        AllTypeObject {
            field_type: "".to_string(),
            a_bool: false,
            a_bool2: None,
            a_byte: 0,
            a_byte2: None,
            a_short: 0,
            a_short2: None,
            a_int: 0,
            a_int2: None,
            a_long: 0,
            a_long2: None,
            a_float: 0.0,
            a_float2: None,
            a_double: 0.0,
            a_double2: None,
            a_string: "".to_string(),
            a_string2: None,
            a_blob: Vec::new(),
            a_blob2: None,
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
            && self.a_bool2 == other.a_bool2
            && self.a_byte2 == other.a_byte2
            && self.a_short2 == other.a_short2
            && self.a_int2 == other.a_int2
            && self.a_long2 == other.a_long2
            && self.a_float2 == other.a_float2
            && self.a_double2 == other.a_double2
            && self.a_string2 == other.a_string2
            && self.a_blob == other.a_blob
            && self.a_blob2 == other.a_blob2
    }
}

pub struct AllTypeObjectHelper {}

impl AllTypeObjectHelper {
    pub fn max_object() -> AllTypeObject {
        AllTypeObject {
            field_type: "max".to_string(),
            a_bool: true,
            a_bool2: Some(true),
            a_byte: i8::MAX,
            a_byte2: Some(i8::MAX),
            a_short: i16::MAX,
            a_short2: Some(i16::MAX),
            a_int: i32::MAX,
            a_int2: Some(i32::MAX),
            a_long: i64::MAX,
            a_long2: Some(i64::MAX),
            a_float: f32::MAX,
            a_float2: Some(f32::MAX),
            a_double: f64::MAX,
            a_double2: Some(f64::MAX),
            a_string: RandomTool::string(),
            a_string2: Some(RandomTool::string()),
            a_blob: RandomTool::bytes(),
            a_blob2: Some(RandomTool::bytes()),
        }
    }

    pub fn min_object() -> AllTypeObject {
        AllTypeObject {
            field_type: "min".to_string(),
            a_bool: false,
            a_bool2: Some(false),
            a_byte: i8::MIN,
            a_byte2: Some(i8::MIN),
            a_short: i16::MIN,
            a_short2: Some(i16::MIN),
            a_int: i32::MIN,
            a_int2: Some(i32::MIN),
            a_long: i64::MIN,
            a_long2: Some(i64::MIN),
            a_float: f32::MIN,
            a_float2: Some(f32::MIN),
            a_double: f64::MIN,
            a_double2: Some(f64::MIN),
            a_string: RandomTool::string(),
            a_string2: Some(RandomTool::string()),
            a_blob: RandomTool::bytes(),
            a_blob2: Some(RandomTool::bytes()),
        }
    }

    pub fn random_object() -> AllTypeObject {
        let mut rng = rand::thread_rng();
        AllTypeObject {
            field_type: "random".to_string(),
            a_bool: rng.gen::<bool>(),
            a_bool2: Some(rng.gen::<bool>()),
            a_byte: rng.gen::<i8>(),
            a_byte2: Some(rng.gen::<i8>()),
            a_short: rng.gen::<i16>(),
            a_short2: Some(rng.gen::<i16>()),
            a_int: rng.gen::<i32>(),
            a_int2: Some(rng.gen::<i32>()),
            a_long: rng.gen::<i64>(),
            a_long2: Some(rng.gen::<i64>()),
            a_float: rng.gen::<f32>(),
            a_float2: Some(rng.gen::<f32>()),
            a_double: rng.gen::<f64>(),
            a_double2: Some(rng.gen::<f64>()),
            a_string: RandomTool::string(),
            a_string2: Some(RandomTool::string()),
            a_blob: RandomTool::bytes(),
            a_blob2: Some(RandomTool::bytes()),
        }
    }

    pub fn empty_object() -> AllTypeObject {
        AllTypeObject {
            field_type: "empty".to_string(),
            a_bool: false,
            a_bool2: None,
            a_byte: 0,
            a_byte2: None,
            a_short: 0,
            a_short2: None,
            a_int: 0,
            a_int2: None,
            a_long: 0,
            a_long2: None,
            a_float: 0.0,
            a_float2: None,
            a_double: 0.0,
            a_double2: None,
            a_string: RandomTool::string(),
            a_string2: None,
            a_blob: Vec::new(),
            a_blob2: None,
        }
    }
}
