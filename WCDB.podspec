# pod lib lint WCDB.podspec --verbose --skip-import-validation 
# pod trunk push WCDB.podspec --verbose --skip-import-validation
Pod::Spec.new do |wcdb|
  wcdb.name         = "WCDB"
  wcdb.version      = "1.1.0"
  wcdb.summary      = "WCDB is a efficient, complete, easy-to-use mobile database framework"
  wcdb.description  = <<-DESC
                      The WeChat Database, for Objective-C. (If you want to use WCDB for Swift, see the "WCDBSwift" pod.)

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
  wcdb.module_name = "WCDB"
  wcdb.dependency 'WCDBOptimizedSQLCipher', '~> 1.0'
  wcdb.dependency 'SQLiteRepairKit', '~> 1.0'
  wcdb.source       = { :git => "https://github.com/Tencent/wcdb.git", :tag => "v#{wcdb.version}" }

  header_files = "objc/source/WCDB.h", "objc/source/**/*.h", "objc/source/**/*.hpp"
  arc_source_files = "objc/source/**/*.cpp", "objc/source/**/*.m", "objc/source/**/*[^N][^o][^A][^R][^C].mm"
  noarc_source_files = "objc/source/interface/orm/accessor/WCTRuntimeBaseAccessor+NoARC.mm",
                       "objc/source/interface/orm/accessor/WCTRuntimeObjCAccessor+NoARC.mm", 
                       "objc/source/interface/chaincall/WCTSelectBase+NoARC.mm"
  wcdb.public_header_files = header_files
  wcdb.source_files = header_files + arc_source_files + noarc_source_files
  wcdb.requires_arc = arc_source_files

  wcdb.ios.frameworks = "UIKit"
  wcdb.libraries = "c++"
  wcdb.prepare_command = "cd tools/templates; sh install.sh || true; cd ../..;"
  wcdb.xcconfig = {
    "GCC_PREPROCESSOR_DEFINITIONS" => "$(inherited) WCDB_BUILTIN_COLUMN_CODING",
    "HEADER_SEARCH_PATHS" => "$(inherited)",
    "LIBRARY_SEARCH_PATHS[sdk=macosx*]" => "$(inherited) $(SDKROOT)/usr/lib/system",
    "CLANG_CXX_LANGUAGE_STANDARD" => "gnu++0x",
    "CLANG_CXX_LIBRARY" => "libc++",
  }
end
