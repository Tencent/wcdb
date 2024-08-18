// swift-tools-version: 5.5
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "WCDBSwift",
    platforms: [
        .macOS(.v10_13),
        .watchOS(.v4),
        .tvOS(.v12),
        .iOS(.v12)],
    products: [
        .library(name: "WCDBSwift", targets: ["WCDBSwift"]),
        .library(name: "WCDBSwiftDynamic", type: .dynamic, targets: ["WCDBSwift"]),
    ],
    dependencies: [
        .package(url: "https://github.com/Tencent/sqlcipher", .exactItem("1.4.5"))
    ],
    targets: [
        .target(
            name: "objc-core",
            path: "src/objc/core",
            sources: ["WCTFileManager.mm", "WCTFoundation.mm", "WCTFTSTokenizerUtil.mm", "WCTOperationQueue.mm"],
            publicHeadersPath:"",
            cSettings: [
                .headerSearchPath("../../**"),
            ],
            linkerSettings: [
                .linkedFramework("UIKit", .when(platforms: [.iOS])),
            ]
        ),
        .target(
            name: "common",
            dependencies: ["sqlcipher", .target(name: "objc-core")],
            path: "src/common",
            exclude: [
                "repair/sqlite",
                "platform/WCTFileManager.cpp",
                "platform/WCTOperationQueue.cpp",
            ],
            publicHeadersPath:"",
            cSettings: [
                .define("SQLITE_WCDB", to: "1"),
                .define("SQLITE_WCDB_LOCK_HOOK", to: "1"),
                .define("SQLITE_WCDB_CHECKPOINT_HANDLER", to: "1"),
                .define("SQLITE_WCDB_SUSPEND", to: "1"),
                .define("SQLITE_WCDB_IMPROVED_CHECKPOINT", to: "1"),
                .define("SQLITE_HAS_CODEC"),
                .define("SQLITE_DEFAULT_PAGE_SIZE", to: "4096"),
                .define("SQLITE_DEFAULT_SYNCHRONOUS", to: "1"),
                .define("SQLITE_DEFAULT_WAL_SYNCHRONOUS", to: "1"),
                .define("SQLITE_DEFAULT_LOCKING_MODE", to: "0"),
                .define("SQLITE_DEFAULT_WAL_AUTOCHECKPOINT", to: "0"),
                .headerSearchPath("**")
            ],
            linkerSettings: [
                .linkedLibrary("z"),
            ]
        ),
        .target(
            name: "bridge",
            dependencies: [.target(name: "common")],
            path: "src/bridge",
            exclude: [
                "tests",
            ],
            cSettings: [
                .headerSearchPath("../**")
            ]
        ),
        .target(
            name: "WCDBSwift",
            dependencies: [.target(name: "bridge")],
            path: "src/swift",
            exclude: [
                "tests"
            ],
            publicHeadersPath: ".",
            cSettings: [
                .headerSearchPath("../bridge/**")
            ]
        )
    ],
    cLanguageStandard: .c11,
    cxxLanguageStandard: .cxx14
)
