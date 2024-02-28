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

package com.tencent.wcdb.support;

/**
 * API for WCDB log redirection.
 *
 * <p>Typically, applications call {@link #setLogger(int)} or {@link #setLogger(LogCallback)}
 * to setup logger callback, WCDB library itself uses the rest methods to
 * send log outputs to the preset or user defined logger.</p>
 */
public class Log {
	
	private Log() {}

	/**
	 * Preset No-op logger which drops all log outputs.
	 * Pass it to {@link #setLogger(int)}.
	 */
	public static final int LOGGER_NONE = 0;

	/**
	 * Default logger which output logs to Android system log (logcat).
	 * Pass it to {@link #setLogger(int)}.
	 */
	public static final int LOGGER_DEFAULT = 1;


	/**
	 * User defined logger interface. Implement this interface and pass
	 * an instance to {@link #setLogger(LogCallback)} to redirect log
	 * outputs to custom loggers.
	 */
	public interface LogCallback {

		/**
		 * Called whenever a log is being written.
		 * @param priority	the priority of this log message. Use constants in Android
		 *                  logging API, {@link android.util.Log}
		 * @param tag		tag of this log message
		 * @param msg		message content
		 */
		void println(int priority, String tag, String msg);
	}

	private static LogCallback mCallback = null;


	private static native void nativeSetLogger(int logger, LogCallback callback);
	private static native void nativePrintLn(int priority, String tag, String msg);

	/**
	 * Use preset logger for log outputs.
	 *
	 * @param logger	preset logger, valid values are
	 *                  <ol>
	 *                  <li>{@link #LOGGER_NONE}</li>
	 *                  <li>{@link #LOGGER_DEFAULT}</li>
	 *                  </ol>
	 */
	public static void setLogger(int logger) {
		mCallback = null;
		nativeSetLogger(logger, null);
	}

	/**
	 * Use user defined logger for log outputs.
	 *
	 * @param callback	logger callback
	 */
	public static void setLogger(LogCallback callback) {
		mCallback = callback;
		nativeSetLogger(-1, callback);
	}

	/**
	 * Low-level logging call. Typically called by WCDB library itself.
	 *
	 * @param priority	the priority of this log message. Use constants in Android
	 *                  logging API, {@link android.util.Log}
	 * @param tag		tag of this log message
	 * @param msg		message content
	 */
	public static void println(int priority, String tag, String msg) {
		if (mCallback != null)
			mCallback.println(priority, tag, msg);
		else
			nativePrintLn(priority, tag, msg);
	}

	/**
	 * Send a FATAL log message. Typically called by WCDB library itself.
 	 *
	 * @param tag	tag of this log message
	 * @param msg	message content
	 */
	public static void f(final String tag, final String msg) {
		println(android.util.Log.ASSERT, tag, msg);
	}

	/**
	 * Send an ERROR log message. Typically called by WCDB library itself.
	 *
	 * @param tag	tag of this log message
	 * @param msg	message content
	 */
	public static void e(final String tag, final String msg) {
		println(android.util.Log.ERROR, tag, msg);
	}

	/**
	 * Send a WARN log message. Typically called by WCDB library itself.
	 *
	 * @param tag	tag of this log message
	 * @param msg	message content
	 */
	public static void w(final String tag, final String msg) {
		println(android.util.Log.WARN, tag, msg);
	}

	/**
	 * Send an INFO log message. Typically called by WCDB library itself.
	 *
	 * @param tag	tag of this log message
	 * @param msg	message content
	 */
	public static void i(final String tag, final String msg) {
		println(android.util.Log.INFO, tag, msg);
	}

	/**
	 * Send a DEBUG log message. Typically called by WCDB library itself.
	 *
	 * @param tag	tag of this log message
	 * @param msg	message content
	 */
	public static void d(final String tag, final String msg) {
		println(android.util.Log.DEBUG, tag, msg);
	}

	/**
	 * Send a VERBOSE log message. Typically called by WCDB library itself.
	 *
	 * @param tag	tag of this log message
	 * @param msg	message content
	 */
	public static void v(final String tag, final String msg) {
		println(android.util.Log.VERBOSE, tag, msg);
	}

	/**
	 * Send a FATAL log message with string formatter.
	 * Typically called by WCDB library itself.
	 *
	 * @param tag		tag of this log message
	 * @param format	message content
	 * @param args 		format arguments
	 */
	public static void f(final String tag, final String format, final Object... args) {
		println(android.util.Log.ASSERT, tag, String.format(format, args));
	}

	/**
	 * Send an ERROR log message with string formatter.
	 * Typically called by WCDB library itself.
	 *
	 * @param tag		tag of this log message
	 * @param format	message content
	 * @param args 		format arguments
	 */
	public static void e(final String tag, final String format, final Object... args) {
		println(android.util.Log.ERROR, tag, String.format(format, args));
	}

	/**
	 * Send a WARN log message with string formatter.
	 * Typically called by WCDB library itself.
	 *
	 * @param tag		tag of this log message
	 * @param format	message content
	 * @param args 		format arguments
	 */
	public static void w(final String tag, final String format, final Object... args) {
		println(android.util.Log.WARN, tag, String.format(format, args));
	}

	/**
	 * Send an INFO log message with string formatter.
	 * Typically called by WCDB library itself.
	 *
	 * @param tag		tag of this log message
	 * @param format	message content
	 * @param args 		format arguments
	 */
	public static void i(final String tag, final String format, final Object... args) {
		println(android.util.Log.INFO, tag, String.format(format, args));
	}

	/**
	 * Send a DEBUG log message with string formatter.
	 * Typically called by WCDB library itself.
	 *
	 * @param tag		tag of this log message
	 * @param format	message content
	 * @param args 		format arguments
	 */
	public static void d(final String tag, final String format, final Object... args) {
		println(android.util.Log.DEBUG, tag, String.format(format, args));
	}

	/**
	 * Send a VERBOSE log message with string formatter.
	 * Typically called by WCDB library itself.
	 *
	 * @param tag		tag of this log message
	 * @param format	message content
	 * @param args 		format arguments
	 */
	public static void v(final String tag, final String format, final Object... args) {
		println(android.util.Log.VERBOSE, tag, String.format(format, args));
	}

}

