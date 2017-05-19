/*
 * Copyright (C) 2008 The Android Open Source Project
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
 * This error can occur if the application creates a SQLiteStatement object and allows multiple
 * threads in the application use it at the same time.
 * Sqlite returns this error if bind and execute methods on this object occur at the same time
 * from multiple threads, like so:
 *     thread # 1: in execute() method of the SQLiteStatement object
 *     while thread # 2: is in bind..() on the same object.
 *</p>
 * FIX this by NEVER sharing the same SQLiteStatement object between threads.
 * Create a local instance of the SQLiteStatement whenever it is needed, use it and close it ASAP.
 * NEVER make it globally available.
 */
@SuppressWarnings("serial")
public class SQLiteMisuseException extends SQLiteException {
    public SQLiteMisuseException() {}

    public SQLiteMisuseException(String error) {
        super(error);
    }
}
