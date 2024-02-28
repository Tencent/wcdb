/*
 * Copyright (C) 2010 The Android Open Source Project
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

package com.tencent.wcdb.database;

/**
 * This exception class is used when sqlite can't access the database file
 * due to lack of permissions on the file.
 */
@SuppressWarnings("serial")
public class SQLiteAccessPermException extends SQLiteException {
    public SQLiteAccessPermException() {}

    public SQLiteAccessPermException(String error) {
        super(error);
    }
}
