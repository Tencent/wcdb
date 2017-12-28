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

public final class HandlePool {
    private class Wrap {
        let handlePool: HandlePool
        var reference: Int = 0
        init(_ handlePool: HandlePool) {
            self.handlePool = handlePool
        }
    }

    private static let spin = Spin()
    private static var pools: [String: Wrap] = [:]

    static func getPool(withPath path: String, defaultConfigs: Configs) -> RecyclableHandlePool {
        spin.lock(); defer { spin.unlock() }
        var index = pools.index(forKey: path)
        if index == nil {
            let handlePool = HandlePool(withPath: path, defaultConfigs: defaultConfigs)
            pools[path] = Wrap(handlePool)
            index = pools.index(forKey: path)
        }
        return getExistingPool(atIndex: index!)
    }

    static func getExistingPool(with tag: Tag) throws -> RecyclableHandlePool {
        spin.lock(); defer { spin.unlock() }
        guard let index = pools.index(where: { (arg) -> Bool in
            return arg.value.handlePool.tag == tag
        }) else {
            throw Error.reportCore(tag: tag,
                                   path: "",
                                   operation: .getPool,
                                   code: .misuse,
                                   message: "Database with tag: \(tag) is not exists.")
        }
        return getExistingPool(atIndex: index)
    }

    static func getExistingPool(withPath path: String) throws -> RecyclableHandlePool {
        spin.lock(); defer { spin.unlock() }
        guard let index = pools.index(forKey: path) else {
            throw Error.reportCore(tag: nil,
                                   path: path,
                                   operation: .getPool,
                                   code: .misuse,
                                   message: "Database at path: \(path) is not exists.")
        }
        return getExistingPool(atIndex: index)
    }

    private static func getExistingPool(atIndex index: Dictionary<String, Wrap>.Index) -> RecyclableHandlePool {
        let node = pools[index]
        let path = node.key
        var wrap = node.value
        wrap.reference += 1
        return Recyclable(wrap.handlePool, onRecycled: {
            spin.lock(); defer { spin.unlock() }
            let wrap = pools[path]!
            wrap.reference -= 1
            if wrap.reference == 0 {
                pools.removeValue(forKey: path)
            }
        })
    }

    typealias HandleWrap = (handle: Handle, configs: Configs)
    private let handles = ConcurrentList<HandleWrap>(withCapacityCap: maxHardwareConcurrency)

    var tag: Tag?
    let path: String
    private let rwlock = RWLock()
    private let aliveHandleCount = Atomic<Int>(0)
    private var configs: Configs

    private init(withPath path: String, defaultConfigs: Configs) {
        self.path = path
        self.configs = defaultConfigs
    }

    var isDrained: Bool {
        return aliveHandleCount == 0
    }

    func fillOne() throws {
        rwlock.lockRead(); defer { rwlock.unlockRead() }
        let handle = try generate()
        if handles.pushBack(handle) {
            aliveHandleCount += 1
        }
    }

    private func invoke(handleWrap: inout HandleWrap) throws {
        let newConfigs = self.configs
        if newConfigs != handleWrap.configs {
            try newConfigs.invoke(handle: handleWrap.handle)
            handleWrap.configs = newConfigs
        }
    }

    static private let maxConcurrency = max(maxHardwareConcurrency, 64)
    static private let maxHardwareConcurrency = ProcessInfo.processInfo.processorCount

    func flowOut() throws -> RecyclableHandle {
        var unlock = true
        rwlock.lockRead(); defer {  if unlock { rwlock.unlockRead() } }
        var handleWrap = handles.popBack()
        if handleWrap == nil {
            guard aliveHandleCount < HandlePool.maxConcurrency else {
                throw Error.reportCore(tag: tag,
                                       path: path,
                                       operation: .flowOut,
                                       code: .exceed,
                                       message: "The concurrency of database exceeds the max concurrency")
            }
            handleWrap = try generate()
            aliveHandleCount += 1
            if aliveHandleCount > HandlePool.maxHardwareConcurrency {
                var warning = "The concurrency of database: \(tag ?? 0) with \(aliveHandleCount)"
                warning.append(" exceeds the concurrency of hardware: \(HandlePool.maxHardwareConcurrency)")
                Error.warning(warning)
            }
        }
        handleWrap!.handle.tag = self.tag
        try invoke(handleWrap: &handleWrap!)
        unlock = false
        return RecyclableHandle(handleWrap!, onRecycled: {
            self.flowBack(handleWrap!)
        })
    }

    private func flowBack(_ handleWrap: HandleWrap) {
        let inserted = handles.pushBack(handleWrap)
        rwlock.unlockRead()
        if !inserted {
            aliveHandleCount -= 1
        }
    }

    private func generate() throws -> HandleWrap {
        let handle = Handle(withPath: path)
        handle.tag = tag
        let configs = self.configs
        try handle.open()
        try configs.invoke(handle: handle)
        return HandleWrap(handle: handle, configs: configs)
    }

    func blockade() {
        rwlock.lockWrite()
    }

    func unblockade() {
        rwlock.unlockWrite()
    }

    var isBlockaded: Bool {
        return rwlock.isWriting
    }

    public typealias OnDrained = () throws -> Void

    func drain(onDrained: OnDrained) rethrows {
        blockade(); defer { unblockade() }
        let size = handles.clear()
        aliveHandleCount -= size
        try onDrained()
    }

    func drain() {
        blockade(); defer { unblockade() }
        let size = handles.clear()
        aliveHandleCount -= size
    }

    func purgeFreeHandles() {
        rwlock.lockRead(); defer { rwlock.unlockRead() }
        let size = handles.clear()
        aliveHandleCount -= size
    }

    public typealias Config = Configs.Callback
    public typealias ConfigOrder = Configs.Order

    func setConfig(named name: String, with callback: @escaping Config, orderBy order: ConfigOrder) {
        configs.setConfig(named: name, with: callback, orderBy: order)
    }

    func setConfig(named name: String, with callback: @escaping Config) {
        configs.setConfig(named: name, with: callback)
    }

    static func purgeFreeHandlesInAllPools() {
        let handlePools: [HandlePool]!
        do {
            spin.lock(); defer { spin.unlock() }
            handlePools = pools.values.reduce(into: []) { $0.append($1.handlePool) }
        }
        handlePools.forEach { $0.purgeFreeHandles() }
    }
}
