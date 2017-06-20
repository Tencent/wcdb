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

#define LOG_TAG "WCDB.DBDumpUtil"

#include <assert.h>
#include <jni.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

#include "JNIHelp.h"
#include "Logger.h"
#include "ModuleLoader.h"

namespace wcdb {

struct previous_mode_data {
    int valid; /* Is there legit data in here? */
    int mode;
    int showHeader;
    int colWidth[100];
};

/*
    ** An pointer to an instance of this structure is passed from
    ** the main program to the callback.  This is used to communicate
    ** state and mode information.
    */
struct callback_data {
    sqlite3 *db;        /* The database */
    int echoOn;         /* True to echo input commands */
    int statsOn;        /* True to display memory stats before each finalize */
    int cnt;            /* Number of records displayed so far */
    FILE *out;          /* Write results here */
    FILE *traceOut;     /* Output for sqlite3_trace() */
    int nErr;           /* Number of errors seen */
    int mode;           /* An output mode setting */
    int writableSchema; /* True if PRAGMA writable_schema=ON */
    int showHeader;     /* True to show column names in List or Column mode */
    char *zDestTable;   /* Name of destination table when MODE_Insert */
    char separator[20]; /* Separator character for MODE_List */
    int colWidth[100];  /* Requested width of each column when in column mode*/
    int actualWidth[100]; /* Actual width of each column */
    char nullvalue[20];   /* The text to print when a NULL comes back from
                               ** the database */
    struct previous_mode_data explainPrev;
    /* Holds the mode information just before
        ** .explain ON */
    char outfile[FILENAME_MAX]; /* Filename for *out */
    const char *zDbFilename;    /* name of the database file */
    const char *zVfs;           /* Name of VFS to use */
    sqlite3_stmt *pStmt;        /* Current statement if any. */
    FILE *pLog;                 /* Write log output here */
};

/* zIn is either a pointer to a NULL-terminated string in memory obtained
    ** from malloc(), or a NULL pointer. The string pointed to by zAppend is
    ** added to zIn, and the result returned in memory obtained from malloc().
    ** zIn, if it was not NULL, is freed.
    **
    ** If the third argument, quote, is not '\0', then it is used as a
    ** quote character for zAppend.
    */
char *appendText(char *zIn, char const *zAppend, char quote)
{
    int len;
    int i;
    int nAppend = strlen(zAppend);
    int nIn = (zIn ? strlen(zIn) : 0);

    len = nAppend + nIn + 1;
    if (quote) {
        len += 2;
        for (i = 0; i < nAppend; i++) {
            if (zAppend[i] == quote)
                len++;
        }
    }

    zIn = (char *) realloc(zIn, len);
    if (!zIn)
        return 0;

    if (quote) {
        char *zCsr = &zIn[nIn];
        *zCsr++ = quote;
        for (i = 0; i < nAppend; i++) {
            *zCsr++ = zAppend[i];
            if (zAppend[i] == quote)
                *zCsr++ = quote;
        }
        *zCsr++ = quote;
        *zCsr++ = '\0';
        assert((zCsr - zIn) == len);
    } else {
        memcpy(&zIn[nIn], zAppend, nAppend);
        zIn[len - 1] = '\0';
    }

    return zIn;
}

/*
    ** Execute a query statement that will generate SQL output.  Print
    ** the result columns, comma-separated, on a line and then add a
    ** semicolon terminator to the end of that line.
    **
    ** If the number of columns is 1 and that column contains text "--"
    ** then write the semicolon on a separate line.  That way, if a
    ** "--" comment occurs at the end of the statement, the comment
    ** won't consume the semicolon terminator.
    */
int run_table_dump_query(
    struct callback_data *p, /* Query context */
    const char *zSelect,     /* SELECT statement to extract content */
    const char *zFirstRow    /* Print before first row, if not NULL */
    )
{
    sqlite3_stmt *pSelect;
    int rc;
    int nResult;
    int i;
    const char *z;
    rc = sqlite3_prepare(p->db, zSelect, -1, &pSelect, 0);
    if (rc != SQLITE_OK || !pSelect) {
        fprintf(p->out, "/**** ERROR: (%d) %s *****/\n", rc,
                sqlite3_errmsg(p->db));
        p->nErr++;
        return rc;
    }
    rc = sqlite3_step(pSelect);
    nResult = sqlite3_column_count(pSelect);
    while (rc == SQLITE_ROW) {
        if (zFirstRow) {
            fprintf(p->out, "%s", zFirstRow);
            zFirstRow = 0;
        }
        z = (const char *) sqlite3_column_text(pSelect, 0);
        fprintf(p->out, "%s", z);
        for (i = 1; i < nResult; i++) {
            fprintf(p->out, ",%s", sqlite3_column_text(pSelect, i));
        }
        if (z == 0)
            z = "";
        while (z[0] && (z[0] != '-' || z[1] != '-'))
            z++;
        if (z[0]) {
            fprintf(p->out, "\n;\n");
        } else {
            fprintf(p->out, ";\n");
        }
        rc = sqlite3_step(pSelect);
    }

    rc = sqlite3_finalize(pSelect);
    if (rc != SQLITE_OK) {
        LOGI(LOG_TAG, "/**** ERROR: (%d) %s *****/\n", rc,
             sqlite3_errmsg(p->db));
        p->nErr++;
    }
    return rc;
}

/*
    ** This is a different callback routine used for dumping the database.
    ** Each row received by this callback consists of a table name,
    ** the table type ("index" or "table") and SQL to create the table.
    ** This routine should print text sufficient to recreate the table.
    */
int dump_callback(void *pArg, int nArg, char **azArg, char **azCol)
{
    int rc;
    const char *zTable;
    const char *zType;
    const char *zSql;
    const char *zPrepStmt = 0;
    struct callback_data *p = (struct callback_data *) pArg;

    if (nArg != 3)
        return 1;
    zTable = azArg[0];
    zType = azArg[1];
    zSql = azArg[2];

    if (strcmp(zTable, "sqlite_sequence") == 0) {
        zPrepStmt = "DELETE FROM sqlite_sequence;\n";
    } else if (strcmp(zTable, "sqlite_stat1") == 0) {
        fprintf(p->out, "ANALYZE sqlite_master;\n");
    } else if (strncmp(zTable, "sqlite_", 7) == 0) {
        return 0;
    } else if (strncmp(zSql, "CREATE VIRTUAL TABLE", 20) == 0) {
        char *zIns;
        if (!p->writableSchema) {
            fprintf(p->out, "PRAGMA writable_schema=ON;\n");
            p->writableSchema = 1;
        }
        zIns = sqlite3_mprintf(
            "INSERT INTO sqlite_master(type,name,tbl_name,rootpage,sql)"
            "VALUES('table','%q','%q',0,'%q');",
            zTable, zTable, zSql);
        fprintf(p->out, "%s\n", zIns);
        sqlite3_free(zIns);
        return 0;
    } else {
        fprintf(p->out, "%s;\n", zSql);
    }
    if (strcmp(zType, "table") == 0) {
        sqlite3_stmt *pTableInfo = 0;
        char *zSelect = 0;
        char *zTableInfo = 0;
        char *zTmp = 0;
        int nRow = 0;

        zTableInfo = appendText(zTableInfo, "PRAGMA table_info(", 0);
        zTableInfo = appendText(zTableInfo, zTable, '"');
        zTableInfo = appendText(zTableInfo, ");", 0);

        rc = sqlite3_prepare(p->db, zTableInfo, -1, &pTableInfo, 0);
        free(zTableInfo);
        if (rc != SQLITE_OK || !pTableInfo) {
            return 1;
        }

        zSelect = appendText(zSelect, "SELECT 'INSERT INTO ' || ", 0);
        /* Always quote the table name, even if it appears to be pure ascii,
            ** in case it is a keyword. Ex:  INSERT INTO "table" ... */
        zTmp = appendText(zTmp, zTable, '"');
        if (zTmp) {
            zSelect = appendText(zSelect, zTmp, '\'');
            free(zTmp);
        }
        zSelect = appendText(zSelect, " || ' VALUES(' || ", 0);
        rc = sqlite3_step(pTableInfo);
        while (rc == SQLITE_ROW) {
            const char *zText =
                (const char *) sqlite3_column_text(pTableInfo, 1);
            zSelect = appendText(zSelect, "quote(", 0);
            zSelect = appendText(zSelect, zText, '"');
            rc = sqlite3_step(pTableInfo);
            if (rc == SQLITE_ROW) {
                zSelect = appendText(zSelect, "), ", 0);
            } else {
                zSelect = appendText(zSelect, ") ", 0);
            }
            nRow++;
        }
        rc = sqlite3_finalize(pTableInfo);
        if (rc != SQLITE_OK || nRow == 0) {
            free(zSelect);
            return 1;
        }
        zSelect = appendText(zSelect, "|| ')' FROM  ", 0);
        zSelect = appendText(zSelect, zTable, '"');

        rc = run_table_dump_query(p, zSelect, zPrepStmt);
        if (rc == SQLITE_CORRUPT) {
            zSelect = appendText(zSelect, " ORDER BY rowid DESC", 0);
            run_table_dump_query(p, zSelect, 0);
        }
        free(zSelect);
    }
    return 0;
}

int run_schema_dump_query(struct callback_data *p, const char *zQuery)
{
    int rc;
    char *zErr = 0;
    rc = sqlite3_exec(p->db, zQuery, dump_callback, p, &zErr);
    if (rc == SQLITE_CORRUPT) {
        char *zQ2;
        int len = strlen(zQuery);
        fprintf(p->out, "/****** ERROR: %s ******/\n", zErr);
        if (zErr) {
            LOGI(LOG_TAG, "/****** %s ******/\n", zErr);
            sqlite3_free(zErr);
            zErr = 0;
        }
        zQ2 = (char *) malloc(len + 100);
        if (zQ2 == 0)
            return rc;
        sqlite3_snprintf(len + 100, zQ2, "%s ORDER BY rowid DESC", zQuery);
        rc = sqlite3_exec(p->db, zQ2, dump_callback, p, &zErr);
        if (rc) {
            fprintf(p->out, "/****** ERROR: %s ******/\n", zErr);
        } else {
            rc = SQLITE_CORRUPT;
        }
        sqlite3_free(zErr);
        free(zQ2);
    }
    return rc;
}

/*
** Make sure the database is open.  If it is not, then open it.  If
** the database fails to open, print an error message and exit.
*/
int open_db(struct callback_data *p, const char *key)
{
    if (p->db == 0) {
        int errcode = 0;
        sqlite3_initialize();
        errcode = sqlite3_open(p->zDbFilename, &p->db);
        if (errcode != SQLITE_DONE && errcode != SQLITE_OK &&
            errcode != SQLITE_ROW) {
            LOGE(LOG_TAG, "Error: in sqlite3_open ");
            return -1;
        }
        if (key) {
            errcode = sqlite3_key(p->db, key, 7);
            if (errcode != SQLITE_DONE && errcode != SQLITE_OK &&
                errcode != SQLITE_ROW) {
                LOGE(LOG_TAG, "Error: setKey ");
                return -1;
            }
        }
        sqlite3 *db = p->db;
        if (db == 0 || SQLITE_OK != sqlite3_errcode(db)) {
            LOGE(LOG_TAG, "Error: unable to open database \"%s\": %s\n",
                 p->zDbFilename, sqlite3_errmsg(db));
            return -1;
        }
#ifndef SQLITE_OMIT_LOAD_EXTENSION
        sqlite3_enable_load_extension(p->db, 1);
#endif
    }
    return 0;
}

int run_command_db_dump(const char *dbPath,
                        const char *dbKey,
                        const char *outFile)
{
    struct callback_data data, *p;
    p = &data;

    //main_init(p);
    memset(&data, 0, sizeof(data));
    data.mode = 2;
    memcpy(data.separator, "|", 2);
    data.showHeader = 0;

    data.zDbFilename = dbPath;
    int errcode = open_db(p, dbKey);
    if (errcode < 0)
        return errcode;

    p->out = fopen(outFile, "w");
    fprintf(p->out, "PRAGMA foreign_keys=OFF;\n");
    fprintf(p->out, "BEGIN TRANSACTION;\n");
    p->writableSchema = 0;
    errcode = sqlite3_exec(p->db, "SAVEPOINT dump; PRAGMA writable_schema=ON",
                           0, 0, 0);
    if (errcode != SQLITE_DONE && errcode != SQLITE_OK &&
        errcode != SQLITE_ROW) {
        if (p->db)
            sqlite3_close(p->db);
        LOGE(LOG_TAG,
             "error in execute SAVEPOINT dump; PRAGMA writable_schema=ON");
        return -1;
    }
    p->nErr = 0;
    errcode = run_schema_dump_query(
        p, "SELECT name, type, sql FROM sqlite_master "
           "WHERE sql NOT NULL AND type=='table' AND name!='sqlite_sequence'");
    if (errcode != SQLITE_DONE && errcode != SQLITE_OK &&
        errcode != SQLITE_ROW) {
        if (p->db)
            sqlite3_close(p->db);
        LOGE(
            LOG_TAG, "error in run_schema_dump_query %s",
            "SELECT name, type, sql FROM sqlite_master "
            "WHERE sql NOT NULL AND type=='table' AND name!='sqlite_sequence'");
        return -1;
    }
    errcode =
        run_schema_dump_query(p, "SELECT name, type, sql FROM sqlite_master "
                                 "WHERE name=='sqlite_sequence'");
    if (errcode != SQLITE_DONE && errcode != SQLITE_OK &&
        errcode != SQLITE_ROW) {
        if (p->db)
            sqlite3_close(p->db);
        LOGE(LOG_TAG, "error in run_schema_dump_query %s",
             "SELECT name, type, sql FROM sqlite_master "
             "WHERE name=='sqlite_sequence'");
        return -1;
    }
    errcode = run_table_dump_query(
        p, "SELECT sql FROM sqlite_master "
           "WHERE sql NOT NULL AND type IN ('index','trigger','view')",
        0);
    if (errcode != SQLITE_DONE && errcode != SQLITE_OK &&
        errcode != SQLITE_ROW) {
        if (p->db)
            sqlite3_close(p->db);
        LOGE(LOG_TAG, "error in SELECT sql FROM sqlite_master WHERE sql NOT "
                      "NULL AND type IN ('index','trigger','view')");
        return -1;
    }
    if (p->writableSchema) {
        fprintf(p->out, "PRAGMA writable_schema=OFF;\n");
        p->writableSchema = 0;
    }
    sqlite3_exec(p->db, "PRAGMA writable_schema=OFF;", 0, 0, 0);
    sqlite3_exec(p->db, "RELEASE dump;", 0, 0, 0);
    fprintf(p->out, p->nErr ? "ROLLBACK; -- due to errors\n" : "COMMIT;\n");
    if (p->out) {
        fclose(p->out);
    }
    if (p->db)
        sqlite3_close(p->db);
    return 0;
}

static jboolean JNICALL nativeDumpDB(JNIEnv *env,
                                     jclass clazz,
                                     jstring dbPathStr,
                                     jstring keyStr,
                                     jstring outPathStr)
{
    const char *dbPath = env->GetStringUTFChars(dbPathStr, NULL);
    const char *key = NULL;
    if (keyStr)
        key = env->GetStringUTFChars(keyStr, NULL);
    const char *outPath = (env)->GetStringUTFChars(outPathStr, NULL);

    int rc = run_command_db_dump(dbPath, key, outPath);

    env->ReleaseStringUTFChars(dbPathStr, dbPath);
    if (key)
        env->ReleaseStringUTFChars(keyStr, key);
    env->ReleaseStringUTFChars(outPathStr, outPath);

    return (rc < 0) ? JNI_FALSE : JNI_TRUE;
}

static jboolean nativeIsSqlComplete(JNIEnv *env, jclass clazz, jstring sqlStr)
{
    const char *sql = env->GetStringUTFChars(sqlStr, NULL);
    int rc = sqlite3_complete(sql);
    env->ReleaseStringUTFChars(sqlStr, sql);

    return rc > 0 ? JNI_TRUE : JNI_FALSE;
}

static const JNINativeMethod sMethods[] = {
    {"nativeDumpDB",
     "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z",
     (void *) nativeDumpDB},
    {"nativeIsSqlComplete", "(Ljava/lang/String;)Z",
     (void *) nativeIsSqlComplete}};

static int register_wcdb_DBDumpUtil(JavaVM *vm, JNIEnv *env)
{
    return jniRegisterNativeMethods(env, "com/tencent/wcdb/repair/DBDumpUtil",
                                    sMethods, NELEM(sMethods));
}
WCDB_JNI_INIT(DBDumpUtil, register_wcdb_DBDumpUtil)
}
