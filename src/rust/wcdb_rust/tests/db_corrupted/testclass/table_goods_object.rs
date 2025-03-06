use crate::base::random_tool::RandomTool;
use table_coding::WCDBTableCoding;

#[derive(WCDBTableCoding)]
pub struct TableGoodsObject {
    #[WCDBField(is_primary = true, is_auto_increment = true)]
    pub id: i64,
    #[WCDBField]
    pub name: String,
    #[WCDBField]
    pub price: f64,
    #[WCDBField]
    pub desc: String,
    #[WCDBField]
    pub brand: String,
}

impl TableGoodsObject {
    fn new() -> Self {
        let len = 1000;
        TableGoodsObject {
            id: 0,
            name: format!("name-{}", RandomTool::string_by_length(len)),
            price: 0.0,
            desc: format!("desc-{}", RandomTool::string_by_length(len)),
            brand: format!("brand-{}", RandomTool::string_by_length(len)),
        }
    }

    pub fn get_obj_vec(data_num: i32) -> Vec<TableGoodsObject> {
        let mut obj_vec = vec![];
        for i in 0..data_num {
            let mut obj = TableGoodsObject::new();
            obj.id = 0i64;
            obj.price = (i * 10) as f64;
            obj_vec.push(obj);
        }
        obj_vec
    }
}
