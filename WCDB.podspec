# pod lib lint --verbose --skip-import-validation WCDB.podspec
# pod trunk push WCDB.podspec --verbose --skip-import-validation
Pod::Spec.new do |wcdb|
  wcdb.name         = "WCDB"
  wcdb.version      = "1.0.6"
  wcdb.summary      = "WCDB is a cross-platform database framework developed by WeChat."
  wcdb.description  = <<-DESC
                      The WeChat Database, for Objective-C. (If you want to use WCDB from Objective-C, see the "WCDBSwift" pod.)

                      WCDB is an efficient, complete, easy-to-use mobile database framework used in the WeChat application.
                      It can be a replacement for Core Data, SQLite & FMDB.
                      DESC
  wcdb.homepage     = "https://github.com/Tencent/wcdb"
  wcdb.license      = { :type => "BSD", :file => "LICENSE"}
  wcdb.author             = { "sanhuazhang" => "sanhuazhang@tencent.com" }
  wcdb.ios.deployment_target = "7.0"
  wcdb.osx.deployment_target = "10.9"
  wcdb.watchos.deployment_target = "2.0"
  wcdb.tvos.deployment_target = "9.0"
  wcdb.source       = { :git => "https://github.com/Tencent/wcdb.git", :tag => "v#{wcdb.version}" }
  wcdb.public_header_files = "objc/WCDB/WCDB.h", "objc/WCDB/**/*.{h,hpp}"
  wcdb.source_files  = "objc/WCDB/WCDB.h", "objc/WCDB/**/*.{h,m,hpp,cpp,mm}", "repair"
  wcdb.frameworks = "CoreFoundation", "Security", "Foundation"
  wcdb.ios.frameworks = "UIKit"
  wcdb.libraries = "z", "c++"
  wcdb.requires_arc = true
  wcdb.pod_target_xcconfig = {
    "GCC_PREPROCESSOR_DEFINITIONS" => "$(inherited) WCDB_BUILTIN_COLUMN_CODING WCDB_COCOAPODS",
    "HEADER_SEARCH_PATHS" => "$(inherited) ${PODS_ROOT}/WCDB",
    "LIBRARY_SEARCH_PATHS[sdk=macosx*]" => "$(inherited) $(SDKROOT)/usr/lib/system",
    "CLANG_CXX_LANGUAGE_STANDARD" => "gnu++0x",
    "CLANG_CXX_LIBRARY" => "libc++",
  }

  wcdb.dependency 'WCDBOptimizedSQLCipher', '1.1.1'
  wcdb.dependency 'SQLiteRepairKit', '~> 1.0'
end
