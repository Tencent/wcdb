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

import java.io.File;

import com.tencent.wcdb.DatabaseErrorHandler;
import com.tencent.wcdb.FileUtils;
import com.tencent.wcdb.database.SQLiteCipherSpec;
import com.tencent.wcdb.database.SQLiteDatabase;
import com.tencent.wcdb.database.SQLiteDatabase.CursorFactory;

/**
 * Wrapper class to implement database opening methods as {@link android.content.Context}
 * does.
 */
public final class Context {

	static {
		// Ensure libwcdb.so is loaded.
		SQLiteDatabase.loadLib();
	}

	private static File getDataDirFile(android.content.Context context) {
		if (context != null) {
			final String mDataDir = context.getApplicationInfo().dataDir;
			final File mDataDirFile = mDataDir != null ? new File(mDataDir) : null;
			return mDataDirFile;
		}
		throw new RuntimeException("Not supported in system context");
	}

	private static File getDatabasesDir(android.content.Context context) {
		File mDatabasesDir = new File(getDataDirFile(context), "databases");
		if (mDatabasesDir.getPath().equals("databases")) {
			mDatabasesDir = new File("/data/system");
		}
		return mDatabasesDir;
	}

	private static File makeFilename(File base, String name) {
		if (name.indexOf(File.separatorChar) < 0) {
			return new File(base, name);
		}
		throw new IllegalArgumentException("File " + name + " contains a path separator");
	}

	private static File validateFilePath(android.content.Context context, String name,
			boolean createDirectory) {
		File dir;
		File f;

		if (name.charAt(0) == File.separatorChar) {
			String dirPath = name.substring(0, name.lastIndexOf(File.separatorChar));
			dir = new File(dirPath);
			name = name.substring(name.lastIndexOf(File.separatorChar));
			f = new File(dir, name);
		} else {
			dir = getDatabasesDir(context);
			f = makeFilename(dir, name);
		}

		if (createDirectory && !dir.isDirectory() && dir.mkdir()) {
			FileUtils.setPermissions(dir.getPath(), FileUtils.S_IRWXU | FileUtils.S_IRWXG
					| FileUtils.S_IXOTH, -1, -1);
		}

		return f;
	}

	private static void setFilePermissionsFromMode(String name, int mode, int extraPermissions) {
		int perms = FileUtils.S_IRUSR | FileUtils.S_IWUSR | FileUtils.S_IRGRP | FileUtils.S_IWGRP
				| extraPermissions;
		if ((mode & android.content.Context.MODE_WORLD_READABLE) != 0) {
			perms |= FileUtils.S_IROTH;
		}
		if ((mode & android.content.Context.MODE_WORLD_WRITEABLE) != 0) {
			perms |= FileUtils.S_IWOTH;
		}
		FileUtils.setPermissions(name, perms, -1, -1);
	}

	public static final int MODE_ENABLE_WRITE_AHEAD_LOGGING = 0x0008;

	/**
	 * Open a new private SQLiteDatabase associated with this Context's
	 * application package. Create the database file if it doesn't exist.
	 *
	 * @param name The name (unique in the application package) of the database.
	 * @param mode Operating mode. Use 0 or {@link android.content.Context#MODE_PRIVATE} for the
	 *            default operation. Use
	 *            {@link android.content.Context#MODE_ENABLE_WRITE_AHEAD_LOGGING} to enable write-ahead
	 *            logging by default. Use {@link android.content.Context#MODE_NO_LOCALIZED_COLLATORS}
	 *            to disable localized collators.
	 * @param factory An optional factory class that is called to instantiate a
	 *            cursor when query is called.
	 * @return The contents of a newly created database with the given name.
	 * @throws android.database.sqlite.SQLiteException if the database file
	 *             could not be opened.
	 */
	public static SQLiteDatabase openOrCreateDatabase(android.content.Context context, String name, int mode,
			CursorFactory factory) {
		return openOrCreateDatabase(context, name, null, null, mode, factory, null);
	}

	public static SQLiteDatabase openOrCreateDatabase(android.content.Context context, String name, int mode,
			CursorFactory factory, DatabaseErrorHandler errorHandler) {
		return openOrCreateDatabase(context, name, null, null, mode, factory, errorHandler);
	}

	public static SQLiteDatabase openOrCreateDatabase(android.content.Context context, String name,
			byte[] password, SQLiteCipherSpec cipher, int mode, CursorFactory factory) {
		return openOrCreateDatabase(context, name, password, cipher, mode,
				factory, null);
	}

	static public SQLiteDatabase openOrCreateDatabase(android.content.Context context, String name,
			byte[] password, SQLiteCipherSpec cipher, int mode, CursorFactory factory,
			DatabaseErrorHandler errorHandler) {
		File f = validateFilePath(context, name, true);
		int flags = SQLiteDatabase.CREATE_IF_NECESSARY;
		if ((mode & MODE_ENABLE_WRITE_AHEAD_LOGGING) != 0) {
			flags |= SQLiteDatabase.ENABLE_WRITE_AHEAD_LOGGING;
		}

		SQLiteDatabase db = SQLiteDatabase.openDatabase(f.getPath(), password, cipher, factory, flags,
				errorHandler, 0);

		setFilePermissionsFromMode(f.getPath(), mode, 0);
		return db;
	}
}
