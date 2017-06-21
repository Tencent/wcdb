# Keep all native methods, their classes and any classes in their descriptors
-keepclasseswithmembers,includedescriptorclasses class com.tencent.wcdb.** {
    native <methods>;
}

# Keep all exception classes
-keep class com.tencent.wcdb.**.*Exception

# Keep classes referenced in JNI code
-keep,includedescriptorclasses class com.tencent.wcdb.database.SQLiteCustomFunction { *; }
-keep class com.tencent.wcdb.database.SQLiteDebug$* { *; }
-keep class com.tencent.wcdb.database.SQLiteCipherSpec { <fields>; }
-keep interface com.tencent.wcdb.support.Log$* { *; }
