/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2018 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <jni.h>
#include "JNIHelp.h"
#include "SQLiteCommon.h"
#include "fts/mm_fts.h"
#include "ModuleLoader.h"

SQLITE_EXTENSION_INIT3

namespace wcdb {

static void nativeInitialize(JNIEnv *env, jclass cls, jlong sqlite3Ptr, jlong apiPtr) {
    sqlite3 *db = (sqlite3 *)(intptr_t) sqlite3Ptr;
    const sqlite3_api_routines *api = (const sqlite3_api_routines *)(intptr_t) apiPtr;

    char *errMsg = NULL;
    int rc = sqlite3_mmftsext_init(db, &errMsg, api);
    if (rc != SQLITE_OK) {
        throw_sqlite3_exception(env, rc, errMsg, "Cannot initialize MMFtsTokenizer extension.");
        sqlite3_free(errMsg);
    }
}

static JNINativeMethod sMethods[] = {
        {"nativeInitialize", "(JJ)V", (void *) nativeInitialize},
};

static int register_extension_MMFtsTokenizer(JavaVM *vm, JNIEnv *env)
{
    return jniRegisterNativeMethods(env,
                                    "com/tencent/wcdb/extension/fts/MMFtsTokenizer",
                                    sMethods, NELEM(sMethods));
}
WCDB_JNI_INIT(MMFtsTokenizer, register_extension_MMFtsTokenizer)

}
