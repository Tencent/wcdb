# pod repo update
# pod repo remove trunk
# pod lib lint --verbose --allow-warnings WCDB.swift.podspec --sources='https://github.com/CocoaPods/Specs.git'
# pod trunk push --verbose --allow-warnings WCDB.swift.podspec
Pod::Spec.new do |wcdb|
  wcdb.name         = "WCDB.swift"
  wcdb.version      = "2.1.11"
  wcdb.summary      = "WCDB is a cross-platform database framework developed by WeChat."
  wcdb.description  = <<-DESC
                      The WeChat Database, for Swift. (If you want to use WCDB for Objective-C or C++, see the "WCDB.objc" pod or "WCDB.cpp" pod. If you want to use WCDB for C++/Objc/Swift in one project, see the "WCDB" pod)

                      WCDB is an efficient, complete, easy-to-use mobile database framework used in the WeChat application.
                      It can be a replacement for Core Data, SQLite & FMDB.
                      DESC
  wcdb.homepage     = "https://github.com/Tencent/wcdb"
  wcdb.module_name  = "WCDBSwift"
  wcdb.license      = { :type => "BSD", :file => "LICENSE" }
  wcdb.author       = { "Qiuwen-Chen" => "qwchen2008@163.com" }
  wcdb.ios.deployment_target = "11.0"
  wcdb.osx.deployment_target = "10.13"
  wcdb.watchos.deployment_target = "7.0"
  wcdb.tvos.deployment_target = "12.4"
  wcdb.source       = { :git => "https://github.com/Tencent/wcdb.git", :tag => "v#{wcdb.version}" }
  wcdb.private_header_files = "src/bridge/**/*.{h}"
  wcdb.source_files  = [
  	"src/swift/**/*.{swift}",
  	"src/bridge/**/*.{swift,h,hpp,c,cpp,m,mm}",
  	"src/common/**/*.{swift,h,hpp,c,cpp,m,mm}",
  	"src/objc/core/WCTFileManager.mm",
    "src/objc/core/WCTOperationQueue.mm", 
  	"src/objc/core/WCTFoundation.h",
  	"src/objc/core/WCTFoundation.mm",
  	"src/objc/core/WCTFTSTokenizerUtil.h",
  	"src/objc/core/WCTFTSTokenizerUtil.mm",
  	"src/objc/core/WCTDeclaration.h",
  	"src/objc/core/WCTValue.h",
  	"src/objc/orm/coding/WCTColumnCoding.h",
  	"src/objc/orm/macro/WCTPropertyMacro.h",
  	"src/objc/orm/coding/WCTTableCoding.h",
  	"src/objc/core/WCTConvertible.h",
  	"src/objc/core/WCTCommon.h",
  	"src/objc/core/WCTOptional.h",
  	"src/objc/core/WCTTag.h"
  ]
  wcdb.exclude_files = [
  	"src/swift/tests/**/*.{swift,h,hpp,c,cpp,m,mm}",
  	"src/bridge/tests/**/*.{swift,h,hpp,c,cpp,m,mm}",
    "src/bridge/include/**/*.{swift,h,hpp,c,cpp,m,mm}",
  	"src/common/platform/WCTFileManager.cpp",
  	"src/common/platform/WCTOperationQueue.cpp",
  ]
  wcdb.requires_arc = true
  wcdb.preserve_path = "src/support/WCDBSwift.modulemap"
  wcdb.module_map = "src/support/WCDBSwift.modulemap"
  wcdb.frameworks = "CoreFoundation", "Security", "Foundation"
  wcdb.libraries = "z", "c++"
  wcdb.pod_target_xcconfig = { 
    "GCC_PREPROCESSOR_DEFINITIONS" =>   "SQLITE_WCDB=1 " +
										"SQLITE_WCDB_LOCK_HOOK=1 " +
										"SQLITE_WCDB_CHECKPOINT_HANDLER=1 " +
										"SQLITE_WCDB_SUSPEND=1 " +
										"SQLITE_WCDB_IMPROVED_CHECKPOINT=1 " +
										"SQLITE_HAS_CODEC " +
										"SQLITE_DEFAULT_PAGE_SIZE=4096 " +
										"SQLITE_DEFAULT_SYNCHRONOUS=1 " +
										"SQLITE_DEFAULT_WAL_SYNCHRONOUS=1 " +
										"SQLITE_DEFAULT_LOCKING_MODE=0 " +
										"SQLITE_DEFAULT_WAL_AUTOCHECKPOINT=0",
    'SWIFT_WHOLE_MODULE_OPTIMIZATION' => 'YES',
    'APPLICATION_EXTENSION_API_ONLY' => 'YES',
    "HEADER_SEARCH_PATHS" => "${PODS_ROOT}/WCDBSwift",
    "LIBRARY_SEARCH_PATHS[sdk=macosx*]" => "$(SDKROOT)/usr/lib/system",
    "CLANG_CXX_LANGUAGE_STANDARD" => "gnu++14",
    "CLANG_CXX_LIBRARY" => "libc++",
    "GCC_C_LANGUAGE_STANDARD" => "gnu11",
    "VALID_ARCHS" => "arm64e arm64 x86_64",
    "VALID_ARCHS[sdk=watchos*]" => "arm64_32 arm64 x86_64",
    "OTHER_CFLAGS" => "-fvisibility-inlines-hidden",
    "OTHER_CPLUSPLUSFLAGS" => "-fvisibility-inlines-hidden",
    "OTHER_SWIFT_FLAGS" => "-no-verify-emitted-module-interface",
  }
  wcdb.swift_versions = '5'
  wcdb.dependency 'WCDBOptimizedSQLCipher', '1.4.7'
end
