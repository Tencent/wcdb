# Add project specific ProGuard rules here.
# You can control the set of applied configuration files using the
# proguardFiles setting in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
#-keepclassmembers class fqcn.of.javascript.interface.for.webview {
#   public *;
#}

# Uncomment this to preserve the line number information for
# debugging stack traces.
#-keepattributes SourceFile,LineNumberTable

# If you keep the line number information, uncomment this to
# hide the original source file name.
#-renamesourcefileattribute SourceFile

# Keep all native functions
-keepclasseswithmembers,includedescriptorclasses class com.tencent.wcdb.** {
    native <methods>;
}

# Keep all exception classes
-keep class com.tencent.wcdb.**.*Exception

# Keep all c callback functions
-keep,includedescriptorclasses class com.tencent.wcdb.core.Database {
    void onBusyTrace(com.tencent.wcdb.core.Database$BusyTracer, long, java.lang.String, long, java.lang.String);
    void onClose(com.tencent.wcdb.core.Database$CloseCallBack);
    boolean onConfig(long, com.tencent.wcdb.core.Database$Config);
    void onCorrupted(com.tencent.wcdb.core.Database$CorruptionNotification, long);
    void onEnumerateInfo(java.util.HashMap, java.lang.String, int, long, double, java.lang.String);
    void onTraceException(com.tencent.wcdb.core.Database$ExceptionTracer, long);
    void filterCompress(com.tencent.wcdb.core.Database$CompressionFilter, long, java.lang.String);
    boolean onProgressUpdate(com.tencent.wcdb.core.Database$ProgressMonitor, double, double);
    void onTableCompressed(com.tencent.wcdb.core.Database$CompressionNotification, long, java.lang.String);
    void onTableMigrated(com.tencent.wcdb.core.Database$MigrationNotification, long, java.lang.String, java.lang.String);
    void onTraceOperation(com.tencent.wcdb.core.Database$OperationTracer, long, int, long);
    void onTracePerformance(com.tencent.wcdb.core.Database$PerformanceTracer, long, java.lang.String, long, java.lang.String, long, int[]);
    void onTraceSQL(com.tencent.wcdb.core.Database$SQLTracer, long, java.lang.String, long, java.lang.String, java.lang.String);
    boolean checkTableShouldBeBackup(com.tencent.wcdb.core.Database$BackupFilter, java.lang.String);
}

-keep,includedescriptorclasses class com.tencent.wcdb.base.WCDBException {
    void addInfo(java.lang.String, int, long, double, java.lang.String);
}

-keep,includedescriptorclasses class com.tencent.wcdb.core.Handle {
    int onPausableTransaction(long, com.tencent.wcdb.core.PausableTransaction, boolean);
    boolean onTransaction(long, com.tencent.wcdb.core.Transaction);
}