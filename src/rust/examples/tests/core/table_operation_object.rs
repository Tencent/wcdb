use crate::base::random_tool::RandomTool;
use std::time::SystemTime;
use wcdb::base::value::Value;
use wcdb::winq::column::Column;
use wcdb_derive::WCDBTableCoding;

#[derive(WCDBTableCoding, Debug, Clone)]
#[WCDBTable(
    multi_primaries(columns = ["category", "target_id", "channel_id"])
)]
pub struct TableOperationObject {
    #[WCDBField]
    pub category: i32,
    #[WCDBField]
    pub target_id: String,
    #[WCDBField]
    pub channel_id: String,
    #[WCDBField]
    pub value: String,
}

impl TableOperationObject {
    pub fn new() -> Self {
        TableOperationObject {
            category: 0,
            target_id: "".to_string(),
            channel_id: "".to_string(),
            value: "".to_string(),
        }
    }
    pub fn get_obj() -> TableOperationObject {
        let time = SystemTime::now()
            .duration_since(SystemTime::UNIX_EPOCH)
            .expect("Time went backwards")
            .as_millis();
        let mut obj = TableOperationObject::new();
        obj.category = 3;
        obj.target_id = format!("target_id-{}", RandomTool::string());
        obj.channel_id = time.to_string();
        obj.value = format!("value-{}", RandomTool::string());
        obj
    }

    pub fn get_obj_vec(data_num: i32) -> Vec<TableOperationObject> {
        let time = SystemTime::now()
            .duration_since(SystemTime::UNIX_EPOCH)
            .expect("Time went backwards")
            .as_millis();
        let mut obj_vec = vec![];
        for i in 0..data_num {
            let mut obj = TableOperationObject::new();
            obj.category = 3;
            obj.target_id = format!("target_id-{}", RandomTool::string());
            obj.channel_id = (time * 10 + i as u128).to_string();
            obj.value = time.to_string();
            obj_vec.push(obj);
        }
        obj_vec
    }

    pub fn get_values_vec(&self) -> Vec<Value> {
        vec![
            Value::from(self.category as i64),
            Value::from(self.target_id.as_str()),
            Value::from(self.channel_id.as_str()),
            Value::from(self.value.as_str()),
        ]
    }

    pub fn get_all_columns() -> Vec<Column> {
        vec![
            Column::new("category", None),
            Column::new("target_id", None),
            Column::new("channel_id", None),
            Column::new("value", None),
        ]
    }
}
