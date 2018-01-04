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

//TODO: refactor for configs
public struct Configs {
    public typealias Callback = Config.Callback
    public typealias Order = Config.Order

    public struct Config: Equatable {
        public typealias Callback = (Handle) throws -> Void
        typealias TaggedCallback = Tagged<Callback>
        public typealias Order = Int

        let name: String
        let callback: TaggedCallback?
        let order: Order

        init(named name: String, with callback: @escaping Callback, orderBy order: Order) {
            self.name = name
            self.callback = Tagged(callback)
            self.order = order
        }

        init(emptyConfigNamed name: String, orderBy order: Order) {
            self.name = name
            self.callback = nil
            self.order = order
        }

        public static func == (lhs: Config, rhs: Config) -> Bool {
            return lhs.name == rhs.name && lhs.callback == rhs.callback && lhs.order == rhs.order
        }
    }

    private var configs: ContiguousArray<Config>

    internal init(_ configs: ContiguousArray<Config>) {
        self.configs = configs
    }

    internal mutating func setConfig(named name: String, with callback: @escaping Callback, orderBy order: Order) {
        var inserted = false
        var newConfigs = configs.reduce(into: ContiguousArray<Config>()) { (result, config) in
            if !inserted && order < config.order {
                result.append(Config(named: name, with: callback, orderBy: order))
                inserted = true
            } else if name != config.name {
                result.append(config)
            }
        }
        if !inserted {
            newConfigs.append(Config(named: name, with: callback, orderBy: order))
        }
        configs = newConfigs
    }

    internal mutating func setConfig(named name: String, with callback: @escaping Callback) {
        if let targetIndex = configs.index(where: { $0.name == name }) {
            let config = configs[targetIndex]
            configs[targetIndex] = Config(named: name, with: callback, orderBy: config.order)
        } else {
            configs.append(Config(named: name, with: callback, orderBy: .max))
        }
    }

    internal func invoke(handle: Handle) throws {
        let configs = self.configs
        for config in configs {
            try config.callback?.value(handle)
        }
    }

    internal func config(by name: String) -> Callback? {
        let configs = self.configs
        return configs.first { $0.name == name }?.callback?.value
    }
}

extension Configs: Equatable {
    public static func == (lhs: Configs, rhs: Configs) -> Bool {
        return lhs.configs.elementsEqual(rhs.configs)
    }
}
