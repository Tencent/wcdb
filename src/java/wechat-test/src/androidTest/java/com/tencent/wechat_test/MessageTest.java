package com.tencent.wechat_test;

import android.content.Context;

import androidx.test.platform.app.InstrumentationRegistry;
import androidx.test.ext.junit.runners.AndroidJUnit4;

import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.*;

import com.tencent.wcdb.base.WCDBException;
import com.tencent.wcdb.core.Database;
import com.tencent.wcdb.core.Handle;
import com.tencent.wcdb.core.Transaction;
import com.tencent.wechat_test.base.DatabaseTestCase;
import com.tencent.wechat_test.base.RandomTool;
import com.tencent.wechat_test.model.DBAppAttach;
import com.tencent.wechat_test.model.DBImgInfo;
import com.tencent.wechat_test.model.DBMessage;
import com.tencent.wechat_test.model.DBVideoInfo;
import com.tencent.wechat_test.model.ImgInfo;
import com.tencent.wechat_test.model.VideoInfo;

/**
 * Instrumented test, which will execute on an Android device.
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
@RunWith(AndroidJUnit4.class)
public class MessageTest extends DatabaseTestCase {
    @Test
    public void createTable() throws WCDBException {
        long start = System.currentTimeMillis();
        for(int i = 0; i < 10000; i++) {
            if(i % 1000 == 0) {
                database.close(new Database.CloseCallBack() {
                    @Override
                    public void onClose() throws WCDBException {
                        database.truncateCheckpoint();
                    }
                });
            }
            String userName = RandomTool.string(30);
            database.createTable("Message_" + userName, DBMessage.INSTANCE);
            database.createTable("ImgInfo_" + userName, DBImgInfo.INSTANCE);
            database.createTable("Video_" + userName, DBVideoInfo.INSTANCE);
            database.createTable("AppAttach" + userName, DBAppAttach.INSTANCE);
        }

        database.close(new Database.CloseCallBack() {
            @Override
            public void onClose() throws WCDBException {
                database.truncateCheckpoint();
            }
        });
        long cost = System.currentTimeMillis() - start;
        logInfo("Database path: " + database.getPath());
        sleep(1000000);
    }
}