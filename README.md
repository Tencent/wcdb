# WCDB

**中文版本请参看[这里][wcdb-wiki]**

WCDB is an **efficient**, **complete**, **easy-to-use** mobile database framework used in WeChat application. It's currently available on iOS, macOS and Android.

# WCDB for iOS/macOS

## Features

* **WINQ**(WCDB language integrated query): WINQ is a native data querying capabilities which frees developers from writing glue codes to concatenate SQL query strings.
* **ORM**(Object Relational Mapping): WCDB provides a flexible, easy-to-use ORM for creating tables, indices and constraints, as well as CRUD through ObjC objects.
* **Multi-threaded concurrency**: WCDB supports concurrent read-read and read-write access via connection pooling.
* **Encryption Support**: WCDB supports database encryption via [SQLCipher][sqlcipher].
* **Corruption recovery**: WCDB provides a built-in repair kit for database corruption recovery.
* **Anti-injection**: WCDB provides a built-in protection from SQL injection.

## Getting Started

### Prerequisites

* Apps using WCDB can target: iOS 8 or later, macOS 10.10 or later.
* Xcode 8.0 or later required.
* Objective-C++ required.

### Installation

* **Via Carthage:** 
  1. Install [Carthage][install-carthage];
  2. Add `github "Tencent/WCDB"` to your Cartfile;
  3. Run `carthage update`.
  4. Drag `WCDB.framework` from the appropriate platform directory in `Carthage/Build/` to the `Linked Binary and Libraries` section of your Xcode project’s `Build Phases` settings;
  5. On your application targets' `Build Phases` settings tab, click the "+" icon and choose `New Run Script Phase`. Create a Run Script with  `carthage copy-frameworks` and add the paths to the frameworks under "Input Files": `$(SRCROOT)/Carthage/Build/iOS/WCDB.framework`;
* **Via source code**: 
  1. Getting source code from git repository. If cloning from git, submodules are required: `git submodule update --init --recursive`.
  2. Drag `WCDB.xcodeproj` in `wcdb/apple/` into your project;
  3. Add `WCDB.framework` to the `Target Dependencies` section and `Linked Binary and Libraries` of your Xcode project's `Build Phases` settings;
  4. Add `WCDB.framework` to the `Enbedded Binaries` section of your Xcode project's `General` settings;

Add `#import <WCDB/WCDB.h>` at the top of your Objective-C++ source files and start your WCDB journey.

## Tutorials

Tutorials can be found [here][iOS-tutorial].

## Documentations

* Documentations can be found on [our Wiki][wcdb-wiki].
* API references for iOS/macOS can be found [here][wcdb-docs-ios].

# WCDB for Android

## Features

* Database encryption via [SQLCipher][sqlcipher].
* Concurrent access via connection pooling from modern Android framework.
* Repair toolkit for database corruption recovery.
* Database backup and recovery utility optimized for small backup size.
* Log redirection and various tracing facilities.

## Getting Started

To include WCDB to your project, choose either way: import via Maven or via AAR package. 

### Import via Maven

To import WCDB via Maven repositories, add the following lines to `build.gradle` on your
app module: 

```gradle
dependencies {
    compile 'com.tencent.wcdb:wcdb-android:1.0.0'
    // Replace "1.0.0" to any available version.
}
```

This will cause Gradle to download AAR package from jcenter while building your application.

### Import Prebuilt AAR Package

  1. **Download AAR package from release page.**

  2. **Import the AAR as new module.** In Android Studio, select `File -> New -> New Module...` menu and choose `"Import JAR/AAR Package"`.

  3. **Add a dependency on the new module.** This can be done using `File -> Project Structure...` in Android Studio, or by adding following code to application's `build.gradle`:

```gradle
dependencies {
    // Change "wcdb" to the actual module name specified in step 2.
    compile project(':wcdb')
}
```

### Migrate from Plain-text SQLite Databases

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

See `sample-encryptdb` for sample for transferring data between plain-text and encrypted
databases.

### Corruption Recovery

See `sample-repairdb` for instructions how to recover corrupted databases using `RepairKit`.

### Redirect Log Output

By default, WCDB prints its log message to system logcat. You may want to change this
behavior in order to, for example, save logs for troubleshooting. WCDB can redirect
all of its log outputs to user-defined routine using `Log.setLogger(LogCallback)`
method.

## Build from Sources

### Build WCDB Android with Prebuilt Dependencies

WCDB itself can be built apart from its dependencies using Gradle or Android Studio. 
To build WCDB Android library, import Gradle project or run Gradle wrapper on 
`android` directory:

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

Android Studio will do this for you when the project is imported. If you have 
*"Failed to notify project evaluation listener"* error, double check your `ndk.dir`
property.

### Build Dependencies from Sources

WCDB depends on OpenSSL crypto library and SQLCipher. You can rebuild all dependencies
if you wish. In this case, a working C compiler on the host system, Perl 5, Tcl and a 
bash environment is needed to be installed on your system.

To build dependencies, checkout all submodules, set `ANDROID_NDK_ROOT` environment 
variable to your NDK path, then run `build-depends-android.sh`:

```bash
$ export ANDROID_NDK_ROOT=/path/to/ndk
$ ./build-depends-android.sh
```

This will build OpenSSL crypto library and generate SQLCipher amalgamation sources
and place them to proper locations suitable for WCDB library building.

## Documentations

* Documentations can be found on [our Wiki][wcdb-wiki].
* API references for Android can be found [here][wcdb-docs-android].

[install-carthage]: https://github.com/Carthage/Carthage#installing-carthage
[wcdb-wiki]: https://github.com/Tencent/wcdb/wiki
[wcdb-docs-ios]: https://tencent.github.io/wcdb/references/ios/index.html
[wcdb-docs-android]: https://tencent.github.io/wcdb/references/android/index.html
[sqlcipher]: https://github.com/sqlcipher/sqlcipher
[iOS-tutorial]: https://github.com/Tencent/wcdb/wiki/iOS+macOS%e4%bd%bf%e7%94%a8%e6%95%99%e7%a8%8b

