package com.tencent.wcdbtest.winq;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import com.tencent.wcdb.winq.*;
import static com.tencent.wcdbtest.base.WinqTool.winqEqual;

import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class LiteralValueTest {
    @Test
    public void test() {
        winqEqual(new LiteralValue(1), "1");
        winqEqual(new LiteralValue(false), "FALSE");
        winqEqual(new LiteralValue(true), "TRUE");
        winqEqual(new LiteralValue(17626545782784L), "17626545782784");
        winqEqual(new LiteralValue(-9223372036854775808L), "-9223372036854775808");
        winqEqual(new LiteralValue(9223372036854775807L), "9223372036854775807");
        winqEqual(new LiteralValue(0.1), "0.10000000000000001");
        winqEqual(new LiteralValue("testContent"), "'testContent'");
        winqEqual(new LiteralValue(null), "NULL");
        winqEqual(LiteralValue.currentDate(), "CURRENT_DATE");
        winqEqual(LiteralValue.currentTimeStamp(), "CURRENT_TIMESTAMP");
        winqEqual(LiteralValue.currentTime(), "CURRENT_TIME");
    }
}