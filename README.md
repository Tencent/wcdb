# WCDB

(中文版本请参看[这里](https://github.com/Tencent/wcdb/wiki))

WCDB is an **efficient**, **complete**, **easy-to-use** mobile database framework, which is based on [sqlcipher][sqlcipher]. This repository holds the source code for the iOS, macOS and Android.

# WCDB for iOS/macOS

## Features

* **WINQ**(WCDB language integrated query): WINQ is a native data querying capabilities which leads developers freed from the glue code for SQL string concatenation.
* **ORM**(Object Relational Mapping): WCDB supports a flexible, easy-to-use ORM for creating table, index and constraint and doing a CRUD through ObjC objects.
* **Multi-threaded concurrent**: WCDB supports concurrency for read/read and read/write, while write/write will execute serially.
* **Encryption**: WCDB supports encrption based on [sqlcipher][sqlcipher].
* **Corruption recovery**: WCDB provides a built-in repair kit for database corruption.
* **Anti-injection**: WCDB provides a built-in protection for SQL injection.
* ...

## Getting Started

### Prerequisites

* Apps using WCDB can target: iOS 8 or later, macOS 10.10 or later.
* Xcode 8.0 or later required.
* Objective-C++ required.

### Installation

* **Via Carthage:** 
  1. Install [Carthage](https://github.com/Carthage/Carthage#installing-carthage);
  2. Add `github "Tencent/WCDB"` to your Cartfile;
  3. Drag `WCDB.framework` from the appropriate platform directory in `Carthage/Build/` to the "Linked Binary and Libraries" section of your Xcode project’s "Build Phases" settings;
  4. iOS: On your application targets' "Build Phases" settings tab, click the "+" icon and choose "New Run Script Phase". Create a Run Script with  `carthage copy-frameworks` and add the paths to the frameworks under "Input Files": `$(SRCROOT)/Carthage/Build/WCDB.framework`;
* **Via source code**: 
  1. Getting source code
     * If cloning from git, recursive flag is required: `git clone https://github.com/Tencent/wcdb.git --recursive` ;
     * If already cloned from git, submodules are required: `git submodule update --init --recursive`;
  2. Drag `WCDB.xcodeproj` into your project;
  3. Add `WCDB.framework` to the "Target Dependencies" section and "Linked Binary and Libraries" of your Xcode project's "Build Phases" settings;
  4. Add `WCDB.framework` to the "Enbedded Binaries" section of your Xcode project's "General" settings;

Add `#import <WCDB/WCDB.h>` at the top of your Objective-C++ source files and start your WCDB journey.

### Tutorial

You can find tutorial at TODO.

### Documentation

* The documentation can be found at https://github.com/Tencent/wcdb/wiki .
* The API reference can be found at https://tencent.github.io/wcdb/references/ios/index.html .

# WCDB for Android

WCDB uses the same interface as Android SQLite Database API.

## Getting Started

To include WCDB to your project, choose either way: import prebuilt AAR package, or import via Maven.

### Import Prebuilt AAR Package

  1. **Download AAR package from release page.**

  2. **Import the AAR as new module.** In Android Studio, select `File -> New -> New Module...` menu and choose `"Import JAR/AAR Package"`.

  3. **Add a dependency on the new module.** This can be done using `File -> Project Structure...` in Android Studio, or by adding following code to application's `build.gradle`:

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
To build WCDB Android library, run Gradle on `android` directory:

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

[sqlcipher]: https://github.com/sqlcipher/sqlcipher
