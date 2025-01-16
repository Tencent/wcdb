package com.tencent.wcdb.utils.leak;

public interface IHandleLeak {
    void startTimer(String handleHashCode);

    void cancelTimer(String handleHashCode);
}
