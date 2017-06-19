## v1.0.1

#### iOS

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
