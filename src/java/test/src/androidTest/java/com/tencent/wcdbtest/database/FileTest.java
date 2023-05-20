// Created by qiuwenchen on 2023/5/8.
//

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

package com.tencent.wcdbtest.database;

import androidx.test.ext.junit.runners.AndroidJUnit4;

import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.core.Database;
import com.tencent.wcdbtest.base.DatabaseTestCase;
import com.tencent.wcdbtest.base.FileTool;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

@RunWith(AndroidJUnit4.class)
public class FileTest extends DatabaseTestCase {
    static String[] dummyPaths;
    @Before
    public void setup() throws WCDBException {
        super.setup();
        dummyPaths = allDummyPath(path);
    }

    String[] allDummyPath(String path) {
        String[] paths = new String[7];
        paths[0] = path;
        paths[1] = path + "-wal";
        paths[2] = path + "-first.material";
        paths[3] = path + "-last.material";
        paths[4] = path + ".factory" + File.separator + new File(path).getName();
        paths[5] = path + "-journal";
        paths[6] = path + "-shm";
        return paths;
    }

    void createDummyFiles() throws IOException {
        for(String path : dummyPaths) {
            FileTool.createFile(path);
        }
    }

    void allFileExists(boolean exist, String[] paths) {
        for(String path : paths) {
            assertEquals(FileTool.fileExist(path), exist);
        }
    }

    @Test
    public void testRemoveFiles() throws Exception {
        createDummyFiles();
        allFileExists(true, dummyPaths);
        assertTrue(database.canOpen());
        assertTrue(database.isOpened());
        database.removeFiles();
        assertFalse(database.isOpened());
        allFileExists(false, dummyPaths);
    }

    @Test
    public void testMoveFiles() throws Exception {
        String moveDirectory = currentDirectory + File.separator + "move";
        String moveDatabase = moveDirectory + File.separator + fileName;
        String[] movedPaths = allDummyPath(moveDatabase);
        allFileExists(false, movedPaths);
        assertTrue(database.canOpen());
        assertTrue(database.isOpened());
        createDummyFiles();
        allFileExists(true, dummyPaths);
        database.moveFile(moveDirectory);
        assertFalse(database.isOpened());
        allFileExists(false, dummyPaths);
        allFileExists(true, movedPaths);
    }

    @Test
    public void testGetFileSize() throws IOException, WCDBException {
        byte[] data = new byte[10];
        createDummyFiles();
        for(String path: dummyPaths) {
            FileOutputStream fileStream = new FileOutputStream(path);
            fileStream.write(data);
        }
        assertEquals(database.getFileSize(), dummyPaths.length * data.length);
    }

    @Test
    public void testAutoCreateImmediateDirectory() {
        String newPath = currentDirectory + File.separator + "immediate" + File.separator + "directory" +
                File.separator + "will" + File.separator + "be" + File.separator + "created" +
                File.separator + "automatically" + File.separator + fileName;
        Database newDatabase = new Database(newPath);
        assertTrue(newDatabase.canOpen());
        assertTrue(FileTool.fileExist(newPath));
    }

    @Test
    public void testPathNormalization() {
        String newPath = currentDirectory + "//directory//database";
        Database newDatabase = new Database(newPath);
        assertTrue(newDatabase.canOpen());
        String normalizationPath = currentDirectory + File.separator + "directory" + File.separator + "database";
        assertEquals(newDatabase.getPath(), normalizationPath);
    }

}
