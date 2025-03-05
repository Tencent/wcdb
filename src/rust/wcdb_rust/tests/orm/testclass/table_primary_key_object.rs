use crate::base::random_tool::RandomTool;
use table_coding::WCDBTableCoding;

#[derive(WCDBTableCoding)]
#[WCDBTable(
    multi_primaries(columns = ["id"]),
)]
pub struct TablePrimaryKeyObjectOld {
    #[WCDBField]
    pub id: i64,
    #[WCDBField]
    pub name: String,
    #[WCDBField]
    pub price: f64,
    #[WCDBField]
    pub desc: String,
}

impl TablePrimaryKeyObjectOld {
    pub fn new() -> Self {
        TablePrimaryKeyObjectOld {
            id: 0,
            name: "".to_string(),
            price: 0.0,
            desc: "".to_string(),
        }
    }
}

impl TablePrimaryKeyObjectOld {
    pub fn get_old_obj_vec(data_num: i32) -> Vec<TablePrimaryKeyObjectOld> {
        let mut obj_vec = vec![];
        for i in 0..data_num {
            let mut obj = TablePrimaryKeyObjectOld::new();
            obj.id = i as i64;
            obj.name = format!("name-{}", RandomTool::string());
            obj.price = i as f64;
            obj.desc = format!("desc-{}", RandomTool::string());
            obj_vec.push(obj);
        }
        obj_vec
    }
}

#[derive(WCDBTableCoding)]
#[WCDBTable(
    multi_primaries(columns = ["id","name"]),
)]
pub struct TablePrimaryKeyObjectNew {
    #[WCDBField]
    pub id: i64,
    #[WCDBField]
    pub name: String,
    #[WCDBField]
    pub price: f64,
    #[WCDBField]
    pub desc: String,
}

impl TablePrimaryKeyObjectNew {
    pub fn new() -> Self {
        TablePrimaryKeyObjectNew {
            id: 0,
            name: "".to_string(),
            price: 0.0,
            desc: "".to_string(),
        }
    }
}
