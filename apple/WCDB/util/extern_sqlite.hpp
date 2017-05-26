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

#ifndef extern_sqlite_hpp
#define extern_sqlite_hpp

extern "C" {

typedef struct sqlite3_stmt sqlite3_stmt;
typedef struct sqlite3 sqlite3;

typedef void (*sqlite3_destructor_type)(void*);
#ifndef SQLITE_TRANSIENT
#define SQLITE_TRANSIENT   ((sqlite3_destructor_type)-1)
#endif

#ifdef SQLITE_INT64_TYPE
typedef SQLITE_INT64_TYPE sqlite_int64;
typedef unsigned SQLITE_INT64_TYPE sqlite_uint64;
#elif defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64 sqlite_int64;
typedef unsigned __int64 sqlite_uint64;
#else
typedef long long int sqlite_int64;
typedef unsigned long long int sqlite_uint64;
#endif
typedef sqlite_int64 sqlite3_int64;

extern int sqlite3_bind_blob(sqlite3_stmt*, int, const void*, int n, void(*)(void*));
extern int sqlite3_bind_double(sqlite3_stmt*, int, double);
extern int sqlite3_bind_int(sqlite3_stmt*, int, int);
extern int sqlite3_bind_int64(sqlite3_stmt*, int, sqlite3_int64);
extern int sqlite3_bind_null(sqlite3_stmt*, int);
extern int sqlite3_bind_text(sqlite3_stmt*,int,const char*,int,void(*)(void*));

extern const void *sqlite3_column_blob(sqlite3_stmt*, int iCol);
extern int sqlite3_column_bytes(sqlite3_stmt*, int iCol);
extern double sqlite3_column_double(sqlite3_stmt*, int iCol);
extern int sqlite3_column_int(sqlite3_stmt*, int iCol);
extern sqlite3_int64 sqlite3_column_int64(sqlite3_stmt*, int iCol);
extern const unsigned char *sqlite3_column_text(sqlite3_stmt*, int iCol);
extern int sqlite3_column_type(sqlite3_stmt*, int iCol);

};

#endif /* extern_sqlite_hpp */
