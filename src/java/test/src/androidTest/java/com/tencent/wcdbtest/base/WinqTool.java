package com.tencent.wcdbtest.base;

import com.tencent.wcdb.winq.Identifier;

import static org.junit.Assert.assertEquals;

import org.junit.Assert;

public class WinqTool {
     public static void winqEqual(Identifier winq, String sql) {
          Assert.assertEquals(winq.getDescription(), sql);
     }
}
