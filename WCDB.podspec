# pod repo update
# pod repo remove trunk
# pod lib lint --verbose --skip-import-validation --allow-warnings WCDB.podspec --sources='https://github.com/CocoaPods/Specs.git'
# pod trunk push --verbose --allow-warnings WCDB.podspec --skip-import-validation
Pod::Spec.new do |wcdb|
  wcdb.name         = "WCDB"
  wcdb.version      = "2.1.14"
  wcdb.summary      = "WCDB is a cross-platform database framework developed by WeChat."
  wcdb.description  = <<-DESC
                      The WeChat Database, for Objc/Swift/C++. (If you want to use WCDB for Objc/Swift/C++ separately, see the "WCDB.objc", "WCDB.swift" or "WCDB.cpp" pod.)

                      WCDB is an efficient, complete, easy-to-use mobile database framework used in the WeChat application.
                      It can be a replacement for Core Data, SQLite & FMDB.
                      DESC
  wcdb.homepage     = "https://github.com/Tencent/wcdb"
  wcdb.license      = { :type => "BSD", :file => "LICENSE" }
  wcdb.author       = { "Qiuwen-Chen" => "qwchen2008@163.com" }
  wcdb.module_name  = "WCDB"
  wcdb.ios.deployment_target = "11.0"
  wcdb.osx.deployment_target = "10.13"
  wcdb.watchos.deployment_target = "7.0"
  wcdb.tvos.deployment_target = "12.4"
  wcdb.source       = { :git => "https://github.com/Tencent/wcdb.git", :tag => "v#{wcdb.version}" }
  wcdb.public_header_files = [
  	"src/WCDB.h", 
	"src/objc/WCDBObjc.h", 
	"src/objc/Interface.h", 
	"src/objc/core/*.{h,hpp}", 
	"src/objc/builtin/*.{h,hpp}", 
	"src/objc/statement/WCTPreparedStatement.h", 
	"src/objc/migration/WCTDatabase+Migration.h", 
	"src/objc/migration/WCTMigrationInfo.h", 
	"src/objc/compression/WCTDatabase+Compression.h", 
	"src/objc/compression/WCTCompressionInfo.h", 
	"src/objc/transaction/*.{h,hpp}", 
	"src/objc/handle/WCTDatabase+Handle.h", 
	"src/objc/handle/WCTHandle.h", 
	"src/objc/handle/WCTCancellationSignal.h", 
	"src/objc/fts/*.{h,hpp}", 
	"src/objc/monitor/WCTError.h", 
	"src/objc/monitor/WCTDatabase+Monitor.h", 
	"src/objc/monitor/WCTPerformanceInfo.h", 
	"src/objc/chaincall/WCTInsert.h", 
	"src/objc/chaincall/WCTDelete.h", 
	"src/objc/chaincall/WCTUpdate.h", 
	"src/objc/chaincall/WCTSelectable.h", 
	"src/objc/chaincall/WCTSelect.h", 
	"src/objc/chaincall/WCTMultiSelect.h", 
	"src/objc/chaincall/WCTHandle+ChainCall.h", 
	"src/objc/chaincall/WCTTable+ChainCall.h", 
	"src/objc/chaincall/WCTDatabase+ChainCall.h", 
	"src/objc/chaincall/WCTChainCall.h", 
	"src/objc/convenient/*.{h,hpp}", 
	"src/objc/database/WCTDatabase.h", 
	"src/objc/database/WCTDatabase+File.h", 
	"src/objc/database/WCTDatabase+Repair.h", 
	"src/objc/database/WCTDatabase+Config.h", 
	"src/objc/database/WCTDatabase+Memory.h", 
	"src/objc/orm/*.{h,hpp}", 
	"src/objc/orm/**/*.{h,hpp}", 
	"src/objc/table/WCTTable.h", 
	"src/objc/table/WCTTableProtocol.h", 
	"src/objc/table/WCTTable+Table.h", 
	"src/objc/table/WCTDatabase+Table.h", 
	"src/objc/table/WCTHandle+Table.h", 
	"src/objc/bridge/WCTBridgeProperty.h", 
	"src/objc/bridge/WCTBridgeProperty+Cpp.h", 
	"src/common/winq/*.{h,hpp}", 
	"src/common/winq/**/*.{h,hpp}", 
	"src/common/platform/SysTypes.h", 
	"src/common/utility/Shadow.hpp", 
	"src/common/utility/Macro.h", 
	"src/common/utility/CaseInsensitiveList.hpp", 
	"src/common/core/function/scalar/ScalarFunctionModule.hpp", 
	"src/common/core/function/scalar/ScalarFunctionTemplate.hpp", 
	"src/common/core/fts/FTSConst.h", 
	"src/common/core/fts/tokenizer/TokenizerModule.hpp", 
	"src/common/core/fts/tokenizer/TokenizerModuleTemplate.hpp", 
	"src/common/core/fts/tokenizer/BaseTokenizerUtil.hpp", 
	"src/common/core/fts/tokenizer/PinyinTokenizer.hpp", 
	"src/common/core/fts/tokenizer/OneOrBinaryTokenizer.hpp", 
	"src/common/core/fts/auxfunction/FTS5AuxiliaryFunctionTemplate.hpp", 
	"src/common/core/fts/auxfunction/AuxiliaryFunctionModule.hpp", 
	"src/common/core/fts/auxfunction/SubstringMatchInfo.hpp", 
	"src/common/core/fts/FTSError.hpp", 
	"src/common/core/RecyclableHandle.hpp", 
	"src/common/core/Tag.hpp", 
	"src/common/base/Recyclable.hpp", 
	"src/common/base/SharedThreadedErrorProne.hpp", 
	"src/common/base/StringView.hpp", 
	"src/common/base/WCDBOptional.hpp", 
	"src/common/base/WCDBError.hpp", 
	"src/common/base/Data.hpp", 
	"src/common/base/UnsafeData.hpp", 
	"src/common/base/MemberPointer.hpp", 
	"src/cpp/WCDBCpp.h", 
	"src/cpp/**/*.{h,hpp}", 
	"src/bridge/objcbridge/WCTBridgeMacro.h", 
  ]
  wcdb.private_header_files = [
  	"src/bridge/WCDBBridging.h",
	"src/bridge/base/**/*.{h}",
	"src/bridge/cppbridge/**/*.{h}",
	"src/bridge/winqbridge/**/*.{h}",
	"src/bridge/objcbridge/WCTAPIBridge.h"
  ]
  wcdb.source_files  = [
  	"src/WCDB.h",
	"src/common/**/*.{swift,h,hpp,c,cpp,m,mm}",
	"src/objc/**/*.{swift,h,hpp,c,cpp,m,mm}",
	"src/cpp/**/*.{swift,h,hpp,c,cpp,m,mm}",
	"src/swift/**/*.{swift,h,hpp,c,cpp,m,mm}",
	"src/bridge/**/*.{swift,h,hpp,c,cpp,m,mm}"
  ]
  wcdb.exclude_files = [
  	"src/common/platform/WCTFileManager.cpp",
	"src/common/platform/WCTOperationQueue.cpp",
	"src/cpp/tests/**/*.{swift,h,hpp,c,cpp,m,mm}",
	"src/objc/tests/**/*.{swift,h,hpp,c,cpp,m,mm}",
	"src/swift/tests/**/*.{swift,h,hpp,c,cpp,m,mm}",
	"src/bridge/tests/**/*.{swift,h,hpp,c,cpp,m,mm}",
	"src/bridge/include/**/*.{swift,h,hpp,c,cpp,m,mm}",
  ]
  wcdb.preserve_path = "src/support/WCDB.modulemap"
  wcdb.module_map = "src/support/WCDB.modulemap"
  wcdb.frameworks = "CoreFoundation", "Security", "Foundation"
  wcdb.ios.frameworks = "UIKit"
  wcdb.libraries = "z", "c++"
  wcdb.requires_arc = true
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
										"SQLITE_DEFAULT_WAL_AUTOCHECKPOINT=0 " +
										"WCDB_SWIFT_BRIDGE_OBJC=1",
    "HEADER_SEARCH_PATHS" => "${PODS_ROOT}/WCDB",
    "LIBRARY_SEARCH_PATHS[sdk=macosx*]" => "$(SDKROOT)/usr/lib/system",
    "CLANG_CXX_LANGUAGE_STANDARD" => "gnu++14",
    "CLANG_CXX_LIBRARY" => "libc++",
    "GCC_C_LANGUAGE_STANDARD" => "gnu11",
    "VALID_ARCHS" => "arm64e arm64 x86_64",
    "VALID_ARCHS[sdk=watchos*]" => "arm64_32 arm64 86_64",
    "OTHER_CFLAGS" => "-fvisibility-inlines-hidden",
    "OTHER_CPLUSPLUSFLAGS" => "-fvisibility-inlines-hidden",
    "SWIFT_ENABLE_EXPLICIT_MODULES" => 'NO',
    "OTHER_SWIFT_FLAGS" => "-no-verify-emitted-module-interface",
    "SWIFT_ACTIVE_COMPILATION_CONDITIONS" => "WCDB_SWIFT_BRIDGE_OBJC",
  }
  wcdb.header_dir = "WCDB"
  wcdb.swift_versions = '5'
  wcdb.dependency 'WCDBOptimizedSQLCipher', '1.4.7'
end
