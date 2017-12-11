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

import Foundation

final class Tracer {
    typealias SQLTracer = (String) -> Void // SQL
    private var sqlTracer: SQLTracer? = nil {
        didSet {
            setup()
        }
    }

    typealias PerformanceTracer = ([String: Int], Int64, Any?) -> Void // (SQL, count), cost, userInfo
    private var performanceTracer: PerformanceTracer? = nil {
        didSet {
            setup()
        }
    }
    var shouldAggregation = false
    private var footprint: [String: Int] = [:]
    private var cost: Int64 = 0

    private let handle: SQLite3
    var userInfo: Any?

    init(with handle: SQLite3) {
        self.handle = handle
    }

    deinit {
        reportPerformance()
    }

    struct TraceType: OptionSet {
        let rawValue: UInt32

        static let stmt = TraceType(rawValue: UInt32(SQLITE_TRACE_STMT))
        static let profile = TraceType(rawValue: UInt32(SQLITE_TRACE_PROFILE))
    }

    private var type: TraceType {
        var type = TraceType()
        if sqlTracer != nil {
            type.insert(.stmt)
        }
        if performanceTracer != nil {
            type.insert(.profile)
        }
        return type
    }

    private func setup() {
        let traceType = self.type
        guard !traceType.isEmpty else {
            sqlite3_trace_v2(handle, 0, nil, nil)
            return
        }
        sqlite3_trace_v2(handle, traceType.rawValue, { (flag, context, stmtPointer, costPointer) -> Int32 in
            let traceType = TraceType(rawValue: flag)
            let pointer = Unmanaged<Tracer>.fromOpaque(context!)
            let tracer = pointer.takeUnretainedValue()

            let stmt = OpaquePointer(stmtPointer)
            guard let csql = sqlite3_sql(stmt) else {
                return SQLITE_MISUSE
            }
            let sql = String(cString: csql)
            switch traceType {
            case .stmt:
                tracer.report(sql: sql)
            case .profile:
                let cost: Int64 = UnsafePointer<Int64>(OpaquePointer(costPointer)!).pointee

                //report last track 
                if !tracer.shouldAggregation {
                    tracer.reportPerformance()
                }

                tracer.recordPerformance(sql: sql, cost: cost)
            default: break
            }
            return SQLITE_OK
        }, Unmanaged.passUnretained(self).toOpaque())
    }

    private func report(sql: String) {
        guard let sqlTracer = self.sqlTracer else {
            return
        }
        sqlTracer(sql)
    }

    private func reportPerformance() {
        guard !footprint.isEmpty else {
            return
        }
        guard let performanceTracer = self.performanceTracer else {
            return
        }
        performanceTracer(footprint, cost, userInfo)
        footprint.removeAll()
        cost = 0
    }

    private func recordPerformance(sql: String, cost: Int64) {
        self.footprint[sql] = self.footprint[sql] ?? 1
        self.cost = cost
    }

    func trace(sql sqlTracer: @escaping SQLTracer) {
        self.sqlTracer = sqlTracer
    }

    func track(performance performanceTracer: @escaping PerformanceTracer) {
        self.performanceTracer = performanceTracer
    }
}
