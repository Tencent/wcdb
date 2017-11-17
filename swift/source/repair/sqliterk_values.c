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

#include "sqliterk_values.h"
#include "SQLiteRepairKit.h"
#include "sqliterk_os.h"
#include "sqliterk_util.h"
#include <stdlib.h>
#include <string.h>

//declaration
static int sqliterkValuesAutoGrow(sqliterk_values *values);

typedef int64_t sqliterk_integer;
typedef double sqliterk_number;
typedef struct sqliterk_text sqliterk_text;
struct sqliterk_text {
    char *t;
    int s;
};
typedef struct sqliterk_binary sqliterk_binary;
struct sqliterk_binary {
    void *b;
    int s;
};
typedef union sqliterk_any sqliterk_any;
union sqliterk_any {
    sqliterk_integer *integer;
    sqliterk_number *number;
    sqliterk_text *text;
    sqliterk_binary *binary;
    void *memory;
};

struct sqliterk_value {
    sqliterk_value_type type;
    sqliterk_any any;
};

struct sqliterk_values {
    int count;
    int capacity;
    sqliterk_value *values;
};

int sqliterkValuesAlloc(sqliterk_values **values)
{
    if (!values) {
        return SQLITERK_MISUSE;
    }
    int rc = SQLITERK_OK;
    sqliterk_values *theValues = sqliterkOSMalloc(sizeof(sqliterk_values));
    if (!theValues) {
        rc = SQLITERK_NOMEM;
        goto sqliterkValuesAlloc_Failed;
    }
    rc = sqliterkValuesAutoGrow(theValues);
    if (rc != SQLITERK_OK) {
        goto sqliterkValuesAlloc_Failed;
    }
    *values = theValues;
    return SQLITERK_OK;
sqliterkValuesAlloc_Failed:
    if (theValues) {
        sqliterkValuesFree(theValues);
    }
    return rc;
}

int sqliterkValuesFree(sqliterk_values *values)
{
    if (!values) {
        return SQLITERK_MISUSE;
    }

    int i;
    for (i = 0; i < values->count; i++) {
        sqliterk_value *value = &values->values[i];
        sqliterkValueClear(value);
    }
    values->count = 0;
    if (values->values) {
        sqliterkOSFree(values->values);
        values->values = NULL;
    }
    values->capacity = 0;
    sqliterkOSFree(values);
    return SQLITERK_OK;
}

int sqliterkValuesClear(sqliterk_values *values)
{
    if (!values) {
        return SQLITERK_MISUSE;
    }

    int i;
    for (i = 0; i < values->count; i++) {
        sqliterk_value *value = &values->values[i];
        sqliterkValueClear(value);
    }
    values->count = 0;
    return SQLITERK_OK;
}

static int sqliterkValuesAutoGrow(sqliterk_values *values)
{
    if (!values) {
        return SQLITERK_MISUSE;
    }
    if (values->count >= values->capacity) {
        int oldCapacity = values->capacity;
        if (oldCapacity <= 0) {
            //init for 4
            values->capacity = 4;
        } else {
            values->capacity = oldCapacity * 2;
        }
        sqliterk_value *newValues =
            sqliterkOSMalloc(sizeof(sqliterk_value) * (values->capacity + 1));
        if (!newValues) {
            return SQLITERK_NOMEM;
        }
        if (values->values) {
            memcpy(newValues, values->values,
                   sizeof(sqliterk_value) * oldCapacity);
            sqliterkOSFree(values->values);
        }
        values->values = newValues;
    }
    return SQLITERK_OK;
}

int sqliterkValuesGetCount(sqliterk_values *values)
{
    if (!values) {
        return 0;
    }
    return values->count;
}

sqliterk_value_type sqliterkValuesGetType(sqliterk_values *values, int index)
{
    if (values && index < sqliterkValuesGetCount(values)) {
        return values->values[index].type;
    }
    return sqliterk_value_type_null;
}

int64_t sqliterkValuesGetInteger64(sqliterk_values *values, int index)
{
    int64_t out = 0;
    if (values && index < sqliterkValuesGetCount(values)) {
        sqliterk_value *value = &values->values[index];
        switch (sqliterkValuesGetType(values, index)) {
            case sqliterk_value_type_integer:
                out = (int64_t)(*value->any.integer);
                break;
            case sqliterk_value_type_number:
                out = (int64_t)(*value->any.number);
                break;
            case sqliterk_value_type_text:
                out = atol(value->any.text->t);
                break;
            default:
                break;
        }
    }
    return out;
}

int sqliterkValuesGetInteger(sqliterk_values *values, int index)
{
    return (int) sqliterkValuesGetInteger64(values, index);
}

double sqliterkValuesGetNumber(sqliterk_values *values, int index)
{
    double out = 0;
    if (values && index < sqliterkValuesGetCount(values)) {
        sqliterk_value *value = &values->values[index];
        switch (sqliterkValuesGetType(values, index)) {
            case sqliterk_value_type_integer:
                out = (double) (*value->any.integer);
                break;
            case sqliterk_value_type_number:
                out = (double) (*value->any.number);
                break;
            case sqliterk_value_type_text:
                out = atof(value->any.text->t);
                break;
            default:
                break;
        }
    }
    return out;
}

const char *sqliterkValuesGetText(sqliterk_values *values, int index)
{
    char *out = NULL;
    if (values && index < sqliterkValuesGetCount(values)) {
        sqliterk_value *value = &values->values[index];
        switch (value->type) {
            case sqliterk_value_type_text:
                out = value->any.text->t;
                break;
            default:
                break;
        }
    }
    return out;
}

const void *sqliterkValuesGetBinary(sqliterk_values *values, int index)
{
    void *out = NULL;
    if (values && index < sqliterkValuesGetCount(values)) {
        sqliterk_value *value = &values->values[index];
        switch (value->type) {
            case sqliterk_value_type_binary:
                out = value->any.binary->b;
                break;
            default:
                break;
        }
    }
    return out;
}

int sqliterkValuesGetBytes(sqliterk_values *values, int index)
{
    int out = 0;
    if (values && index < sqliterkValuesGetCount(values)) {
        sqliterk_value *value = &values->values[index];
        switch (value->type) {
            case sqliterk_value_type_binary:
                out = value->any.binary->s;
                break;
            case sqliterk_value_type_text:
                out = value->any.text->s;
                break;
            default:
                break;
        }
    }
    return out;
}

int sqliterkValuesAddInteger64(sqliterk_values *values, int64_t i)
{
    if (!values) {
        return SQLITERK_MISUSE;
    }
    int rc = sqliterkValuesAutoGrow(values);
    if (rc != SQLITERK_OK) {
        return rc;
    }
    sqliterk_value *value = &values->values[values->count];
    value->type = sqliterk_value_type_integer;
    value->any.integer = sqliterkOSMalloc(sizeof(sqliterk_integer));
    if (!value->any.integer) {
        rc = SQLITERK_NOMEM;
        goto sqliterkValuesAddInteger64_Failed;
    }
    *value->any.integer = i;
    values->count++;
    return SQLITERK_OK;

sqliterkValuesAddInteger64_Failed:
    sqliterkValueClear(value);
    return rc;
}

int sqliterkValuesAddInteger(sqliterk_values *values, int i)
{
    return sqliterkValuesAddInteger64(values, i);
}

int sqliterkValuesAddNumber(sqliterk_values *values, double d)
{
    if (!values) {
        return SQLITERK_MISUSE;
    }
    int rc = sqliterkValuesAutoGrow(values);
    if (rc != SQLITERK_OK) {
        return rc;
    }
    sqliterk_value *value = &values->values[values->count];
    value->type = sqliterk_value_type_number;
    value->any.number = sqliterkOSMalloc(sizeof(sqliterk_number));
    if (!value->any.number) {
        rc = SQLITERK_NOMEM;
        goto sqliterkValuesAddNumber_Failed;
    }
    *value->any.number = d;
    values->count++;
    return SQLITERK_OK;

sqliterkValuesAddNumber_Failed:
    sqliterkValueClear(value);
    return rc;
}

int sqliterkValuesAddText(sqliterk_values *values, const char *t)
{
    return sqliterkValuesAddNoTerminatorText(values, t, (int) strlen(t));
}

int sqliterkValuesAddNoTerminatorText(sqliterk_values *values,
                                      const char *t,
                                      const int s)
{
    if (!values || !t) {
        return SQLITERK_MISUSE;
    }
    int rc = sqliterkValuesAutoGrow(values);
    if (rc != SQLITERK_OK) {
        return rc;
    }
    sqliterk_value *value = &values->values[values->count];
    value->type = sqliterk_value_type_text;
    value->any.text = sqliterkOSMalloc(sizeof(sqliterk_text));
    if (!value->any.text) {
        rc = SQLITERK_NOMEM;
        goto sqliterkValuesAddNoTerminatorText_Failed;
    }
    value->any.text->s = s;
    value->any.text->t = sqliterkOSMalloc(sizeof(char) * (s + 1));
    if (!value->any.text->t) {
        rc = SQLITERK_NOMEM;
        goto sqliterkValuesAddNoTerminatorText_Failed;
    }
    memcpy(value->any.text->t, t, s);
    value->any.text->t[s] = '\0';
    values->count++;
    return SQLITERK_OK;

sqliterkValuesAddNoTerminatorText_Failed:
    sqliterkValueClear(value);
    return rc;
}

int sqliterkValuesAddBinary(sqliterk_values *values, const void *b, const int s)
{
    if (!values || !b) {
        return SQLITERK_MISUSE;
    }
    int rc = sqliterkValuesAutoGrow(values);
    if (rc != SQLITERK_OK) {
        return rc;
    }
    sqliterk_value *value = &values->values[values->count];
    value->type = sqliterk_value_type_binary;
    value->any.binary = sqliterkOSMalloc(sizeof(sqliterk_binary));
    if (!value->any.binary) {
        rc = SQLITERK_NOMEM;
        goto sqliterkValuesAddBinary_Failed;
    }
    value->any.binary->s = s;
    value->any.binary->b = sqliterkOSMalloc(sizeof(void *) * s);
    if (!value->any.binary->b) {
        return SQLITERK_NOMEM;
    }
    memcpy(value->any.binary->b, b, s);
    values->count++;
    return SQLITERK_OK;

sqliterkValuesAddBinary_Failed:
    sqliterkValueClear(value);
    return rc;
}

int sqliterkValuesAddNull(sqliterk_values *values)
{
    if (!values) {
        return SQLITERK_MISUSE;
    }
    int rc = sqliterkValuesAutoGrow(values);
    if (rc != SQLITERK_OK) {
        return rc;
    }
    sqliterk_value *value = &values->values[values->count];
    value->type = sqliterk_value_type_null;
    value->any.memory = NULL;
    values->count++;
    return SQLITERK_OK;
}

int sqliterkValueClear(sqliterk_value *value)
{
    if (!value) {
        return SQLITERK_MISUSE;
    }
    if (value->any.memory) {
        switch (value->type) {
            case sqliterk_value_type_text:
                if (value->any.text->t) {
                    sqliterkOSFree(value->any.text->t);
                    value->any.text->t = NULL;
                }
                break;
            case sqliterk_value_type_binary:
                if (value->any.binary->b) {
                    sqliterkOSFree(value->any.binary->b);
                    value->any.binary->b = NULL;
                }
                break;
            default:
                break;
        }
        sqliterkOSFree(value->any.memory);
        value->any.memory = NULL;
    }
    value->type = sqliterk_value_type_null;
    return SQLITERK_OK;
}