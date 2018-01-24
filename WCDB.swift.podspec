# pod lib lint WCDB.swift.podspec --verbose 
# pod trunk push WCDB.swift.podspec --verbose
Pod::Spec.new do |wcdb|
  wcdb.name         = "WCDB.swift"
  wcdb.version      = "1.1.0"
  wcdb.summary      = "WCDB is a cross-platform database framework developed by WeChat."
  wcdb.description  = <<-DESC
                      The WeChat Database, for Swift. (If you want to use WCDB for Objective-C, see the "WCDB" pod.)

                      WCDB is an efficient, complete, easy-to-use mobile database framework used in the WeChat application.
                      It can be a replacement for Core Data, SQLite & FMDB.
                      DESC
  wcdb.homepage     = "https://github.com/Tencent/wcdb"
  wcdb.license      = { :type => "BSD", :file => "LICENSE"}
  wcdb.author             = { "sanhuazhang" => "sanhuazhang@tencent.com" }
  wcdb.ios.deployment_target = "8.0"
  wcdb.osx.deployment_target = "10.9"
  wcdb.watchos.deployment_target = "2.0"
  wcdb.tvos.deployment_target = "9.0"
  wcdb.source       = { :git => "https://github.com/Tencent/wcdb.git", :tag => "v#{wcdb.version}" }
  wcdb.source_files  = "swift/source/**/*.swift", "swift/source/**/**/*.swift", "swift/source/util/*.{h,c}"
  wcdb.module_name = "WCDBSwift"
  wcdb.dependency 'WCDBOptimizedSQLCipher', '~> 1.0'
  wcdb.dependency 'SQLiteRepairKit', '~> 1.0'
  wcdb.requires_arc = true
  wcdb.prepare_command = "cd tools/templates; sh install.sh || true; cd ../..;"
  wcdb.pod_target_xcconfig = { 'SWIFT_WHOLE_MODULE_OPTIMIZATION' => 'YES',
                               'APPLICATION_EXTENSION_API_ONLY' => 'YES' }
  wcdb.xcconfig = {
    "GCC_PREPROCESSOR_DEFINITIONS" => "$(inherited) WCDB_BUILTIN_SQLCIPHER SQLITE_HAS_CODEC",
    "HEADER_SEARCH_PATHS" => "$(inherited) ${PODS_ROOT}/WCDBSwift",
    "LIBRARY_SEARCH_PATHS[sdk=macosx*]" => "$(inherited) $(SDKROOT)/usr/lib/system",
    "OTHER_SWIFT_FLAGS[config=Release]" => "-D WCDB_HAS_CODEC",
    "OTHER_SWIFT_FLAGS[config=Release][sdk=iphonesimulator*]" => "-D WCDB_HAS_CODEC -D WCDB_IOS",
    "OTHER_SWIFT_FLAGS[config=Release][sdk=iphoneos*]" => "-D WCDB_HAS_CODEC -D WCDB_IOS",
    "OTHER_SWIFT_FLAGS[config=Debug]" => "-D WCDB_HAS_CODEC -D DEBUG",
    "OTHER_SWIFT_FLAGS[config=Debug][sdk=iphonesimulator*]" => "-D WCDB_HAS_CODEC -D WCDB_IOS -D DEBUG",
    "OTHER_SWIFT_FLAGS[config=Debug][sdk=iphoneos*]" => "-D WCDB_HAS_CODEC -D WCDB_IOS -D DEBUG",
    "SWIFT_VERSION" => "4.0",
  }
end
