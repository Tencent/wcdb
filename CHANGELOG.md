## v1.0.2

#### iOS

- Performance optimization and add Benchmark for performance test, which shows that WCDB take a better performance than FMDB.
  **Batch Write Performance Test**.

  ![](https://raw.githubusercontent.com/wiki/Tencent/wcdb/assets/benchmark/baseline_batch_write.png)
  For more benchmark data, please refer to [our benchmark][Benchmark-iOS].

- Change builtin NSData/NSMutableData column coding from NSCoding to raw data. 
  For those old data before v1.0.2, you can call `[WCTCompatible sharedCompatible].builtinNSDataColumnCodingCompatibleEnabled = YES` to make it compatible.

- Rename `updateTable:...` to `updateRowsInTable:...`  to make the interface more clear. For compatible, you can just do a batch string replacement.

- Add `attach`, `detach`, `vacuum`, `savepoint`, `rollback`, `release`, `reindex`, `explain` statement for WINQ.

- Remove auto increment for `insertOrReplace`.

- Add pragma `cipher`, `cipher_add_random`, `cipher_default_kdf_iter` and so on for WINQ.

- Fix an spelling issue: `statictics` to `statistics`. For compatible, you can just do a batch string replacement.

- Fix some issue that it's not critical.

#### Android

* TODO

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
