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
        println!("cargo:rustc-link-lib=static=WCDB");
    } else if target.contains("ohos") || target.contains("android") || target.contains("linux") {
        let manifest_dir = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap());
        let openssl_search_path = openssl_search_path_from_target(&target).unwrap();
        let openssl_path = manifest_dir.join(openssl_search_path);
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
        let cc = env::var("CC").expect(&format!("{} is not set CC", &target));
        let cxx = env::var("CXX").expect(&format!("{} is not set CXX", &target));

        // 指定 asm 编译器：zstd huf_decompress_amd64.S 是汇编
        // 指定 C 编译器：openssl 主要是 C
        // 指定 C++ 编译器：WCDB 主要是 C++
        cmake
            .define("CMAKE_ASM_COMPILER", &cc)
            .define("CMAKE_ASM_FLAGS", "-x assembler-with-cpp")
            .define("CMAKE_C_COMPILER", &cc)
            .define("CMAKE_CXX_COMPILER", &cxx);
    }
    if target.contains("android") {
        config_cmake_for_android(target, &mut cmake);
    }

    let dst = cmake
        .define("CMAKE_CXX_FLAGS", "-D_Nullable= -D_Nonnull=")
        .define("CMAKE_C_FLAGS", "-D_Nullable= -D_Nonnull=")
        .define("CMAKE_BUILD_TYPE", "Release")
        .define("BUILD_FROM_CARGO", "ON")
        .build_arg(format!("-j{}", num_cpus::get()))
        .build_target("all")
        .build();
    dst
}

fn config_cmake_for_android(target: &str, cmake: &mut cmake::Config) {
    let toolchain_file = env::var("CMAKE_TOOLCHAIN_FILE")
        .expect(&format!("{} is not set CMAKE_TOOLCHAIN_FILE", &target));
    cmake.define("CMAKE_TOOLCHAIN_FILE", toolchain_file);

    let abi = android_abi_from_target(target).expect(&format!("{} can not find abi", &target));
    cmake.define("ANDROID_ABI", abi);

    // ✅ 设置平台版本（例如 android-21）
    cmake.define("ANDROID_PLATFORM", "android-21");
}

fn android_abi_from_target(target: &str) -> Option<&'static str> {
    match target {
        "aarch64-linux-android" => Some("arm64-v8a"),
        "armv7-linux-androideabi" => Some("armeabi-v7a"),
        "i686-linux-android" => Some("x86"),
        "x86_64-linux-android" => Some("x86_64"),
        _ => None,
    }
}

fn openssl_search_path_from_target(target: &str) -> Option<&'static str> {
    match target {
        // android
        "aarch64-linux-android" => Some("../../../tools/prebuild/openssl/android/arm64-v8a"),
        "armv7-linux-androideabi" => Some("../../../tools/prebuild/openssl/android/armeabi-v7a"),
        "i686-linux-android" => Some("../../../tools/prebuild/openssl/android/x86"),
        "x86_64-linux-android" => Some("../../../tools/prebuild/openssl/android/x86_64"),
        // ohos
        "aarch64-unknown-linux-ohos" => Some("../../../tools/prebuild/openssl/ohos/arm64-v8a"),
        "x86_64-unknown-linux-ohos" => Some("../../../tools/prebuild/openssl/ohos/x86_64"),
        // linux
        "aarch64-unknown-linux-gnu" => Some("../../../tools/prebuild/openssl/linux/arm64"),
        "x86_64-unknown-linux-gnu" => Some("../../../tools/prebuild/openssl/linux/x86_64"),
        _ => None,
    }
}
