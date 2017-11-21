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

class Tracer {
    typealias SQLTracer = (String) -> Void // SQL
    private var sqlTracer: SQLTracer? = nil {
        didSet {
            setup()
        }
    }
    
    typealias PerformanceTracer = ([String:Int], Int64) -> Void // (SQL, count), cost
    private var performanceTracer: PerformanceTracer? = nil {
        didSet {
            setup()
        }
    }
    private var shouldAggregation = false
    private var footprint: [String:Int] = [:]
    private var cost: Int64 = 0
    
    private let handle: sqlite3
    
    init(with handle: sqlite3) {
        self.handle = handle
    }
    
    deinit {
        reportPerformance()
    }
    
    private var flag: Int32 {
        var flag: Int32 = 0
        if sqlTracer != nil {
            flag |= SQLITE_TRACE_STMT
        }
        if performanceTracer != nil {
            flag |= SQLITE_TRACE_PROFILE
        }
        return flag
    }
    
    private func setup() {
        let flag: Int32 = self.flag
        guard flag > 0 else {
            sqlite3_trace_v2(handle, 0, nil, nil)
            return 
        }
        var this = self
        sqlite3_trace_v2(handle, UInt32(flag), { (flag, M, P, X) -> Int32 in
            let tracer = UnsafePointer<Tracer>(OpaquePointer(M)!).pointee
            let stmt = OpaquePointer(P)
            guard let csql = sqlite3_sql(stmt) else {
                return SQLITE_MISUSE
            } 
            let sql = String(cString: csql)
            switch (flag) {
            case UInt32(SQLITE_TRACE_STMT):
                tracer.report(sql: sql)
            case UInt32(SQLITE_TRACE_PROFILE):
                let cost: Int64 = UnsafePointer<Int64>(OpaquePointer(X)!).pointee
                
                //report last track 
                if !tracer.shouldAggregation {
                    tracer.reportPerformance()
                }
                
                tracer.recordPerformance(sql: sql, cost: cost)
            default: break
            }
            return SQLITE_OK
        }, &this)
    }
    
    private func report(sql: String) {
        guard let sqlTracer = self.sqlTracer else {
            return 
        }
        sqlTracer(sql)
    }
    
    private func reportPerformance() {
        guard footprint.isEmpty else {
            return
        }
        guard let performanceTracer = self.performanceTracer else {
            return
        } 
        performanceTracer(footprint, cost)
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
