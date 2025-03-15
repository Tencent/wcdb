package com.tencent.wcdb.tracer

import com.tencent.wcdb.core.Database
import java.util.concurrent.ConcurrentSkipListSet

/**
 * Currently, WCDB does not support monitoring connection pool busy status for each database.
 * See: https://github.com/Tencent/wcdb/issues/1267
 * Therefore, we consolidate the registration points for connection pool busy tracers here
 * to make it easier for upper-layer businesses to add busy monitoring.
 */
object WCDBConnectionPoolBusyTracer {

    // Timeout in seconds for blocking database operation. default is 30 seconds
    private var timeOutSeconds: Double = 30.0

    // single global tracer for WCDB-Native monitoring
    private var globalTracer: Database.BusyTracer? = null

    // tracer list registered by upper-layer businesses, will be notified when connection pool is busy
    private var tracerList: ConcurrentSkipListSet<Database.BusyTracer>? = null

    /**
     * Register a connection pool busy tracer.
     */
    fun registerGlobalTracer(tracer: Database.BusyTracer) {
        if (tracerList == null) {
            tracerList = ConcurrentSkipListSet()
        }
        val list = tracerList ?: return
        if (list.contains(tracer)) {
            // If already registered, return directly
            return
        }
        // Add to the list
        list.add(tracer)
        if (globalTracer == null) {
            // If not registered yet, register it
            globalTracer = Database.BusyTracer { tag, path, tid, sql ->
                // Notify all observers
                val toNotifyList = tracerList
                if (toNotifyList != null) {
                    for (t in toNotifyList) {
                        t.onTrace(tag, path, tid, sql)
                    }
                }
            }
            // Consolidate the call point to prevent misuse by businesses
            @Suppress("DEPRECATION")
            Database.globalTraceBusy(globalTracer, timeOutSeconds)
        }
    }

    /**
     * Unregister a connection pool busy tracer.
     */
    fun unregisterGlobalTracer(tracer: Database.BusyTracer) {
        // If not registered, return directly
        val list = tracerList ?: return
        if (list.contains(tracer)) {
            // If contained, remove it
            list.remove(tracer)
        }
        if (list.isEmpty()) {
            // If the tracer list is empty, unregister the monitoring
            globalTracer = null
            // Unregister
            @Suppress("DEPRECATION")
            Database.globalTraceBusy(null, timeOutSeconds)
        }
    }

    /**
     * Change busy time out seconds.
     */
    fun updateBusyTimeOutSeconds(timeOutSeconds: Double) {
        WCDBConnectionPoolBusyTracer.timeOutSeconds = timeOutSeconds
    }
}