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

class DeviceInfo: CustomDebugStringConvertible {    
    static let current = DeviceInfo()
    
    private func sysctl(forKey key: String) -> String? {
        var keyArray = [UInt8](key.utf8).map{ return Int8(bitPattern: $0) }
        var length = 0
        sysctlbyname(&keyArray, nil, &length, nil, 0)
        guard length > 0 else{
            return nil
        }
        var result = UnsafeMutableRawPointer.allocate(bytes: length, alignedTo: length); defer { result.deallocate(bytes: length, alignedTo: length) }
        sysctlbyname(&keyArray, result, &length, nil, 0)
        return String(data: Data(bytes: result, count: length), encoding: .utf8)
    }
    
    private func sysctl<T: Numeric>(forKey key: String) -> T? {
        var keyArray = [UInt8](key.utf8).map{ return Int8(bitPattern: $0) }
        var length = 0
        sysctlbyname(&keyArray, nil, &length, nil, 0)
        guard length > 0 else{
            return nil
        }
        var result = UnsafeMutableRawPointer.allocate(bytes: length, alignedTo: length); defer { result.deallocate(bytes: length, alignedTo: length) }
        sysctlbyname(&keyArray, result, &length, nil, 0)
        return result.assumingMemoryBound(to: T.self).pointee
    }
    
    lazy var model: String = {
        let optionalModel: String? = sysctl(forKey: "hw.model")
        return optionalModel ?? "unknown" 
    }()
    
    lazy var osVersion: String = ProcessInfo.processInfo.operatingSystemVersionString
    
    lazy var cpuFrequency: Float = {
        let optionalCpufrequency: Int64? = sysctl(forKey: "hw.cpufrequency")
        guard let cpuFrequency = optionalCpufrequency else {
            return 0
        }
        return Float(Double(cpuFrequency) /  1000000000)
    }() 
    
    lazy var numberOfCores: Int = {
        let optionalNumberOfCores: Int32? = sysctl(forKey: "hw.ncpu")
        guard let numberOfCores = optionalNumberOfCores else {
            return 0
        } 
        return Int(numberOfCores)
    }()
    
    lazy var physicalMemory: Int = Int(ProcessInfo.processInfo.physicalMemory / 1024 / 1024)
    
    lazy var isBeingDebugged: Bool = {
        #if DEBUG
            return true
        #else 
            var mid: [Int32] = [CTL_KERN, KERN_PROC, KERN_PROC_PID, getpid()]
            var info = kinfo_proc()
            info.kp_proc.p_flag = 0
            var size = MemoryLayout.size(ofValue: info)
            let junk = Darwin.sysctl(&mid, 4, &info, &size, nil, 0)
            assert(junk == 0)
            return (info.kp_proc.p_flag & P_TRACED) != 0
        #endif
    }()

    var debugDescription: String {
        return """
        Model: \(model)
        OS Version: \(osVersion)
        CPU Frequency: \(cpuFrequency) GHz
        Number of cores: \(numberOfCores)
        Phsical Memory: \(physicalMemory) MB
        Is In Debug Mode: \(isBeingDebugged)
        """
    }

}
