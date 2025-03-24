use crate::base::random_tool::RandomTool;
use wcdb_derive::WCDBTableCoding;

const CATEGORY_DEFAULT_VALUE: i32 = 111;

#[derive(WCDBTableCoding)]
pub struct ColumnRenameObjectOld {
    #[WCDBField]
    category: i32,
    #[WCDBField]
    target_id: String,
    #[WCDBField]
    channel_id: String,
}

#[derive(WCDBTableCoding)]
pub struct ColumnRenameObjectNew {
    #[WCDBField(column_name = "conversation_type")]
    category: i32,
    #[WCDBField]
    target_id: String,
    #[WCDBField]
    channel_id: String,
}

impl ColumnRenameObjectOld {
    pub fn new() -> Self {
        ColumnRenameObjectOld {
            category: 0,
            target_id: "".to_string(),
            channel_id: "".to_string(),
        }
    }

    pub fn get_old_obj_vec(data_num: i32) -> Vec<ColumnRenameObjectOld> {
        let mut obj_vec = vec![];
        for i in 0..data_num {
            let mut obj = ColumnRenameObjectOld::new();
            obj.category = CATEGORY_DEFAULT_VALUE;
            obj.target_id = format!("target_id-{}", RandomTool::string());
            obj.channel_id = format!("channel_id-{}", RandomTool::string());
            obj_vec.push(obj);
        }
        obj_vec
    }
}

impl ColumnRenameObjectNew {
    pub fn is_same(&self) -> bool {
        // 只需要关注列名变更后，值是否相同即可
        self.category == CATEGORY_DEFAULT_VALUE
    }
}
