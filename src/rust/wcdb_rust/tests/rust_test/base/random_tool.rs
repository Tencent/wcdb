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
}
