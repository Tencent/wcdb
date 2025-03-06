use std::process::{Command, Output};

pub fn run_cmd(cmd: &str) -> Output {
    Command::new("sh")
        .arg("-c")
        .arg(cmd)
        .output()
        .expect("failed to execute process")
}
