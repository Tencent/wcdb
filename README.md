# Introduce

WCDB is a multi-platform database framework based on [SQLCipher][sqlcipher], 
with features such as corruption recovery and concurrent access via connection pooling.

# WCDB for iOS

WCDB provide a flexible ORM (Object Relational Mapping) interface on iOS platform. 
See documentation for details.

## Getting Started

// TODO

# WCDB for Android

WCDB uses the same interface as Android SQLite Database API.

## Getting Started

To include WCDB to your project, choose either way: import prebuilt AAR package, or import via Maven.

### Import Prebuilt AAR Package

  1. **Download AAR package from release page.**
  2. **Import the AAR as new module.** In Android Studio, select `File -> New -> New Module...` menu and choose `"Import JAR/AAR Package"`.
  3. **Add a dependency on the new module.** This can be done using `File -> Project Structure...` in Android Studio,
  or by adding following code to application's `build.gradle`:

```groovy
dependencies {
    // Change "wcdb" to the actual module name specified in step 2.
    compile project(':wcdb')
}
```

### Import via Maven

Edit `build.gradle` and add following lines.

```groovy
dependencies {
    // Change "0.4.1" to any available version.
    compile 'com.tencent.wcdb:wcdb-android:0.4.1'
}
```

This will cause Gradle to download AAR package from jcenter while building your application.

You may add additional repositories by adding the following lines:

```groovy
repositories {
    mavenLocal()
    maven {
        url 'http://your.maven.server/path/to/repo'
    }
}
```

### Mirgate from Android Builtin SQLite

WCDB has interfaces very similar to Android SQLite Database APIs. To migrate you application from
AOSP API, change import path from `android.database.*` to `com.tencent.wcdb.*`, and 
`android.database.sqlite.*` to `com.tencent.wcdb.database.*`. After import path update, 
your application links to WCDB instead of AOSP API.

To open or create an encrypted database, use with-password versions of 
`SQLiteDatabase.openOrCreateDatabase()`, `SQLiteOpenHelper.getWritableDatabase()`, 
or `Context.openOrCreateDatabase()`.

*Note: WCDB uses `byte[]` for password instead of `String` in SQLCipher Android Binding.*

```java
String password = "MyPassword";
SQLiteDatabase db = SQLiteDatabase.openOrCreateDatabase("/path/to/database", password.getBytes(), 
        null, null);
```

### Redirect Log Output

By default, WCDB prints its log message to system logcat. You may want to change this
behavior in order to, for example, save logs for troubleshooting. WCDB can redirect
all of its log outputs to user-defined routine using `Log.setLogger(LogCallback)`
method.

## Build from Sources

### Build WCDB Android with Prebuilt Dependencies

WCDB itself can be built apart from its dependencies using Gradle or Android Studio. 
To do so, make sure you have checked out `external` submodule then run:

```bash
$ cd android
$ ./gradlew build
```

Building WCDB requires Android NDK installed. If Gradle failed to find your SDK and/or 
NDK, you may need to create a file named `local.properties` on the `android` directory 
with content:

```
sdk.dir=path/to/sdk
ndk.dir=path/to/ndk
```

Android Studio will do this for you when the project is imported.

### Build Dependencies from Sources

You can rebuild all dependencies, i.e. OpenSSL and SQLCipher, if you wish. In this case,
Android NDK *(version r11c or later is recommended)*, a working C compiler on the host 
system, Perl 5 and a bash environment is needed. 

To build dependencies, set `ANDROID_NDK_ROOT` environment variable to your NDK path, 
then run `build-depends-android.sh`

```bash
$ export ANDROID_NDK_ROOT=/path/to/ndk
$ ./build-depends-android.sh
```

Be patient, OpenSSL and SQLCipher static libraries will be built in `external` directory.

## Comparison to SQLCipher Official Android Binding

SQLCipher also provides [its own Android binding][sqlcipher-android]. WCDB works in a similar way, with several improvements:

  * WCDB Android framework is derived from moden Android framework, which provides connection pooling 
    for concurrent access to database in WAL journal mode. See [Android SDK reference][sdk-ref-sqlitedb].
  * `arm64-v8a` archtecture is supported. This enables WCDB to work with other native 64-bit libraries.
  * Sources, including Java and JNI parts, are debuggable using Android Studio 2.2+.
  * Some additional features is available with extra APIs, such as database corruption recovery
    (`RepairKit`, `BackupKit`, `RecoverKit`) and alternative Cursor implementations
    (`SQLiteDirectCursor`, `SQLiteAsyncCursor`).

[sqlcipher]: https://github.com/sqlcipher/sqlcipher
[ios-doc]: https://xxxx/yyy
[sqlcipher-android]: https://github.com/sqlcipher/android-database-sqlcipher
[sdk-ref-sqlitedb]: https://developer.android.com/reference/android/database/sqlite/SQLiteDatabase.html
