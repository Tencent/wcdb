use std::env;
use std::path::PathBuf;
use std::process::{Command, Output};

fn main() {
    run_cmd("git submodule update --init openssl sqlcipher zstd");

    let target = env::var("TARGET").unwrap();
    let dst = config_cmake(&target);

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
        let openssl_search_path = openssl_search_path_from_target(&target)
            .expect(&format!("wcdb not support target: {}", target));
        let openssl_path = manifest_dir.join(openssl_search_path);
        println!("cargo:rustc-link-lib=stdc++");
        println!("cargo:rustc-link-search=native={}", openssl_path.display());
        println!("cargo:rustc-link-lib=static=crypto");
        println!(
            "cargo:rustc-link-search=native={}/build/wcdb/",
            dst.display()
        );
        println!("cargo:rustc-link-lib=static=WCDB");
    } else if target.contains("windows") {
        // 根据工具链选择不同的 C++ 标准库
        if target.contains("msvc") {
            // MSVC 工具链
            println!("cargo:rustc-link-lib=dylib=msvcrt");
            println!("cargo:rustc-link-lib=dylib=user32");
            println!("cargo:rustc-link-lib=dylib=advapi32");
            println!("cargo:rustc-link-lib=dylib=crypt32");

            // 静态链接 C++ 标准库 (可选)
            // println!("cargo:rustc-link-lib=static=libcmt");
        } else {
            // GNU (MinGW) 工具链
            println!("cargo:rustc-link-lib=dylib=stdc++");
            println!("cargo:rustc-link-lib=dylib=gcc");
            println!("cargo:rustc-link-lib=dylib=winpthread");
        }

        let manifest_dir = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap());
        let openssl_search_path = openssl_search_path_from_target(&target)
            .expect(&format!("wcdb not support target: {}", target));
        let openssl_path = manifest_dir.join(openssl_search_path);
        println!("cargo:rustc-link-search=native={}", openssl_path.display());
        println!("cargo:rustc-link-lib=static=libcrypto");
        // Windows 平台配置
        println!(
            "cargo:rustc-link-search=native={}/build/wcdb/",
            dst.display()
        );
        println!(
            "cargo:rustc-link-search=native={}/build/wcdb/Debug",
            dst.display()
        );
        println!(
            "cargo:rustc-link-search=native={}/build/wcdb/Release",
            dst.display()
        );
        println!("cargo:rustc-link-lib=static=WCDB");
    }
}

fn config_cmake(target: &str) -> PathBuf {
    let mut cmake = cmake::Config::new("../cpp");
    cmake.very_verbose(true);

    let mut cc = String::new();
    let mut cxx = String::new();
    if target.contains("apple") {
        cc = env::var("CC").unwrap_or("/usr/bin/clang".to_string());
        cxx = env::var("CXX").unwrap_or("/usr/bin/clang++".to_string());
    } else if target.contains("windows") {
        cc = env::var("CC").unwrap_or_else(|_| "cl.exe".into());
        cxx = env::var("CXX").unwrap_or_else(|_| "cl.exe".into());
    } else {
        cc = env::var("CC").expect(&format!("wcdb: {} is not set CC", &target));
        cxx = env::var("CXX").expect(&format!("wcdb: {} is not set CXX", &target));
    }
    cmake
        .define("CMAKE_ASM_COMPILER", &cc) // 指定 asm 编译器：zstd huf_decompress_amd64.S 是汇编
        .define("CMAKE_ASM_FLAGS", "-x assembler-with-cpp")
        .define("CMAKE_C_COMPILER", &cc) // 指定 C 编译器：openssl 主要是 C
        .define("CMAKE_CXX_COMPILER", &cxx); // 指定 C++ 编译器：WCDB 主要是 C++

    if target.contains("apple") {
        let (system_name, sdk_name, arch_name) = ios_sdk_arch_from_target(target)
            .expect(&format!("wcdb not support target: {}", target));

        let output = run_cmd(&format!("xcrun --sdk {} --show-sdk-path", sdk_name));
        let sysroot = String::from_utf8(output.stdout).unwrap().replace("\n", "");

        cmake
            .define("CMAKE_SYSTEM_NAME", system_name)
            .define("CMAKE_OSX_SYSROOT", &sysroot)
            .define("CMAKE_SYSROOT", &sysroot)
            .define("CMAKE_OSX_ARCHITECTURES", arch_name);
    } else if target.contains("android") {
        let toolchain_file = env::var("CMAKE_TOOLCHAIN_FILE").expect(&format!(
            "wcdb: {} is not set CMAKE_TOOLCHAIN_FILE",
            &target
        ));
        cmake.define("CMAKE_TOOLCHAIN_FILE", toolchain_file);

        let abi =
            android_abi_from_target(target).expect(&format!("wcdb: {} can not find abi", &target));
        cmake.define("ANDROID_ABI", abi);

        // ✅ 设置平台版本（例如 android-21）
        cmake.define("ANDROID_PLATFORM", "android-21");
    } else if target.contains("windows") {
        // cmake.generator("Visual Studio 17 2022");
        let abi =
            windows_abi_from_target(target).expect(&format!("wcdb: {} can not find abi", &target));
        cmake.define("CMAKE_GENERATOR_PLATFORM", abi);
        cmake
            .define("WIN32", "ON")
            .define("CMAKE_MSVC_RUNTIME_LIBRARY", "MultiThreadedDLL") // MT/MTd/MD/MDd
            .profile("Release"); // ← 明确设置为 Release
    }

    cmake
        .define("CMAKE_CXX_FLAGS", "-D_Nullable= -D_Nonnull=")
        .define("CMAKE_C_FLAGS", "-D_Nullable= -D_Nonnull=")
        .define("CMAKE_BUILD_TYPE", "Release")
        .define("BUILD_FROM_CARGO", "ON");

    if target.contains("windows") {
        cmake.build_target("ALL_BUILD");
    } else {
        cmake.build_arg(format!("-j{}", num_cpus::get()));
        cmake.build_target("all");
    }

    cmake.build()
}

fn ios_sdk_arch_from_target(target: &str) -> Option<(&'static str, &'static str, &'static str)> {
    match target {
        // ios
        "aarch64-apple-ios" => Some(("iOS", "iphoneos", "arm64")),
        "aarch64-apple-ios-sim" => Some(("iOS", "iphonesimulator", "arm64")),
        "x86_64-apple-ios" => Some(("iOS", "iphonesimulator", "x86_64")),
        // mac
        "aarch64-apple-darwin" => Some(("Darwin", "macosx", "arm64")),
        "x86_64-apple-darwin" => Some(("Darwin", "macosx", "x86_64")),
        _ => None,
    }
}

fn windows_abi_from_target(target: &str) -> Option<&'static str> {
    match target {
        // msvc
        "x86_64-pc-windows-msvc" => Some("x64"),
        "i686-pc-windows-msvc" => Some("Win32"),
        // gnu
        // "x86_64-pc-windows-gnu" => Some("x64"),
        // "i686-pc-windows-gnu" => Some("Win32"),
        _ => None,
    }
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
        // windows
        "x86_64-pc-windows-msvc" => Some("..\\..\\..\\tools\\prebuild\\openssl\\windows\\win64"),
        "i686-pc-windows-msvc" => Some("..\\..\\..\\tools\\prebuild\\openssl\\windows\\win32"),
        _ => None,
    }
}

fn run_cmd(cmd: &str) -> Output {
    Command::new("sh")
        .arg("-c")
        .arg(cmd)
        .output()
        .expect(&format!("wcdb : failed to execute command :{}", cmd))
}
