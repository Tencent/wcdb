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

package com.tencent.wcdb.repair;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import android.text.TextUtils;

import com.tencent.wcdb.database.SQLiteDatabase;
import com.tencent.wcdb.support.Log;

/**
 * Interface to database recovery dump utility from SQLite command-line tools.
 *
 * @deprecated Use {@link RepairKit} for database corruption recovery.
 */
public class DBDumpUtil {

	private static final String TAG = "WCDB.DBDumpUtil";

	/** @deprecated Use {@link RepairKit} for database corruption recovery. */
	public interface ExecuteSqlCallback {
		String preExecute(String sql);
	}

	public static boolean doRecoveryDb(SQLiteDatabase db, String crashDbPath, String key,
			String outputPath) {
		return doRecoveryDb(db, crashDbPath, key, outputPath, null, null, null, true);
	}

	public static boolean doRecoveryDb(SQLiteDatabase db, String crashDbPath, String key,
			String outputPath, List<String> filterTable, List<String> destTables,
			ExecuteSqlCallback callback, boolean needDeleteAfterSuccess) {
		if (db == null || !db.isOpen()) {
			Log.w(TAG, "Database is not open");
			return false;
		}

		boolean dumpOk = nativeDumpDB(crashDbPath, key, outputPath);
		if (!dumpOk) {
			return false;
		}

		BufferedReader reader;
		try {
			reader = new BufferedReader(new FileReader(outputPath));
		} catch (FileNotFoundException e) {
			Log.w(TAG, "SQL file '%s' not found", outputPath);
			return false;
		}

		String line;
		db.execSQL("PRAGMA foreign_keys=OFF;");
		db.beginTransaction();
		int failureCount = 0;
		int allCount = 0;
		int executeCount = 0;
		try {
			String temp = null;
			boolean contact = false;
			HashMap<String, String> tables = new HashMap<>();

			while ((line = reader.readLine()) != null) {
				if (contact) {
					temp += "\n" + line;
					if (!temp.endsWith(";") || !nativeIsSqlComplete(temp)) {
						continue;
					}
				} else if (line.startsWith("INSERT") || line.startsWith("CREATE TABLE")) {
					if (!line.endsWith(";") || !nativeIsSqlComplete(line)) {
						if (TextUtils.isEmpty(temp)) {
							temp = line;
						} else {
							temp += "\n" + line;
						}
						contact = true;
						continue;
					} else {
						temp = line;
					}
				} else {
					continue;
				}
				contact = false;
				String tableName = null;
				boolean needFilter = false;
				if (destTables != null && destTables.size() > 0) {
					tableName = getTableNameFromSql(temp);
					needFilter = true;
					for (String dest : destTables) {
						if (tableName.equals(dest)) {
							needFilter = false;
							break;
						}
					}

				} else if (filterTable != null && filterTable.size() > 0) {
					tableName = getTableNameFromSql(temp);
					needFilter = false;
					for (String filter : filterTable) {
						if (tableName.equals(filter)) {
							needFilter = true;
							temp = "";
							break;
						}
					}
				}
				if (needFilter) {
					Log.i(TAG, "filter table %s", tableName);
					continue;
				}
				try {
					if (temp.startsWith("CREATE TABLE")) {
						ArrayList<String> columns = getColumnNamesFromSql(temp);
						String bindStr = buildColumnsString(columns);
						tables.put(tableName, bindStr);
					} else if (temp.startsWith("INSERT INTO")) {
						String bindStr = tables.get(tableName);
						if (!TextUtils.isEmpty(bindStr)) {
							StringBuilder sb = new StringBuilder("INSERT INTO ");
							sb.append("\"").append(tableName).append("\"");
							String prefix = sb.toString();
							sb.append(bindStr);
							temp = temp.replace(prefix, sb.toString());
						}
					}
					String tempSql = null;

					if (callback != null) {
						tempSql = callback.preExecute(temp);
					}
					if (!TextUtils.isEmpty(tempSql)) {
						temp = tempSql;
					}
					allCount++;
					db.execSQL(temp);
					executeCount++;
					if (executeCount >= 100) {
						db.setTransactionSuccessful();
						db.endTransaction();
						db.beginTransaction();
						executeCount = 0;
					}
				} catch (Exception e) {
					failureCount++;
				}
				temp = null;
			}
		} catch (IOException e) {
			Log.w(TAG, "I/O error in read sql file ");
			return false;
		} finally {
			if (reader != null) {
				try {
					reader.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
		if (allCount > failureCount) {
			if (db.inTransaction())
				db.setTransactionSuccessful();
			if (db.inTransaction()) {
				db.endTransaction();
			}
			if (needDeleteAfterSuccess) {
				File file = new File(outputPath);
				if (file.exists()) {
					file.delete();
				}
				File crashFile = new File(crashDbPath);
				if (crashFile.exists()) {
					crashFile.delete();
				}
			}
			Log.i(TAG, "restore : %d , fail:%d ", allCount, failureCount);
			return true;
		} else {
			return false;
		}
	}

	public static String buildColumnsString(ArrayList<String> columns) {
		if (columns == null || columns.size() == 0) {
			return "";
		}
		StringBuilder sb = new StringBuilder();
		sb.append("(");
		for (int i = 0; i < columns.size(); i++) {
			sb.append(columns.get(i));
			if (i != columns.size() - 1)
				sb.append(",");
		}
		sb.append(")");

		String buildStr = sb.toString();
		System.out.println(buildStr);
		return buildStr;
	}

	public static String getTableNameFromSql(String sql) {
		if (sql.length() > 100) {
			sql = sql.substring(0, 100);
		}
		String[] str = sql.split("\\s");
		if (str != null && str.length > 1) {
			return str[2].replaceAll("\"", "");
		}
		return null;
	}

	public static byte[] readFromFile(String path) {
		File file = new File(path);
		if (!file.exists()) {
			Log.w(TAG, "readFromFile error, file is not exit, path = %s", path);
			return null;
		}

		FileInputStream fin = null;
		try {
			int size = (int) file.length();
			fin = new FileInputStream(file);

			byte[] buf = new byte[size];
			int count = fin.read(buf);
			if (count != size) {
				Log.w(TAG,
						"readFromFile error, size is not equal, path = %s, file length is %d, count is %d",
						path, size, count);
				return null;

			} else {
				return buf;
			}

		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			if (fin != null) {
				try {
					fin.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}

		Log.e(TAG, "readFromFile failed!");
		return null;
	}

	public static ArrayList<String> getColumnNamesFromSql(String sql) {
		ArrayList<String> columns = new ArrayList<>();
		String temp = sql.substring(sql.indexOf("(") + 1, sql.lastIndexOf(")"));
		String[] Str = temp.trim().split(",");
		for (int i = 0; i < Str.length; i++) {
			Str[i] = Str[i].trim();
			int secondIndex = Str[i].indexOf(" ");
			columns.add(Str[i].substring(0, secondIndex));
		}
		return columns;
	}

	private static native boolean nativeDumpDB(String dbPath, String key, String outputPath);
	private static native boolean nativeIsSqlComplete(String sql);
}
