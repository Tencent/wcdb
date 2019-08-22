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

#ifndef sqliterk_h
#define sqliterk_h

typedef struct sqliterk sqliterk;
typedef struct sqliterk_cipher_conf sqliterk_cipher_conf;
typedef struct sqliterk_notify sqliterk_notify;

int sqliterkOpen(const char *path,
                 const sqliterk_cipher_conf *cipher,
                 sqliterk **rk);
int sqliterkParse(sqliterk *rk);
int sqliterkParsePage(sqliterk *rk, int pageno);
int sqliterkParseMaster(sqliterk *rk);
int sqliterkClose(sqliterk *rk);
int sqliterkSetNotify(sqliterk *rk, sqliterk_notify notify);
int sqliterkSetUserInfo(sqliterk *rk, void *userInfo);
void *sqliterkGetUserInfo(sqliterk *rk);

int sqliterkGetParsedPageCount(sqliterk *rk);
int sqliterkGetValidPageCount(sqliterk *rk);
int sqliterkGetPageCount(sqliterk *rk);
unsigned int sqliterkGetIntegrity(sqliterk *rk);

#endif /* sqliterk_h */
