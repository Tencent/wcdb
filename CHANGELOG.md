## v1.0.1

#### iOS

* Add `cocoapods` support.
* Add iOS 7 and macOS 10.9 support. Apps using WCDB can target to iOS 7 now.
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

* Various bug fixes.

## v1.0.0

Initial release.
