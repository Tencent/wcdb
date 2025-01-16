package com.tencent.wcdb.utils.leak;

public class HandleLeakNoop implements IHandleLeak {

    @Override
    public void startTimer(String handleHashCode) {
        
    }

    @Override
    public void cancelTimer(String handleHashCode) {

    }
}