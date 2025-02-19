use crate::base::test_object::TestObject;
use rand::seq::SliceRandom;

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

    pub fn auto_increment_test_case_objects(count: i32) -> Vec<TestObject> {
        let mut vec = Vec::new();
        for x in 0..count {
            let obj = TestObject::create_auto_increment_object(Self::string());
            vec.push(obj);
        }
        vec
    }
}
