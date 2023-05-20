package com.tencent.wcdbtest.base;

import com.tencent.wcdb.winq.Identifier;

import org.junit.Assert;

public class WinqTool {
     public static void winqEqual(Identifier winq, String sql) {
          Assert.assertEquals(winq.getDescription(), sql);
     }
}
