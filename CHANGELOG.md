## v2.1.14

* Support 16kb pagesize for Android
* Supports using ksp in Kotlin version 2.1.0+
* Fix stack overflow crash
* Fix some known bugs

## v2.1.13

* Fix compile error of Swift Package Manager
* Fix crash caused by in-transaction handle
* Convert ANSI error msg in windows to UTF8

## v2.1.12

* Fix build error on Xcode 26 beta
* Fix empty string crash in java/kotlin winq
* Fix linking error on linux x86

## v2.1.11

* Support to open database in readonly mode
* Support to create in-memory database in java/c++/swift
* Fix the exception problem of tableExist method in Java/Kotlin

## v2.1.10

* Disable checkpoint fullsync in apple platform, it will significantly reduce disk IO
* Add main db file protection to windows
* Reduce memory cost of data compression
* Reduce handle usage of data backup
* Support to detect schema corruption
* Fix some known bugs and compile issues

## v2.1.9

* Add lite mode (Set journal mode and synchronous flag to OFF)
* Fix memory leak and other known bug

## v2.1.8

* Increase migration speed
* Fix compile error of SwiftPM in Xcode 16

## v2.1.7

* WCDB C++ supports OpenHarmony OS
* Support to recompress existing data with new configuration
* Support auto vacuum
* Support Swift6
* Fix some compile issues

## v2.1.6

* Some bugfix for data repair, Java/Kotlin ORM and Swift query
* Fix some compile error and warning

## v2.1.5

* Support legacy mmicu tokenizer in WCDB Java/Kotlin
* Support to rollback compression
* Improve performance of vacuum
* Add valueOr interface to Optional
* Some bugfix for compression and vacuum

## v2.1.4

* Fix the compile error of WCDB Java/Koltin caused by minifyEnabled flag
* Add regular memory verification to Zstd dict

## v2.1.3

* Add ProGuard config for WCDB Java/Kotlin
* Support to build WCDB C++ as a framework on Apple platform with CMake
* Support to insert an array of object pointers or object shared pointers in WCDB C++
* Fix the bug that error cannot be obtained through database

## v2.1.2

* Some bugfix for data compression
* Fix compile error of WCDB Objc on Carthage
* Fix compile error of WCDB C++ when built as a CMake submodule

## v2.1.1

* Some bugfix for WCDB C++ and WCDB Swift
* Fix compile error on Mac Catalyst

## v2.1.0

* Add WCDB Java/Kotlin
* Support data compression
* Support incremental data backup
* Support to configure a filter condition for data migration
* Support to use std::optional and std::shared_ptr in C++ ORM
* C++ ORM supports inheritance
* Add more monitoring capabilities
* WCDB Swift supports Swift Package Manager

## v2.0.4

* Support to asynchronously interrupt database operations
* Optimize the handle management mechanism
* Some bugfix for WCDB C++ in Windows

## v2.0.3

* Transaction supports nesting
* Some bug fix for data migration
* Fix compile error in new Xcode

## v2.0.2.5

* Fix compile issue caused by header

## v2.0.2

* WCDB C++ supports Windows
* Optimizie C++ ORM performance
* Downgrade the minimum supported system version for iPhone and Apple watch

## v2.0.1

* Support databases created by old versions of sqlcipher
* Fix crash of WCDB Swift in x86 emulator
* Support insertOrIgnore

## v2.0.0

* Add WCDB C++
* New implementation of WCDB Swift
* New implementation of Winq
* New implementation of database repair
* Support data migration
* Support FTS5

## v1.1.0

#### iOS/macOS
* Support Swift 5 for Xcode 14.
* Fix compile error for SQLCipher 4.1.0.
* Fix some bug.

## v1.0.8.2

#### iOS/macOS
* Support Swift 4.2 for Xcode 10.2

## v1.0.8.1

#### iOS/macOS
* Fix a bug that archive failed with Xcode new build system.

## v1.0.8

#### Android

* Support for [Room library](https://developer.android.com/topic/libraries/architecture/room) from Android Jetpack. See README and [Wiki](https://github.com/Tencent/wcdb/wiki/Android-WCDB-%E4%BD%BF%E7%94%A8-Room-ORM-%E4%B8%8E%E6%95%B0%E6%8D%AE%E7%BB%91%E5%AE%9A) for details.

## v1.0.7.5

#### iOS/macOS

* Compatible with Swift 4.2.
* Enable FTS5 macro for sqlcipher.

## v1.0.7

#### iOS/macOS

* Fix nil string bug while inserting empty string.
* Code template is not installed automatically now.
* Reduce size of binary and provide a non-bitcode scheme for Carthage.
* Avoid conflict between builtin SQLCipher and other SQLite based library(including system library).

## v1.0.6.2

#### iOS/macOS

It's a bug fixed version. Since Swift 4.1.x contains [bugs](https://github.com/Tencent/wcdb/issues/283) that make WCDB fails to compile, developers should use Xcode 10(with Swift 4.2).

##### Swift
* Compatible with Swift 4.2. The `ColumnEncodable` and `ColumnDecodable` is now changed. Check the code snippet, file template or wiki for the new usage.
* Use `Double` column type for `Date`.

FYI, a refactor is needed to fit the new conditional conformance design of Swift 4.2. We will finish it in next version.

#### Android

* Use C++14 and libc++_static runtime on JNI routines.
* Fix "no implementation found" log on libwcdb.so initialization.
* Fix ProGuard rules when importing from AAR package.

## v1.0.6

#### iOS/macOS

It's the first release for WCDB Swift, which contains exactly the same features as the ObjC version, including:

* Object-Relational-Mapping based on Swift 4.0 `Codable` protocol.
* WCDB Integrated Language Query.
* Multithreading safety and concurrency.
* Encryption based on SQLCipher.
* Protection for SQL injection.
* Full text search.
* Corruption recovery.
* ...

For further information, please check tutorial on wiki.

#### Android

* Migrate to gradle plugin 3.0.0, target API 26, and minimal API 14.
* Support NDK r16, change default toolchain to clang.
* Various bug fixes.

## v1.0.5

#### iOS/macOS

* Builtin full-text search support for ORM.
```objc
WCTProperty *tableProperty = WCTSampleFTSData.PropertyNamed(tableNameFTS).match("Eng*")];

[databaseFTS getObjectsOfClass:WCTSampleFTSData.class fromTable:tableNameFTS where:tableProperty.match("Eng*")];
```
* Support read-only databases.
* Some minor bug fixes and code refactor.

#### Android

* Optimize asynchronous checkpointer, greatly improve write performance with WAL and asynchronous checkpointing.
```java
SQLiteDatabase db = SQLiteDatabase.openOrCreateDatabaseInWalMode(...);
db.setAsyncCheckpointEnabled(true);
```
* Add benchmark for asynchronous checkpointer.
* Add connection pooling friendly interface. `SQLiteDatabase.setSynchronousMode()` to set database synchronization mode.
* Enable `dbstat` virtual table while compiling.

## v1.0.4

#### Repair Kit

* Add `sqliterk_cancel` function to cancel ongoing output operations.
* Add corresponding Java interface to cancel operations on Android.

#### iOS/macOS

* Builtin `WCTColumnCoding` supports all `id<NSCoding>` objects now.
* Compatible with iOS 11.
* `Fullfsync` is used by default for data integrity.
* Add `-initWithExistingTag:` for `WCTDatabase` to get existing database without path.

```objc
WCTDatabase* database = [WCTDatabase [alloc] initWithPath:path];
database.tag = 123;
WCTDatabase* withoutPath = [[WCTDatabase alloc] initWithExistingTag:123];
```

* Some minor bug fixes, performance improvement and code refactor.

#### Android

* Add asynchronous checkpointing support and custom checkpointing callback. This can
  improve performance in WAL mode.

```java
SQLiteDatabase db = SQLiteDatabase.openOrCreateDatabaseInWalMode(...);

// Use asynchronous checkpointing.
db.setAsyncCheckpointEnabled(true);

// OR use custom checkpointer.
SQLiteCheckpointListener callback = new SQLiteCheckpointListener() {
    //...
};
db.setCheckpointCallback(callback);
```

* Add `SQLiteTrace.onConnectionObtained(...)` interface to trace concurrency performance.
* Add cancelable version of `SQLiteDatabase.execSQL()`. See `CancellationSignal` for details.

```java
CancellationSignal signal = new CancellationSignal();
db.execSQL(longRunningSQL, args, signal);

// on another thread
signal.cancel();
```

* Enable `SQLITE_ENABLE_FTS3_PARENTHESIS` compilation option on SQLCipher, which enables `AND`, `OR` operators in FTS3/4.
* Use `CancellationSignal` for canceling `BackupKit`, `RecoverKit` and `RepairKit` operations. See repair sample for details.
* Add callback interface for `RepairKit` to show progress to the users. See `RepairKit.Callback` and `RepairKit.setCallback()`.
* Do not load `libwcdb.so` if it's already loaded on the first use. This makes WCDB compatible to Tinker framework.
* Various bug fixes.

## v1.0.3

#### Repair Kit

* Fix INTEGER PRIMARY KEY columns not properly recovered.

#### iOS/macOS

* Add `WCTColumnCoding` support for all `WCTValue`. Developers can use `id<WCTColumnCoding>` objects for WINQ and all interfaces.
```objc
//WINQ
NSDate *now = [NSDate date];
[database getObjectsOfClass:Message.class fromTable:tableName where:Message.modifedTime==now];

//Interfaces
[database updateAllRowsInTable:tableName 
          onProperty:Message.modifiedTime 
            withValue:[NSDate date]];
```
* Add monitor for all executed SQL to check WINQ correctness.
```objc
//SQL Execution Monitor
[WCTStatistics SetGlobalSQLTrace:^(NSString *sql) {
  NSLog(@"SQL: %@", sql);
}];
```
* Update `WCTTableCoding` XCode file template for the best practice of isolating Objective C++ codes. See Wiki page for details.
* Some minor bug fixes.

#### Android

* Add `CursorWindow.windowSize(int)` static method to set or get default size for cursor windows.
* `SQLiteDatabase.dump()` reports IDs for all threads that hold database connections, to aid dead-lock debugging.
* Fix crashing on devices fail to load ICU library.
* Fix `SQLiteTrace.onSQLExecuted(...)` reports negative execution time.

## v1.0.2

#### iOS/macOS

* Performance optimization and benchmark. See Wiki page for details.
* Change builtin `NSData` or `NSMutableData` column coding to raw data format. To be compatible with earlier versions, call `[WCTCompatible sharedCompatible].builtinNSDataColumnCodingCompatibleEnabled = YES`.
* Add `attach`, `detach`, `vacuum`, `savepoiint`, `rollback`, `release`, `reindex`, `explain` statement and SQLCipher related pragma to WINQ.
* Remove auto increment for `insertOrReplace`.
* Rename `updateTable` to `updateRowsInTables`, and `statictics`(typo) to `statistics`.
* Some minor bug fixes.

#### Android

* Performance optimization and benchmark. See Wiki page for details.
* Expose ProGuard rules to AAR package. Fix crash when minify is enabled in gradle.

## v1.0.1

#### iOS/macOS

* Add CocoaPods support.
* Add iOS 7 and macOS 10.9 support. Apps using WCDB can target iOS 7 now.
* Fix an issue that `[WCTDatabase canOpen]` never return YES.
* Fix an issue that the global tracer return some odd values.
* Add `@autoreleasepool` in `runTransaction` to avoid OOM.

#### Android

* Add `x86_64` ABI support.
* Publish debug version of AAR and native symbols. To reference debug version of WCDB library, modify your `build.gradle`.

```gradle
dependencies {
    // Append ":debug@aar" to reference debug library.
    compile 'com.tencent.wcdb:wcdb-android:1.0.1:debug@aar'
}
```

* **Device-locking** is available in cipher options. Databases created with device-locking enabled can be only accessed in
  the same device where the databases are created. Device-locking is currently **in alpha stage**. You can enable it with
  the following code:

```java
SQLiteCipherSpec cipher = new SQLiteCipherSpec()
        // add the following line to enable device-locking
        .setCipher(SQLiteCipherSpec.CIPHER_DEVLOCK);
SQLiteDatabase db = SQLiteDatabase.openOrCreateDatabase(path, key, cipher, ...);
```

* Various bug fixes.

## v1.0.0

Initial release.
