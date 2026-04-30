use crate::base::test_object::TestObject;
use rand::seq::SliceRandom;
use rand::Rng;

pub struct RandomTool {}

impl RandomTool {
    pub fn string() -> String {
        Self::string_by_length(100)
    }

    pub fn string_by_length(length: i32) -> String {
        let chars: Vec<char> = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
            .chars()
            .collect();
        let mut rng = rand::thread_rng();
        (0..length)
            .map(|_| *chars.choose(&mut rng).unwrap())
            .collect()
    }

    pub fn bytes() -> Vec<u8> {
        Self::bytes_by_length(100)
    }

    pub fn bytes_by_length(length: i32) -> Vec<u8> {
        let mut rng = rand::thread_rng(); // 创建随机数生成器
        let mut bytes = vec![0u8; length as usize]; // 初始化字节数组
        rng.fill(bytes.as_mut_slice()); // 填充随机数据
        bytes
    }

    pub fn auto_increment_test_case_objects(count: i32) -> Vec<TestObject> {
        let mut vec = Vec::new();
        for x in 0..count {
            let obj = TestObject::create_auto_increment_object(Self::string());
            vec.push(obj);
        }
        vec
    }
}
