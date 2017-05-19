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

/**
 * Describes encryption options for SQLite databases.
 */
public class SQLiteCipherSpec {

    public static final String CIPHER_AES256CBC = "aes-256-cbc";
    public static final String CIPHER_XXTEA = "xxtea";
    public static final String CIPHER_DEVLOCK = "devlock";

    /**
     * Encryption method used, null for default method.
     */
    public String cipher;

    /**
     * KDF iteration times to generate encryption keys, 0 for default value.
     * Typical values are 4000 for SQLCipher version 1 and 2, 64000 for
     * version 3.
     */
    public int kdfIteration;

    /**
     * Whether HMAC should be used.
     */
    public boolean hmacEnabled = true;

    /**
     * Page size to use.
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
     * @param c     The method name to be used
     * @return      This object to allow for chaining of calls to set methods
     */
    public SQLiteCipherSpec setCipher(String c) {
        cipher = c; return this;
    }

    /**
     * Set KDF iteration times to be used.
     *
     * @param iter  New iteration times value
     * @return      This object to allow for chaining of calls to set methods
     */
    public SQLiteCipherSpec setKDFIteration(int iter) {
        kdfIteration = iter; return this;
    }

    /**
     * Set whether to use HMAC in encryption. Default to true.
     *
     * @param enabled   Whether to use HMAC
     * @return          This object to allow for chaining of calls to set methods
     */
    public SQLiteCipherSpec withHMACEnabled(boolean enabled) {
        hmacEnabled = enabled; return this;
    }

    /**
     * Set the page size to use. The page size should be a power of two.
     *
     * @param size  Page size in byte
     * @return      This object to allow for chaining of calls to set methods
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
