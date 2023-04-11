/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
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

#ifndef SQLITE3_WCDB_H
#define SQLITE3_WCDB_H

/*
 * Make sure we can call this stuff from C++.
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef SQLITE_WCDB

#ifdef SQLITE_WCDB_LOCK_HOOK
/*
 ** Register handlers when lock state changed.
 */
SQLITE_API int sqlite3_lock_hook(void (*xWillLock)(void *pArg, const char* zPath, int eLock),
                      void (*xLockDidChange)(void *pArg, const char* zPath, int eLock),
                      void (*xWillShmLock)(void *pArg, const char* zPath, int flags, int mask),
                      void (*xShmLockDidChange)(void *pArg, const char* zPath, void* id, int sharedMask, int exclMask),
                      void *pArg);
#endif //SQLITE_WCDB_LOCK_HOOK


#ifdef SQLITE_WCDB_CHECKPOINT_HANDLER
/*
 ** Register a handler when checkpoint did happen.
 */
SQLITE_API void *sqlite3_wal_checkpoint_handler(sqlite3 *, void (*xCallback)(void*, sqlite3*, const char *), void*);
#endif // SQLITE_WCDB_CHECKPOINT_HANDLER

#ifdef SQLITE_WCDB_SUSPEND
/*
 ** Suspend a handler to interrupt and prevent all operations.
 */
void sqlite3_suspend(sqlite3 *db, int suspend);

/*
 ** Ignore suspend and interrupt
 */
void sqlite3_unimpeded(sqlite3 *db, int unimpeded);
#endif

void sqlite3_revertCommitOrder(sqlite3 *db);

void* sqlite3_getCipherContext(sqlite3 *db, const char* schema);

int sqlcipher_codec_ctx_get_reservesize(void *ctx);

void* sqlite3Codec(void *iCtx, void *data, unsigned int pgno, int mode);

#endif // SQLITE_WCDB

#endif /* SQLITE3_WCDB_H */

#ifdef __cplusplus
}
#endif
