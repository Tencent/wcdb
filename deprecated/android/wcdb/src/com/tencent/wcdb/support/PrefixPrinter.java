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

package com.tencent.wcdb.support;

import android.util.Printer;

/**
 * PrefixPrinter is a Printer which prefixes all lines with a given
 * prefix.
 *
 * @hide
 */
public class PrefixPrinter implements Printer {
    private final Printer mPrinter;
    private final String mPrefix;

    /**
     * Creates a new PrefixPrinter.
     *
     * <p>If prefix is null or empty, the provided printer is returned, rather
     * than making a prefixing printer.
     */
    public static Printer create(Printer printer, String prefix) {
        if (prefix == null || prefix.equals("")) {
            return printer;
        }
        return new PrefixPrinter(printer, prefix);
    }

    private PrefixPrinter(Printer printer, String prefix) {
        mPrinter = printer;
        mPrefix = prefix;
    }

    public void println(String str) {
        mPrinter.println(mPrefix + str);
    }
}