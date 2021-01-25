real_local_path := $(call my-dir)
root_path := $(call parent-dir, $(real_local_path))
prebuilt_path := $(root_path)/android/prebuilt/$(TARGET_ARCH)
build_info_path := $(NDK_OUT)

common_cflags := -Wall -Werror -Wno-unused-const-variable \
	-ffunction-sections -fdata-sections \
	-DSQLITE_HAS_CODEC -DSQLITE_CORE -DSQLITE_OS_UNIX
common_cppflags := -std=c++17 -fno-exceptions -fno-rtti
common_c_includes := \
	$(prebuilt_path)/include \
	$(root_path)/android/sqlcipher \
	$(root_path)/icucompat

# Main library
LOCAL_PATH := $(root_path)/android/jni
include $(CLEAR_VARS)
LOCAL_MODULE := wcdb
LOCAL_CFLAGS := $(common_cflags)
LOCAL_CPPFLAGS := $(common_cppflags)
LOCAL_C_INCLUDES := $(common_c_includes)
LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/*.c)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/*.cpp))

LOCAL_LDLIBS := -llog -lz -ldl -latomic
LOCAL_LDFLAGS := -Wl,--gc-sections -Wl,--version-script=$(root_path)/android/wcdb.map

LOCAL_STATIC_LIBRARIES := \
	wcdb-repair \
	wcdb-backup \
	wcdb-vfslog \
	wcdb-fts \
	wcdb-icucompat \
	wcdb-safe-pcache \
	sqlcipher \
	crypto-static

include $(BUILD_SHARED_LIBRARY)

# Repair
LOCAL_PATH := $(root_path)/repair
include $(CLEAR_VARS)
LOCAL_MODULE := wcdb-repair
LOCAL_CFLAGS := $(common_cflags)
LOCAL_CPPFLAGS := $(common_cppflags)
LOCAL_C_INCLUDES := $(common_c_includes)
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/*.c)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/*.cpp))
include $(BUILD_STATIC_LIBRARY)

# Backup
LOCAL_PATH := $(root_path)/backup
include $(CLEAR_VARS)
LOCAL_MODULE := wcdb-backup
LOCAL_CFLAGS := $(common_cflags)
LOCAL_CPPFLAGS := $(common_cppflags)
LOCAL_C_INCLUDES := $(common_c_includes)
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/*.c)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/*.cpp))
include $(BUILD_STATIC_LIBRARY)

# ICU compat
LOCAL_PATH := $(root_path)/icucompat
include $(CLEAR_VARS)
LOCAL_MODULE := wcdb-icucompat
LOCAL_CFLAGS := $(common_cflags)
LOCAL_CPPFLAGS := $(common_cppflags)
LOCAL_C_INCLUDES := $(common_c_includes)
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/*.c)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/*.cpp))
include $(BUILD_STATIC_LIBRARY)

# FTS
LOCAL_PATH := $(root_path)/fts
include $(CLEAR_VARS)
LOCAL_MODULE := wcdb-fts
LOCAL_CFLAGS := $(common_cflags)
LOCAL_CPPFLAGS := $(common_cppflags)
LOCAL_C_INCLUDES := $(common_c_includes)
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/*.c)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/*.cpp))
include $(BUILD_STATIC_LIBRARY)

# VFSLOG
LOCAL_PATH := $(root_path)/vfslog
include $(CLEAR_VARS)
LOCAL_MODULE := wcdb-vfslog
LOCAL_CFLAGS := $(common_cflags)
LOCAL_CPPFLAGS := $(common_cppflags)
LOCAL_C_INCLUDES := $(common_c_includes)
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/*.c)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/*.cpp))
include $(BUILD_STATIC_LIBRARY)

# Safe PCache
LOCAL_PATH := $(root_path)/android/jni/safe-pcache
include $(CLEAR_VARS)
LOCAL_MODULE := wcdb-safe-pcache
LOCAL_CFLAGS := $(common_cflags) \
	-DSQLITE_ENABLE_MEMORY_MANAGEMENT=1 \
	-DSQLITE_THREADSAFE=2

LOCAL_CPPFLAGS := $(common_cppflags)
LOCAL_C_INCLUDES := $(common_c_includes) $(root_path)/sqlcipher/src
LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/*.c)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/*.cpp))
LOCAL_STATIC_LIBRARIES := sqlcipher
include $(BUILD_STATIC_LIBRARY)

# SQLCipher
LOCAL_PATH := $(root_path)/android/sqlcipher
include $(CLEAR_VARS)
LOCAL_MODULE := sqlcipher
LOCAL_CFLAGS := $(common_cflags) -Wno-unused -Wno-missing-braces \
	-DSQLITE_PRIVATE= \
	-DSQLITE_ENABLE_MEMORY_MANAGEMENT=1 \
	-DHAVE_USLEEP=1 \
	-DHAVE_FDATASYNC=1 \
	-DSQLITE_HAVE_ISNAN \
	-DSQLITE_DEFAULT_FILE_FORMAT=4 \
	-DSQLITE_THREADSAFE=2 \
	-DSQLITE_TEMP_STORE=3 \
	-DSQLITE_ENABLE_FTS3 -DSQLITE_ENABLE_FTS4 \
	-DSQLITE_ENABLE_FTS5 -DSQLITE_ENABLE_JSON1 \
	-DSQLITE_ENABLE_SESSION -DSQLITE_ENABLE_PREUPDATE_HOOK \
	-DSQLITE_DEFAULT_WORKER_THREADS=2 \
  	-DSQLITE_DEFAULT_JOURNAL_SIZE_LIMIT=1048576 \
  	-DUSE_PREAD64=1 \
	-DSQLITE_ENABLE_FTS3_PARENTHESIS \
  	-DSQLITE_ENABLE_FTS3_TOKENIZER \
  	-DSQLITE_ENABLE_STAT4 \
  	-DSQLITE_ENABLE_EXPLAIN_COMMENTS \
	-DSQLITE_ENABLE_DBSTAT_VTAB \
	-DOMIT_MEMLOCK \
	-DOMIT_MEM_SECURITY \
	-DSQLCIPHER_CRYPTO_OPENSSL \
	-DSQLITE_MALLOC_SOFT_LIMIT=0

LOCAL_C_INCLUDES := $(common_c_includes)
LOCAL_SRC_FILES := sqlite3.c
LOCAL_STATIC_LIBRARIES := crypto-static
include $(BUILD_STATIC_LIBRARY)

# Prebuilt libcrypto
LOCAL_PATH := $(prebuilt_path)
include $(CLEAR_VARS)
LOCAL_MODULE := crypto-static
LOCAL_SRC_FILES := lib/libcrypto.a
include $(PREBUILT_STATIC_LIBRARY)
