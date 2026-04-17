use std::fs;
use std::path::Path;

pub struct FileTool {}

impl FileTool {
    pub fn new() -> Self {
        FileTool {}
    }

    pub fn new_box() -> Box<Self> {
        Box::new(FileTool {})
    }

    pub fn file_exist(path: &str) -> bool {
        let ret = Path::new(path).try_exists().unwrap();
        ret
    }

    pub fn get_file_size(path: &str) -> i64 {
        let ret = Path::new(path).try_exists().unwrap();
        if ret {
            let result = fs::metadata(path).unwrap().len() as i64;
            result
        } else {
            0
        }
    }
}
