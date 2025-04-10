use std::process::Command;

fn main() {
    let dst = cmake::Config::new("../cpp")
        .define("CMAKE_CXX_FLAGS", "-D_Nullable= -D_Nonnull=")
        .define("CMAKE_C_FLAGS", "-D_Nullable= -D_Nonnull=")
        .define("CMAKE_BUILD_TYPE", "Release")
        .build_arg(format!("-j{}", num_cpus::get()))
        .build_target("all")
        .build();

    Command::new("git")
        .arg("submodule update --init sqlcipher zstd")
        .output()
        .expect("failed to execute cmd: git submodule update --init sqlcipher zstd");

    println!("cargo:rerun-if-changed=cpp");
    println!("cargo:rustc-link-lib=z");
    println!("cargo:rustc-link-lib=static=sqlcipher");
    println!("cargo:rustc-link-lib=static=zstd");
    if cfg!(target_os = "macos") {
        println!("cargo:rustc-link-lib=c++");
        println!("cargo:rustc-link-lib=framework=CoreFoundation");
        println!("cargo:rustc-link-lib=framework=Security");
        println!(
            "cargo:rustc-link-search=native={}/build/wcdb/",
            dst.display()
        );
        println!(
            "cargo:rustc-link-search=framework={}/build/wcdb/",
            dst.display()
        );
        println!("cargo:rustc-link-lib=framework=WCDB");
    } else if cfg!(target_os = "linux") {
        println!("cargo:rustc-link-lib=stdc++");
        println!("cargo:rustc-link-search=native=../../tools/prebuild/openssl/linux/x86_64");
        println!("cargo:rustc-link-lib=static=crypto");
        println!(
            "cargo:rustc-link-search=native={}/build/wcdb/",
            dst.display()
        );
        println!("cargo:rustc-link-lib=static=WCDB");
    }
}
