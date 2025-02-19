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
        println!("bugtags.file_exist: {}", ret);
        ret
    }
}
