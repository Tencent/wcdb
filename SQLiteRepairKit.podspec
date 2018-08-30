# pod lib lint --verbose --skip-import-validation --allow-warnings SQLiteRepairKit.podspec
# pod trunk push --verbose --skip-import-validation --allow-warnings SQLiteRepairKit.podspec
Pod::Spec.new do |sqliterk|
  sqliterk.name         = "SQLiteRepairKit"
  sqliterk.version      = "1.1.6"
  sqliterk.summary      = "Repair kit for SQLite based database"
  sqliterk.description  = <<-DESC
                          sqliterk is a efficient, self-contain repair kit for restoring corrupted SQLite based database.

                          It's basically used in WCDB, which is an efficient, complete, easy-to-use mobile database framework.
                          DESC
  sqliterk.homepage     = "https://github.com/Tencent/wcdb"
  sqliterk.license      = { :type => "BSD", :file => "LICENSE" }
  sqliterk.author             = { "sanhuazhang" => "sanhuazhang@tencent.com" }
  sqliterk.module_name = "sqliterk"
  sqliterk.watchos.deployment_target = "2.0"
  sqliterk.tvos.deployment_target = "9.0"
  sqliterk.osx.deployment_target = "10.9"
  sqliterk.ios.deployment_target = "8.0"
  sqliterk.dependency 'WCDBOptimizedSQLCipher', '~> 1.1.0'
  sqliterk.source       = { :git => "https://github.com/Tencent/wcdb.git", :commit => "5a4cd1b71cb1dcd79b48f334e577c78c2d88db1d" }
  sqliterk.public_header_files = "repair/SQLiteRepairKit.h"
  sqliterk.source_files  = "repair/*.{h,c,cpp}"
  sqliterk.libraries = "z", "c++"
  sqliterk.pod_target_xcconfig = {
    "GCC_PREPROCESSOR_DEFINITIONS" => "SQLITE_HAS_CODEC WCDB_BUILTIN_SQLCIPHER",
    "LIBRARY_SEARCH_PATHS[sdk=macosx*]" => "$(SDKROOT)/usr/lib/system",
    "CLANG_CXX_LANGUAGE_STANDARD" => "gnu++0x",
    "CLANG_CXX_LIBRARY" => "libc++",
    "GCC_SYMBOLS_PRIVATE_EXTERN" => "YES",
    "DEFINES_MODULE" => 'YES',
  }
  sqliterk.static_framework = true
  sqliterk.header_dir = "sqliterk"
  sqliterk.module_map = "repair/SQLiteRepairKit.modulemap"
end
