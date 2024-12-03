fn main() {
    let dst = cmake::Config::new("../cpp")
        .define("CMAKE_BUILD_TYPE", "Release")
        .build_arg(format!("-j{}", num_cpus::get()))
        .build_target("all")
        .build();

    println!("cargo:rustc-link-lib=c++");
    println!("cargo:rustc-link-lib=z");
    println!("cargo:rustc-link-lib=framework=CoreFoundation");
    println!("cargo:rustc-link-lib=framework=Security");

    println!("cargo:rustc-link-search=framework={}/build/wcdb/", dst.display());
    println!("cargo:rustc-link-lib=framework=WCDB");

    println!("cargo:rustc-link-search=native={}/build/wcdb/", dst.display());
    println!("cargo:rustc-link-lib=static=sqlcipher");

    println!("cargo:rustc-link-search=native={}/build/wcdb/", dst.display());
    println!("cargo:rustc-link-lib=static=zstd");
}
