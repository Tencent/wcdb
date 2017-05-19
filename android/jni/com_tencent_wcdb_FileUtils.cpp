/* //device/libs/android_runtime/android_util_Process.cpp
**
** Copyright 2006, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#define LOG_TAG "WCDB.FileUtils"

#include <jni.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

#include "JNIHelp.h"
#include "ModuleLoader.h"


namespace wcdb {

    jint android_os_FileUtils_setPermissions(JNIEnv* env, jobject clazz,
        jstring file, jint mode, jint uid, jint gid)
    {
        int ret = 0;
        if (!file) return ENOENT;

        const char *path = env->GetStringUTFChars(file, NULL);
        if (uid >= 0 || gid >= 0) {
            int res = chown(path, uid, gid);
            if (res != 0) {
                ret = errno;
                goto end;
            }
        }

        if (chmod(path, mode) != 0)
            ret = errno;

    end:
        env->ReleaseStringUTFChars(file, path);
        return ret;
    }

    static const JNINativeMethod methods[] = {
        {"setPermissions", "(Ljava/lang/String;III)I", (void*) android_os_FileUtils_setPermissions}
    };

    static int register_wcdb_FileUtils(JavaVM *vm, JNIEnv* env)
    {
        return jniRegisterNativeMethods(env, "com/tencent/wcdb/FileUtils", methods, NELEM(methods));
    }
    WCDB_JNI_INIT(FileUtils, register_wcdb_FileUtils)

}
