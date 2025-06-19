use std::env;
use std::path::PathBuf;
use std::process::Command;

fn main() {
    let target = env::var("TARGET").unwrap();

    let dst = config_cmake(&target);

    Command::new("git")
        .arg("submodule update --init openssl sqlcipher zstd")
        .output()
        .expect("failed to execute cmd: git submodule update --init openssl sqlcipher zstd");

    println!("cargo:rerun-if-changed=cpp");
    println!("cargo:rustc-link-lib=static=sqlcipher");
    println!("cargo:rustc-link-lib=static=zstd");
    println!("cargo:rustc-link-lib=z");
    if target.contains("apple") {
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
    } else if target.contains("ohos") || target.contains("android") || target.contains("linux") {
        let manifest_dir = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap());
        let (platform, abi) = platform_abi_from_target(&target).unwrap();
        // ../../../tools/prebuild/openssl/ohos/arm64-v8a"
        let openssl_path = manifest_dir
            .join("../../../tools/prebuild/openssl")
            .join(platform)
            .join(abi);
        println!(
            "cargo:warning=WCDB MANIFEST_DIR: {}",
            manifest_dir.display()
        );
        println!("cargo:warning=WCDB crypto path: {}", openssl_path.display());
        println!("cargo:warning=WCDB crypto exist: {}", openssl_path.exists());
        println!("cargo:rustc-link-lib=stdc++");
        println!("cargo:rustc-link-search=native={}", openssl_path.display());
        println!("cargo:rustc-link-lib=static=crypto");
        println!(
            "cargo:rustc-link-search=native={}/build/wcdb/",
            dst.display()
        );
        println!("cargo:rustc-link-lib=static=WCDB");
    }
}

fn config_cmake(target: &str) -> PathBuf {
    let mut cmake = cmake::Config::new("../cpp");
    if !target.contains("apple") {
        // apple 平台会直接使用 mac 系统内置的 asm/C/C++ 编译器
        // 非 apple 平台需要指明 asm/C/C++ 编译器
        let cc = env::var("clang").expect(&format!("{} is not set clang", &target));
        let cxx = env::var("clang++").unwrap_or_else(|_| cc.replace("clang", "clang++"));

        // 指定 asm 编译器：zstd huf_decompress_amd64.S 是汇编
        // 指定 C 编译器：openssl 主要是 C
        // 指定 C++ 编译器：WCDB 主要是 C++
        cmake
            .define("CMAKE_ASM_COMPILER", &cc)
            .define("CMAKE_ASM_FLAGS", "-x assembler-with-cpp")
            .define("CMAKE_C_COMPILER", &cc);
        // .define("CMAKE_CXX_COMPILER", &cxx);
    }

    let dst = cmake
        .define("CMAKE_CXX_FLAGS", "-D_Nullable= -D_Nonnull=")
        .define("CMAKE_C_FLAGS", "-D_Nullable= -D_Nonnull=")
        .define("CMAKE_BUILD_TYPE", "Release")
        .build_arg(format!("-j{}", num_cpus::get()))
        .build_target("all")
        .build();
    dst
}

// 通过 target 获取 platform abi，用于找到 tools/prebuild/openssl/{platform}/{abi}/libcrypto.a
fn platform_abi_from_target(target: &str) -> Option<(&'static str, &'static str)> {
    match target {
        // android
        "aarch64-unknown-linux-android" => Some(("android", "arm64-v8a")),
        "armv7-linux-androideabi" => Some(("android", "armeabi-v7a")),
        "i686-linux-android" => Some(("android", "x86")),
        "x86_64-linux-android" => Some(("android", "x86_64")),
        // ohos
        "aarch64-unknown-linux-ohos" => Some(("ohos", "arm64-v8a")),
        "x86_64-unknown-linux-ohos" => Some(("ohos", "x86_64")),
        // linux
        "aarch64-unknown-linux-gnu" => Some(("linux", "arm64")),
        "x86_64-unknown-linux-gnu" => Some(("linux", "x86_64")),
        _ => None,
    }
}
