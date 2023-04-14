package com.tencent.wcdb.TestUtility;

import com.tencent.wcdb.winq.Identifier;

import static org.junit.Assert.assertEquals;

public class TestTool {
     public static void winqEqual(Identifier winq, String sql) {
          assertEquals(winq.getDescription(), sql);
     }
}
