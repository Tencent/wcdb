use table_coding::WCDBTableCoding;

#[derive(WCDBTableCoding)]
pub struct AllTypeObject {
    // #[WCDBField]
    // pub field_type : String,

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
    // #[WCDBField] todo qixinbing 待实现
    // a_string : String,

    // BLOB
    // #[WCDBField] todo qixinbing 待实现
    // a_blob : Vec<u8>,
}

impl AllTypeObject {
    pub fn new() -> Self {
        AllTypeObject {
            // field_type : "".to_string(),
            a_bool: false,
            a_byte: 0,
            a_short: 0,
            a_int: 0,
            a_long: 0,
            a_float: 0.0,
            a_double: 0.0,
            // a_string : String::new(),
            // a_blob : Vec::new(),
        }
    }

    pub fn equals(&self, other: &AllTypeObject) -> bool {
        return self.a_bool == other.a_bool
            && self.a_byte == other.a_byte
            && self.a_short == other.a_short
            && self.a_int == other.a_int
            && self.a_long == other.a_long
            && self.a_float == other.a_float
            && self.a_double == other.a_double;
    }
}
