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

#ifndef __WCDB_VFSLOG_H__
#define __WCDB_VFSLOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <sqlite3.h>
#include <stdint.h>

#define SQLITE_FCNTL_VFSLOG_STAT 10001

typedef enum VLogOp {
    VLOG_OP_CLOSE = 0,
    VLOG_OP_READ,
    VLOG_OP_CHNGCTR_READ,
    VLOG_OP_WRITE,
    VLOG_OP_CHNGCTR_WRITE,
    VLOG_OP_TRUNCATE,
    VLOG_OP_SYNC,
    VLOG_OP_FILESIZE,
    VLOG_OP_LOCK,
    VLOG_OP_UNLOCK,
    VLOG_OP_CHECKRESERVEDLOCK,
    VLOG_OP_FILECONTROL,
    VLOG_OP_SECTORSIZE,
    VLOG_OP_DEVCHAR,
    VLOG_OP_SHMMAP,
    VLOG_OP_SHMLOCK,
    VLOG_OP_SHMUNMAP,
    VLOG_OP_FETCH,
    VLOG_OP_UNFETCH,

    VLOG_OP_OPEN,
    VLOG_OP_DELETE,
    VLOG_OP_ACCESS,
} VLogOp;

extern volatile uint32_t vlogDefaultLogFlags;

typedef struct VLogStat {
    int64_t lastMainReadOffset;
    int64_t lastMainWriteOffset;
    int64_t lastJournalReadOffset;
    int64_t lastJournalWriteOffset;
} VLogStat;

int vlogGetStats(sqlite3 *db, const char *dbName, VLogStat *stats);

#ifdef __cplusplus
}
#endif

#endif