use std::process::{Command, Output};

// let output = run_cmd(cmd);
// println!("run cmd {}",String::from_utf8_lossy(&output.stdout));
pub fn run_cmd(cmd: &str) -> Output {
    Command::new("sh")
        .arg("-c")
        .arg(cmd)
        .output()
        .expect("failed to execute process")
}
