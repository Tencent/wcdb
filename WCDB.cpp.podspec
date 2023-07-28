# pod repo update
# pod repo remove trunk
# pod lib lint --verbose --skip-import-validation --allow-warnings WCDB.cpp.podspec --sources='https://github.com/CocoaPods/Specs.git'
# pod trunk push WCDB.cpp.podspec --verbose --skip-import-validation --allow-warnings
Pod::Spec.new do |wcdb|
  wcdb.name         = "WCDB.cpp"
  wcdb.version      = "2.0.3"
  wcdb.summary      = "WCDB is a cross-platform database framework developed by WeChat."
  wcdb.description  = <<-DESC
                      The WeChat Database, for C++. (If you want to use WCDB for Swift or Objective-C, see the "WCDB.swift" pod or "WCDB.objc" pod. If you want to use WCDB for C++/Objc/Swift in one project, see the "WCDB" pod)

                      WCDB is an efficient, complete, easy-to-use mobile database framework used in the WeChat application.
                      It can be a replacement for Core Data, SQLite & FMDB.
                      DESC
  wcdb.homepage     = "https://github.com/Tencent/wcdb"
  wcdb.license      = { :type => "BSD", :file => "LICENSE" }
  wcdb.author       = { "Qiuwen-Chen" => "qwchen2008@163.com" }
  wcdb.module_name  = "WCDBCpp"
  wcdb.ios.deployment_target = "11.0"
  wcdb.osx.deployment_target = "10.13"
  wcdb.watchos.deployment_target = "6.0"
  wcdb.tvos.deployment_target = "12.4"
  wcdb.source       = { :git => "https://github.com/Tencent/wcdb.git", :tag => "v#{wcdb.version}" }
  wcdb.public_header_files = "src/cpp/WCDBCpp.h", "src/cpp/**/*.{h,hpp}", "src/common/winq/*.{h,hpp}", "src/common/winq/**/*.{h,hpp}", "src/common/platform/SysTypes.h", "src/common/utility/Shadow.hpp", "src/common/utility/Macro.h", "src/common/utility/CaseInsensitiveList.hpp", "src/common/core/fts/FTSConst.h", "src/common/core/fts/tokenizer/TokenizerModule.hpp", "src/common/core/fts/tokenizer/TokenizerModuleTemplate.hpp", "src/common/core/fts/tokenizer/BaseTokenizerUtil.hpp", "src/common/core/fts/tokenizer/PinyinTokenizer.hpp", "src/common/core/fts/tokenizer/OneOrBinaryTokenizer.hpp", "src/common/core/fts/auxfunction/FTS5AuxiliaryFunctionTemplate.hpp", "src/common/core/fts/auxfunction/AuxiliaryFunctionModule.hpp", "src/common/core/fts/auxfunction/SubstringMatchInfo.hpp", "src/common/core/fts/FTSError.hpp", "src/common/core/RecyclableHandle.hpp", "src/common/core/Tag.hpp", "src/common/base/Recyclable.hpp", "src/common/base/SharedThreadedErrorProne.hpp", "src/common/base/StringView.hpp", "src/common/base/WCDBOptional.hpp", "src/common/base/WCDBError.hpp", "src/common/base/Data.hpp", "src/common/base/UnsafeData.hpp"
  wcdb.source_files  = "src/common/**/*.{h,hpp,c,cpp}", "src/cpp/**/*.{h,hpp,c,cpp}", "src/objc/core/WCTFileManager.mm", "src/objc/core/WCTOperationQueue.mm", "src/objc/core/WCTFoundation.h", "src/objc/core/WCTFoundation.mm"
  wcdb.exclude_files = "src/cpp/tests/**/*.{h,hpp,c,cpp}", "src/common/platform/WCTFileManager.cpp", "src/common/platform/WCTOperationQueue.cpp", "src/objc/core/WCTOperationQueue.mm", "src/common/core/operate/OperationQueueForMemory.cpp"
  wcdb.subspec 'no-arc' do |sna|
    sna.requires_arc = false
    sna.source_files = "src/objc/core/WCTOperationQueue.mm", "src/common/core/operate/OperationQueueForMemory.cpp", "src/common/core/operate/OperationQueueForMemory.hpp"
  end
  wcdb.frameworks = "CoreFoundation", "Security", "Foundation"
  wcdb.ios.frameworks = "UIKit"
  wcdb.libraries = "z", "c++"
  wcdb.requires_arc = true
  wcdb.pod_target_xcconfig = { 
    "GCC_PREPROCESSOR_DEFINITIONS" => "SQLITE_WCDB=1 SQLITE_WCDB_LOCK_HOOK=1 SQLITE_WCDB_CHECKPOINT_HANDLER=1 SQLITE_WCDB_SUSPEND=1 SQLITE_WCDB_IMPROVED_CHECKPOINT=1 SQLITE_HAS_CODEC SQLITE_DEFAULT_PAGE_SIZE=4096 SQLITE_MAX_VARIABLE_NUMBER=99 SQLITE_DEFAULT_SYNCHRONOUS=1 SQLITE_DEFAULT_WAL_SYNCHRONOUS=1 SQLITE_DEFAULT_LOCKING_MODE=0 SQLITE_DEFAULT_WAL_AUTOCHECKPOINT=0",
    "HEADER_SEARCH_PATHS" => "${PODS_ROOT}/WCDB",
    "LIBRARY_SEARCH_PATHS[sdk=macosx*]" => "$(SDKROOT)/usr/lib/system",
    "CLANG_CXX_LANGUAGE_STANDARD" => "gnu++14",
    "CLANG_CXX_LIBRARY" => "libc++",
    "GCC_C_LANGUAGE_STANDARD" => "gnu11",
    "VALID_ARCHS" => "arm64e arm64 x86_64",
    "VALID_ARCHS[sdk=watchos*]" => "arm64_32 arm64 x86_64",
    "OTHER_CFLAGS" => "-fvisibility-inlines-hidden",
    "OTHER_CPLUSPLUSFLAGS" => "-fvisibility-inlines-hidden",
  }
  wcdb.user_target_xcconfig = { 
    "VALID_ARCHS" => "arm64e arm64 x86_64",
    "VALID_ARCHS[sdk=watchos*]" => "arm64_32 armv7k",
  }
  wcdb.header_dir = "WCDB"
  wcdb.dependency 'WCDBOptimizedSQLCipher', '~> 1.4.1'
end
