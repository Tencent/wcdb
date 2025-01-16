package com.tencent.wcdb.utils.leak;

import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;

import com.tencent.wcdb.core.Handle;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.TimeUnit;

/**
 * There is a bug recording to the {@link Handle} that created from {@link Handle#getCppHandle}
 */
public class HandleLeakUtil implements IHandleLeak {

    public static final String TAG = "WCDB2_Handler_Leak";

    public static boolean enableCheckHandleLeak = false;

    public static final long DELAY_INTERVAL = TimeUnit.SECONDS.toMillis(10);

    private static IHandleLeak inst;

    // handleHashCode map to codeStackTrace
    private final ConcurrentHashMap<String, Runnable> handleLeakHashMap = new ConcurrentHashMap<>();

    private final Handler handler;


    public HandleLeakUtil() {
        HandlerThread handlerThread = new HandlerThread("handlerCheck");
        handlerThread.start();
        this.handler = new Handler(handlerThread.getLooper());
    }

    public static IHandleLeak inst() {
        if (inst != null) {
            return inst;
        }
        if (enableCheckHandleLeak) {
            inst = new HandleLeakUtil();
        } else {
            inst = new HandleLeakNoop();
        }
        return inst;
    }

    /**
     * start to record the stackTrace which will be printed if the handle is leaked and not recycled more than 10 seconds {@link HandleLeakUtil#DELAY_INTERVAL}
     *
     * @param handleHashCode Handle's hashCode {@link Handle}
     */
    @Override
    public void startTimer(String handleHashCode) {
        if (handleLeakHashMap.containsKey(handleHashCode)) {
            return;
        }
        Log.d(TAG, "start-checkHandleLeakTimer, handleHashCode=" + handleHashCode);
        String traceLog = Log.getStackTraceString(new RuntimeException("handleLeak=" + handleHashCode + "_tid=" + Thread.currentThread().getId() + "_tName=" + Thread.currentThread().getName()));
        Runnable runnable = () -> Log.e(TAG, traceLog);
        handleLeakHashMap.put(handleHashCode, runnable);
        handler.postDelayed(runnable, DELAY_INTERVAL);
    }

    /**
     * if the {@link Handle} has been recycled, then we remove and cancel the timer
     *
     * @param handleHashCode Handle's hashCode, see {@link Handle}
     */
    @Override
    public void cancelTimer(String handleHashCode) {
        Runnable runnable = handleLeakHashMap.get(handleHashCode);
        if (runnable != null) {
            Log.d(TAG, "cancel-checkHandleLeakTimer, handleHashCode=" + handleHashCode);
            handler.removeCallbacks(runnable);
            handleLeakHashMap.remove(handleHashCode);
        }
    }
}
