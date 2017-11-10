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

public struct Configs {
    public typealias Callback = Config.Callback
    public typealias Order = Config.Order
    
    public struct Config: Equatable {
        public typealias Callback = (Handle) throws -> ()
        typealias TaggedCallback = Tagged<Callback>
        public typealias Order = Int

        let name: String
        let callback: TaggedCallback
        let order: Order
        
        init(named name: String, with callback: @escaping Callback, orderBy order: Order) {
            self.name = name
            self.callback = Tagged(callback)
            self.order = order
        }
        
        public static func ==(lhs: Config, rhs: Config) -> Bool {
            return lhs.name == rhs.name && lhs.callback == rhs.callback && lhs.order == rhs.order
        }
        
        public static func !=(lhs: Config, rhs: Config) -> Bool {
            return !(lhs==rhs)
        }
    }
    
    private var configs: [Config] = []
    
    init(_ configs: Config...) {
        self.init(configs)
    }
    init(_ configs: [Config]) {
        self.configs = configs
    }
    
    mutating func setConfig(named name: String, with callback: @escaping Callback, orderBy order: Order) {
        var newConfigs: [Config] = []
        var inserted: Bool = false
        for config in configs {
            if !inserted && order < config.order {
                newConfigs.append(Config(named: name, with: callback, orderBy: order))
                inserted = true
            }else if name != config.name {
                newConfigs.append(config)
            }
        }
        if !inserted {
            newConfigs.append(Config(named: name, with: callback, orderBy: order))
        }
        configs = newConfigs
    }
    
    mutating func setConfig(named name: String, with callback: @escaping Callback) {
        var newConfigs: [Config] = []
        var inserted: Bool = false
        for config in configs {
            if name != config.name {
                newConfigs.append(config)
            }else {
                newConfigs.append(Config(named: name, with: callback, orderBy: config.order))
                inserted = true
            }
        }
        if !inserted {
            newConfigs.append(Config(named: name, with: callback, orderBy: Order.max))
        }
        configs = newConfigs
    }
    
    func invoke(handle: Handle) throws{
        let configs = self.configs
        do {
            for config in configs {
                try config.callback.value(handle)
            }
        }catch let error {
            throw error
        }
    }
    
    func config(by name: String) -> Callback? {
        let configs = self.configs
        for config in configs {
            if config.name == name {
                return config.callback.value
            }
        }
        return nil
    }
}

extension Configs: Equatable {
    public static func == (lhs: Configs, rhs: Configs) -> Bool {
        return lhs.configs == rhs.configs
    }
    
    public static func != (lhs: Configs, rhs: Configs) -> Bool {
        return lhs.configs != rhs.configs
    }
}
