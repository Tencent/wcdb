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

package com.tencent.wcdb.database;

import com.tencent.wcdb.DatabaseErrorHandler;

/**
 * Describes encryption options for SQLite databases.
 *
 * @see SQLiteDatabase#openDatabase(String, byte[], SQLiteCipherSpec, SQLiteDatabase.CursorFactory, int, DatabaseErrorHandler, int)
 */
public class SQLiteCipherSpec {

    /** 256-bit AES encryption, CBC mode. */
    public static final String CIPHER_AES256CBC = "aes-256-cbc";

    /**
     * XXTEA encryption, under development.
     * @hide
     */
    public static final String CIPHER_XXTEA = "xxtea";

    /**
     * Default encryption (AES-256-CBC) with device lock, under development.
     * @hide
     */
    public static final String CIPHER_DEVLOCK = "devlock";


    /**
     * Encryption method used, null for default method, which is 256-bit AES in CBC mode.
     */
    public String cipher;

    /**
     * KDF iteration times to generate encryption keys, 0 for default value.
     *
     * <p>Typical values are 4000 for SQLCipher version 1.x and 2.x, 64000 for
     * version 3.x. For databases created by WCDB itself, leave it 0.</p>
     */
    public int kdfIteration;

    /**
     * Whether HMAC should be used.
     *
     * <p>Defaulted to {@code false} for SQLCipher 1.x, and {@code true} otherwise.
     * Must be set to the same value as when the database is created, or the database
     * cannot be opened and {@link SQLiteException} is thrown.</p>
     */
    public boolean hmacEnabled = true;

    /**
     * Page size to use.
     *
     * <p>Page size must be set to the same value as when the database is created, in order to
     * open existing encrypted database. For databases created by WCDB itself, leave it to
     * the default is preferred for most applications. If database cannot be opened and
     * {@link SQLiteException} is thrown, try setting it to 1024 or 4096.
     * </p>
     */
    public int pageSize = SQLiteGlobal.defaultPageSize;

    /**
     * Create a new object with default values.
     */
    public SQLiteCipherSpec() {
    }

    /**
     * Create a new object as a copy of an existed one.
     *
     * @param rhs   The object to be copied
     */
    public SQLiteCipherSpec(SQLiteCipherSpec rhs) {
        cipher = rhs.cipher;
        kdfIteration = rhs.kdfIteration;
        hmacEnabled = rhs.hmacEnabled;
        pageSize = rhs.pageSize;
    }

    /**
     * Set encryption method to be used.
     *
     * @param c The method name to be used
     * @return  This object to allow for chaining of calls to set methods
     * @see #cipher
     */
    public SQLiteCipherSpec setCipher(String c) {
        cipher = c; return this;
    }

    /**
     * Set KDF iteration times to be used.
     *
     * @param iter  New iteration times value
     * @return      This object to allow for chaining of calls to set methods
     * @see #kdfIteration
     */
    public SQLiteCipherSpec setKDFIteration(int iter) {
        kdfIteration = iter; return this;
    }

    /**
     * Set whether to use HMAC in encryption. Default to true.
     *
     * @param enabled   Whether to use HMAC
     * @return          This object to allow for chaining of calls to set methods
     * @see #hmacEnabled
     */
    public SQLiteCipherSpec withHMACEnabled(boolean enabled) {
        hmacEnabled = enabled; return this;
    }

    /**
     * Set the page size to use. The page size should be a power of two.
     *
     * @param size  Page size in byte
     * @return      This object to allow for chaining of calls to set methods
     * @see #pageSize
     */
    public SQLiteCipherSpec setPageSize(int size) {
        pageSize = size; return this;
    }

    /**
     * Set SQLCipher version compatibility. The version should be 1, 2 or 3 which
     * represent SQLCipher 1.x, 2.x or 3.x. This method overwrites HMAC and KDF
     * iteration settings.
     *
     * @param version   SQLCipher version, should be 1, 2 or 3
     * @return          This object to allow for chaining of calls to set methods
     * @see #kdfIteration
     * @see #hmacEnabled
     */
    public SQLiteCipherSpec setSQLCipherVersion(int version) {
        switch (version) {
            case 1: hmacEnabled = false; kdfIteration = 4000;  break;
            case 2: hmacEnabled = true;  kdfIteration = 4000;  break;
            case 3: hmacEnabled = true;  kdfIteration = 64000; break;
            default: throw new IllegalArgumentException("Unsupported SQLCipher version: " + version);
        }
        return this;
    }
}
