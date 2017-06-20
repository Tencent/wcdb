/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#undef LOG_TAG
#define LOG_TAG "WCDB.CursorWindow"

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include "ModuleLoader.h"

#include <inttypes.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>

#include "CursorWindow.h"
#include "Errors.h"
#include "JNIHelp.h"
#include "Logger.h"
#include "SQLiteCommon.h"
#include "Unicode.h"

namespace wcdb {

static struct {
    jfieldID data;
    jfieldID sizeCopied;
} gCharArrayBufferClassInfo;

static void throwExceptionWithRowCol(JNIEnv *env, jint row, jint column)
{
    char buf[256];
    snprintf(buf, sizeof(buf),
             "Couldn't read row %d, col %d from CursorWindow.  "
             "Make sure the Cursor is initialized correctly before accessing "
             "data from it.",
             row, column);
    jniThrowException(env, "java/lang/IllegalStateException", buf);
}

static void throwUnknownTypeException(JNIEnv *env, jint type)
{
    char buf[128];
    snprintf(buf, sizeof(buf), "UNKNOWN type %d", type);
    jniThrowException(env, "java/lang/IllegalStateException", buf);
}

static jlong
nativeCreate(JNIEnv *env, jclass clazz, jstring nameObj, jint cursorWindowSize)
{
    CursorWindow *window;
    status_t status = CursorWindow::create(cursorWindowSize, &window);
    if (status || !window) {
        LOGE(LOG_TAG,
             "Could not allocate CursorWindow of size %d due to error %d.",
             cursorWindowSize, status);
        return 0;
    }

    //LOGV(LOG_TAG,"nativeInitializeEmpty: window = %p", window);
    return (jlong)(intptr_t) window;
}

static void nativeDispose(JNIEnv *env, jclass clazz, jlong windowPtr)
{
    CursorWindow *window = (CursorWindow *) (intptr_t) windowPtr;
    if (window) {
        //LOGV(LOG_TAG,"Closing window %p", window);
        delete window;
    }
}

static void nativeClear(JNIEnv *env, jclass clazz, jlong windowPtr)
{
    CursorWindow *window = (CursorWindow *) (intptr_t) windowPtr;
    LOG_WINDOW("Clearing window %p", window);
    status_t status = window->clear();
    if (status) {
        LOGW(LOG_TAG, "Could not clear window. error=%d", status);
    }
}

static jint nativeGetNumRows(JNIEnv *env, jclass clazz, jlong windowPtr)
{
    CursorWindow *window = (CursorWindow *) (intptr_t) windowPtr;
    return window->getNumRows();
}

static jboolean
nativeSetNumColumns(JNIEnv *env, jclass clazz, jlong windowPtr, jint columnNum)
{
    CursorWindow *window = (CursorWindow *) (intptr_t) windowPtr;
    status_t status = window->setNumColumns(columnNum);
    return status == OK;
}

static jboolean nativeAllocRow(JNIEnv *env, jclass clazz, jlong windowPtr)
{
    CursorWindow *window = (CursorWindow *) (intptr_t) windowPtr;
    status_t status = window->allocRow(NULL);
    return status == OK;
}

static void nativeFreeLastRow(JNIEnv *env, jclass clazz, jlong windowPtr)
{
    CursorWindow *window = (CursorWindow *) (intptr_t) windowPtr;
    window->freeLastRow();
}

static jint
nativeGetType(JNIEnv *env, jclass clazz, jlong windowPtr, jint row, jint column)
{
    CursorWindow *window = (CursorWindow *) (intptr_t) windowPtr;
    LOG_WINDOW("returning column type affinity for %d,%d from %p", row, column,
               window);

    CursorWindow::FieldSlot *fieldSlot = window->getFieldSlot(row, column);
    if (!fieldSlot) {
        throwExceptionWithRowCol(env, row, column);
        return CursorWindow::FIELD_TYPE_NULL;
    }
    return window->getFieldSlotType(fieldSlot);
}

static jbyteArray
nativeGetBlob(JNIEnv *env, jclass clazz, jlong windowPtr, jint row, jint column)
{
    CursorWindow *window = (CursorWindow *) (intptr_t) windowPtr;
    LOG_WINDOW("Getting blob for %d,%d from %p", row, column, window);

    CursorWindow::FieldSlot *fieldSlot = window->getFieldSlot(row, column);
    if (!fieldSlot) {
        throwExceptionWithRowCol(env, row, column);
        return NULL;
    }

    int32_t type = window->getFieldSlotType(fieldSlot);
    if (type == CursorWindow::FIELD_TYPE_BLOB ||
        type == CursorWindow::FIELD_TYPE_STRING) {
        size_t size;
        const void *value = window->getFieldSlotValueBlob(fieldSlot, &size);
        jbyteArray byteArray = env->NewByteArray(size);
        if (!byteArray) {
            env->ExceptionClear();
            throw_sqlite3_exception(env, "Native could not create new byte[]");
            return NULL;
        }
        env->SetByteArrayRegion(byteArray, 0, size,
                                static_cast<const jbyte *>(value));
        return byteArray;
    } else if (type == CursorWindow::FIELD_TYPE_INTEGER) {
        throw_sqlite3_exception(env, "INTEGER data in nativeGetBlob ");
    } else if (type == CursorWindow::FIELD_TYPE_FLOAT) {
        throw_sqlite3_exception(env, "FLOAT data in nativeGetBlob ");
    } else if (type == CursorWindow::FIELD_TYPE_NULL) {
        // do nothing
    } else {
        throwUnknownTypeException(env, type);
    }
    return NULL;
}

static jstring nativeGetString(
    JNIEnv *env, jclass clazz, jlong windowPtr, jint row, jint column)
{
    CursorWindow *window = (CursorWindow *) (intptr_t) windowPtr;
    LOG_WINDOW("Getting string for %d,%d from %p", row, column, window);

    CursorWindow::FieldSlot *fieldSlot = window->getFieldSlot(row, column);
    if (!fieldSlot) {
        throwExceptionWithRowCol(env, row, column);
        return NULL;
    }

    int32_t type = window->getFieldSlotType(fieldSlot);
    if (type == CursorWindow::FIELD_TYPE_STRING) {
        size_t sizeIncludingNull;
        const char *value =
            window->getFieldSlotValueString(fieldSlot, &sizeIncludingNull);
        if (sizeIncludingNull <= 1) {
            return env->NewStringUTF("");
        }
        // Convert to UTF-16 here instead of calling NewStringUTF.  NewStringUTF
        // doesn't like UTF-8 strings with high codepoints.  It actually expects
        // Modified UTF-8 with encoded surrogate pairs.
        ssize_t u16_size = utf8_to_utf16_length((const uint8_t *) value,
                                                sizeIncludingNull - 1);
        if (u16_size < 0) {
            // Invalid UTF-16 detected, return empty string.
            return env->NewStringUTF("");
        }
        uint16_t *u16_buf = new uint16_t[u16_size + 1];
        utf8_to_utf16((const uint8_t *) value, sizeIncludingNull - 1, u16_buf);
        jstring ret = env->NewString(u16_buf, u16_size);
        delete[] u16_buf;
        return ret;
    } else if (type == CursorWindow::FIELD_TYPE_INTEGER) {
        int64_t value = window->getFieldSlotValueLong(fieldSlot);
        char buf[32];
        snprintf(buf, sizeof(buf), "%" PRId64, value);
        return env->NewStringUTF(buf);
    } else if (type == CursorWindow::FIELD_TYPE_FLOAT) {
        double value = window->getFieldSlotValueDouble(fieldSlot);
        char buf[32];
        snprintf(buf, sizeof(buf), "%g", value);
        return env->NewStringUTF(buf);
    } else if (type == CursorWindow::FIELD_TYPE_NULL) {
        return NULL;
    } else if (type == CursorWindow::FIELD_TYPE_BLOB) {
        throw_sqlite3_exception(env, "Unable to convert BLOB to string");
        return NULL;
    } else {
        throwUnknownTypeException(env, type);
        return NULL;
    }
}

static jcharArray
allocCharArrayBuffer(JNIEnv *env, jobject bufferObj, size_t size)
{
    jcharArray dataObj = jcharArray(
        env->GetObjectField(bufferObj, gCharArrayBufferClassInfo.data));
    if (dataObj && size) {
        jsize capacity = env->GetArrayLength(dataObj);
        if (size_t(capacity) < size) {
            env->DeleteLocalRef(dataObj);
            dataObj = NULL;
        }
    }
    if (!dataObj) {
        jsize capacity = size;
        if (capacity < 64) {
            capacity = 64;
        }
        dataObj = env->NewCharArray(capacity); // might throw OOM
        if (dataObj) {
            env->SetObjectField(bufferObj, gCharArrayBufferClassInfo.data,
                                dataObj);
        }
    }
    return dataObj;
}

static void fillCharArrayBufferUTF(JNIEnv *env,
                                   jobject bufferObj,
                                   const char *str,
                                   size_t len)
{
    ssize_t size =
        utf8_to_utf16_length(reinterpret_cast<const uint8_t *>(str), len);
    if (size < 0) {
        size = 0; // invalid UTF8 string
    }
    jcharArray dataObj = allocCharArrayBuffer(env, bufferObj, size);
    if (dataObj) {
        if (size) {
            jchar *data = static_cast<jchar *>(
                env->GetPrimitiveArrayCritical(dataObj, NULL));
            utf8_to_utf16_no_null_terminator(
                reinterpret_cast<const uint8_t *>(str), len,
                reinterpret_cast<uint16_t *>(data));
            env->ReleasePrimitiveArrayCritical(dataObj, data, 0);
        }
        env->SetIntField(bufferObj, gCharArrayBufferClassInfo.sizeCopied, size);
    }
}

static void clearCharArrayBuffer(JNIEnv *env, jobject bufferObj)
{
    jcharArray dataObj = allocCharArrayBuffer(env, bufferObj, 0);
    if (dataObj) {
        env->SetIntField(bufferObj, gCharArrayBufferClassInfo.sizeCopied, 0);
    }
}

static void nativeCopyStringToBuffer(JNIEnv *env,
                                     jclass clazz,
                                     jlong windowPtr,
                                     jint row,
                                     jint column,
                                     jobject bufferObj)
{
    CursorWindow *window = (CursorWindow *) (intptr_t) windowPtr;
    LOG_WINDOW("Copying string for %d,%d from %p", row, column, window);

    CursorWindow::FieldSlot *fieldSlot = window->getFieldSlot(row, column);
    if (!fieldSlot) {
        throwExceptionWithRowCol(env, row, column);
        return;
    }

    int32_t type = window->getFieldSlotType(fieldSlot);
    if (type == CursorWindow::FIELD_TYPE_STRING) {
        size_t sizeIncludingNull;
        const char *value =
            window->getFieldSlotValueString(fieldSlot, &sizeIncludingNull);
        if (sizeIncludingNull > 1) {
            fillCharArrayBufferUTF(env, bufferObj, value,
                                   sizeIncludingNull - 1);
        } else {
            clearCharArrayBuffer(env, bufferObj);
        }
    } else if (type == CursorWindow::FIELD_TYPE_INTEGER) {
        int64_t value = window->getFieldSlotValueLong(fieldSlot);
        char buf[32];
        snprintf(buf, sizeof(buf), "%" PRId64, value);
        fillCharArrayBufferUTF(env, bufferObj, buf, strlen(buf));
    } else if (type == CursorWindow::FIELD_TYPE_FLOAT) {
        double value = window->getFieldSlotValueDouble(fieldSlot);
        char buf[32];
        snprintf(buf, sizeof(buf), "%g", value);
        fillCharArrayBufferUTF(env, bufferObj, buf, strlen(buf));
    } else if (type == CursorWindow::FIELD_TYPE_NULL) {
        clearCharArrayBuffer(env, bufferObj);
    } else if (type == CursorWindow::FIELD_TYPE_BLOB) {
        throw_sqlite3_exception(env, "Unable to convert BLOB to string");
    } else {
        throwUnknownTypeException(env, type);
    }
}

static jlong
nativeGetLong(JNIEnv *env, jclass clazz, jlong windowPtr, jint row, jint column)
{
    CursorWindow *window = (CursorWindow *) (intptr_t) windowPtr;
    LOG_WINDOW("Getting long for %d,%d from %p", row, column, window);

    CursorWindow::FieldSlot *fieldSlot = window->getFieldSlot(row, column);
    if (!fieldSlot) {
        throwExceptionWithRowCol(env, row, column);
        return 0;
    }

    int32_t type = window->getFieldSlotType(fieldSlot);
    if (type == CursorWindow::FIELD_TYPE_INTEGER) {
        return window->getFieldSlotValueLong(fieldSlot);
    } else if (type == CursorWindow::FIELD_TYPE_STRING) {
        size_t sizeIncludingNull;
        const char *value =
            window->getFieldSlotValueString(fieldSlot, &sizeIncludingNull);
        return sizeIncludingNull > 1 ? strtoll(value, NULL, 0) : 0L;
    } else if (type == CursorWindow::FIELD_TYPE_FLOAT) {
        return jlong(window->getFieldSlotValueDouble(fieldSlot));
    } else if (type == CursorWindow::FIELD_TYPE_NULL) {
        return 0;
    } else if (type == CursorWindow::FIELD_TYPE_BLOB) {
        throw_sqlite3_exception(env, "Unable to convert BLOB to long");
        return 0;
    } else {
        throwUnknownTypeException(env, type);
        return 0;
    }
}

static jdouble nativeGetDouble(
    JNIEnv *env, jclass clazz, jlong windowPtr, jint row, jint column)
{
    CursorWindow *window = (CursorWindow *) (intptr_t) windowPtr;
    LOG_WINDOW("Getting double for %d,%d from %p", row, column, window);

    CursorWindow::FieldSlot *fieldSlot = window->getFieldSlot(row, column);
    if (!fieldSlot) {
        throwExceptionWithRowCol(env, row, column);
        return 0.0;
    }

    int32_t type = window->getFieldSlotType(fieldSlot);
    if (type == CursorWindow::FIELD_TYPE_FLOAT) {
        return window->getFieldSlotValueDouble(fieldSlot);
    } else if (type == CursorWindow::FIELD_TYPE_STRING) {
        size_t sizeIncludingNull;
        const char *value =
            window->getFieldSlotValueString(fieldSlot, &sizeIncludingNull);
        return sizeIncludingNull > 1 ? strtod(value, NULL) : 0.0;
    } else if (type == CursorWindow::FIELD_TYPE_INTEGER) {
        return jdouble(window->getFieldSlotValueLong(fieldSlot));
    } else if (type == CursorWindow::FIELD_TYPE_NULL) {
        return 0.0;
    } else if (type == CursorWindow::FIELD_TYPE_BLOB) {
        throw_sqlite3_exception(env, "Unable to convert BLOB to double");
        return 0.0;
    } else {
        throwUnknownTypeException(env, type);
        return 0.0;
    }
}

static jboolean nativePutBlob(JNIEnv *env,
                              jclass clazz,
                              jlong windowPtr,
                              jbyteArray valueObj,
                              jint row,
                              jint column)
{
    CursorWindow *window = (CursorWindow *) (intptr_t) windowPtr;
    jsize len = env->GetArrayLength(valueObj);

    void *value = env->GetPrimitiveArrayCritical(valueObj, NULL);
    status_t status = window->putBlob(row, column, value, len);
    env->ReleasePrimitiveArrayCritical(valueObj, value, JNI_ABORT);

    if (status) {
        LOGW(LOG_TAG, "Failed to put blob. error=%d", status);
        return false;
    }

    LOG_WINDOW("%d,%d is BLOB with %u bytes", row, column, len);
    return true;
}

static jboolean nativePutString(JNIEnv *env,
                                jclass clazz,
                                jlong windowPtr,
                                jstring valueObj,
                                jint row,
                                jint column)
{
    CursorWindow *window = (CursorWindow *) (intptr_t) windowPtr;

    size_t sizeIncludingNull = env->GetStringUTFLength(valueObj) + 1;
    const char *valueStr = env->GetStringUTFChars(valueObj, NULL);
    if (!valueStr) {
        LOG_WINDOW("value can't be transferred to UTFChars");
        return false;
    }
    status_t status =
        window->putString(row, column, valueStr, sizeIncludingNull);
    env->ReleaseStringUTFChars(valueObj, valueStr);

    if (status) {
        LOG_WINDOW("Failed to put string. error=%d", status);
        return false;
    }

    LOG_WINDOW("%d,%d is TEXT with %zu bytes", row, column, sizeIncludingNull);
    return true;
}

static jboolean nativePutLong(JNIEnv *env,
                              jclass clazz,
                              jlong windowPtr,
                              jlong value,
                              jint row,
                              jint column)
{
    CursorWindow *window = (CursorWindow *) (intptr_t) windowPtr;
    status_t status = window->putLong(row, column, value);

    if (status) {
        LOG_WINDOW("Failed to put long. error=%d", status);
        return false;
    }

    LOG_WINDOW("%d,%d is INTEGER 0x%016" PRIx64, row, column, (uint64_t) value);
    return true;
}

static jboolean nativePutDouble(JNIEnv *env,
                                jclass clazz,
                                jlong windowPtr,
                                jdouble value,
                                jint row,
                                jint column)
{
    CursorWindow *window = (CursorWindow *) (intptr_t) windowPtr;
    status_t status = window->putDouble(row, column, value);

    if (status) {
        LOG_WINDOW("Failed to put double. error=%d", status);
        return false;
    }

    LOG_WINDOW("%d,%d is FLOAT %lf", row, column, value);
    return true;
}

static jboolean
nativePutNull(JNIEnv *env, jclass clazz, jlong windowPtr, jint row, jint column)
{
    CursorWindow *window = (CursorWindow *) (intptr_t) windowPtr;
    status_t status = window->putNull(row, column);

    if (status) {
        LOG_WINDOW("Failed to put null. error=%d", status);
        return false;
    }

    LOG_WINDOW("%d,%d is NULL", row, column);
    return true;
}

static const JNINativeMethod sMethods[] = {
    /* name, signature, funcPtr */
    {"nativeCreate", "(Ljava/lang/String;I)J", (void *) nativeCreate},
    {"nativeDispose", "(J)V", (void *) nativeDispose},
    {"nativeClear", "(J)V", (void *) nativeClear},
    {"nativeGetNumRows", "(J)I", (void *) nativeGetNumRows},
    {"nativeSetNumColumns", "(JI)Z", (void *) nativeSetNumColumns},
    {"nativeAllocRow", "(J)Z", (void *) nativeAllocRow},
    {"nativeFreeLastRow", "(J)V", (void *) nativeFreeLastRow},
    {"nativeGetType", "(JII)I", (void *) nativeGetType},
    {"nativeGetBlob", "(JII)[B", (void *) nativeGetBlob},
    {"nativeGetString", "(JII)Ljava/lang/String;", (void *) nativeGetString},
    {"nativeGetLong", "(JII)J", (void *) nativeGetLong},
    {"nativeGetDouble", "(JII)D", (void *) nativeGetDouble},
    {"nativeCopyStringToBuffer", "(JIILandroid/database/CharArrayBuffer;)V",
     (void *) nativeCopyStringToBuffer},
    {"nativePutBlob", "(J[BII)Z", (void *) nativePutBlob},
    {"nativePutString", "(JLjava/lang/String;II)Z", (void *) nativePutString},
    {"nativePutLong", "(JJII)Z", (void *) nativePutLong},
    {"nativePutDouble", "(JDII)Z", (void *) nativePutDouble},
    {"nativePutNull", "(JII)Z", (void *) nativePutNull},
};

#define FIND_CLASS(var, className)                                             \
    var = env->FindClass(className);                                           \
    LOG_FATAL_IF(!var, "Unable to find class " className);

#define GET_FIELD_ID(var, clazz, fieldName, fieldDescriptor)                   \
    var = env->GetFieldID(clazz, fieldName, fieldDescriptor);                  \
    LOG_FATAL_IF(!var, "Unable to find field " fieldName);

static int register_wcdb_CursorWindow(JavaVM *vm, JNIEnv *env)
{
    jclass clazz;
    FIND_CLASS(clazz, "android/database/CharArrayBuffer");

    GET_FIELD_ID(gCharArrayBufferClassInfo.data, clazz, "data", "[C");
    GET_FIELD_ID(gCharArrayBufferClassInfo.sizeCopied, clazz, "sizeCopied",
                 "I");

    return jniRegisterNativeMethods(env, "com/tencent/wcdb/CursorWindow",
                                    sMethods, NELEM(sMethods));
}
WCDB_JNI_INIT(CursorWindow, register_wcdb_CursorWindow)

} // namespace wcdb
