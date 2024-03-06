//
// Created by qiuwenchen on 2022/4/2.
//

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
import WCDB_Private

internal final class ErrorBridge {
    static func getErrorFrom(cppError error: CPPError) -> WCDBError {
        let recycleError = ObjectBridge.createRecyclableCPPObject(error)
        let level = WCDBError.Level(rawValue: Int(WCDBErrorGetLevel(recycleError.raw)))
        let code = WCDBError.Code(rawValue: WCDBErrorGetCode(recycleError.raw))
        let message = String(cString: WCDBErrorGetMsg(recycleError.raw))
        let valueWrap: ValueWrap<[String: Value]> = ValueWrap([:])
        let valueWrapPointer = ObjectBridge.getUntypeSwiftObject(valueWrap)

        let enumerator: @convention(c) (UnsafeMutableRawPointer, UnsafePointer<CChar>, CPPCommonValue) -> Void = {
            valueWrapPointer, key, value in
            let valueWrap: ValueWrap<[String: Value]>? = ObjectBridge.extractTypedSwiftObject(valueWrapPointer)
            guard let valueWrap = valueWrap else {
                return
            }
            switch value.type {
            case WCDBBridgedType_Int:
                valueWrap.value[String(cString: key)] = Value(value.intValue)
            case WCDBBridgedType_Double:
                valueWrap.value[String(cString: key)] = Value(value.doubleValue)
            case WCDBBridgedType_String:
                valueWrap.value[String(cString: key)] = Value(String(cString: unsafeBitCast(value.intValue, to: UnsafePointer<CChar>.self)))
            default:
                return
            }
        }
        WCDBErrorEnumerateAllInfo(error, valueWrapPointer, enumerator)

        var infos = WCDBError.Infos()
        var extInfos = WCDBError.ExtInfos()
        for (key, value) in valueWrap.value {
            let errorKey = WCDBError.Key(stringKey: key)
            if errorKey != .invalidKey {
                infos[errorKey] = value
            } else {
                extInfos[key] = value
            }
        }
        ObjectBridge.releaseSwiftObject(valueWrapPointer)
        infos[WCDBError.Key.message] = Value(message)
        return WCDBError(level: level ?? .Error, code: code ?? .Error, infos: infos, extInfos: extInfos)
    }
}
