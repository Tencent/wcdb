## v1.0.4

### iOS

* Builtin `WCTColumnCoding` supports all `id<NSCoding>` objects now.

* Compatible with iOS 11.

* `Fullfsync` is used by default for data integrity.

* Add `-initWithExistingTag:` for `WCTDatabase` to get existing database without path.

  ```objective-c
  WCTDatabase* database = [WCTDatabase [alloc] initWithPath:path];
  database.tag = 123;
  WCTDatabase* withoutPath = [[WCTDatabase alloc] initWithExistingTag:123];
  ```

* Some minor bug fixes, performance improvement and code refactor.

## v1.0.3

#### Repair Kit

* Fix INTEGER PRIMARY KEY columns not properly recovered.

#### iOS

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

#### iOS

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

#### iOS

* Add CocoaPods support.
* Add iOS 7 and macOS 10.9 support. Apps using WCDB can target iOS 7 now.
* Fix an issue that `[WCTDatabase canOpen]` never return YES.
* Fix an issue that the global tracer return some odd values.
* Add `@autoreleasepool` in `runTransaction` to avoid OOM.

#### Android

* Add `x86_64` ABI support.
* Publish debug version of AAR and native symbols. To reference debug version of WCDB library, modify your `build.gradle`

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
