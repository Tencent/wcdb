//
// Created by qiuwenchen on 2022/9/15.
//

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

#include "CrossPlatform.h"

#if defined(__linux__) || defined(__ANDROID__)
#include <errno.h>
#include <unistd.h>

#if __GLIBC__ == 2 && __GLIBC_MINOR__ < 30
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)
#endif

int pthread_main_np()
{
    int pid = getpid();
    int tid = gettid();
    return pid == tid ? 1 : 0;
}

#ifdef __ANDROID__
#include <sys/sysconf.h>

int getdtablesize()
{
    return (int) sysconf(_SC_OPEN_MAX);
}
#endif /* __ANDROID__ */

#endif /* defined(__linux__) || defined(__ANDROID__) */
