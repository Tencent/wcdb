//
// Created by qiuwenchen on 2022/3/31.
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
import ObjectiveC

internal final class ObjectBridge {

    @discardableResult
    static func createRecyclableCPPObject<T>(_ obj: T) -> Recyclable<T> {
        return Recyclable<T>(obj, onRecycled: {
            let innerValue = unsafeBitCast(obj, to: UnsafeMutablePointer<CPPObject>.self)
            WCDBReleaseCPPObject(innerValue)
        })
    }

    static func extractTypedSwiftObject<T>(_ obj: OpaquePointer?) -> T? {
        guard let obj = obj else { return nil}
        return Unmanaged<AnyObject>.fromOpaque(UnsafeRawPointer(obj)).takeUnretainedValue() as? T
    }

    static func getUntypeSwiftObject(_ obj: AnyObject) -> OpaquePointer {
        return OpaquePointer(Unmanaged<AnyObject>.passRetained(obj).toOpaque())
    }

    static func initializeCPPAPI() {
        WCDBReleaseSwiftObject = { (obj: OpaquePointer) -> Void in
            Unmanaged<AnyObject>.fromOpaque(UnsafeRawPointer(obj)).release()
        }
        WCDBReleaseSwiftClosure = { (closure: IMP) -> Void in
            imp_removeBlock(closure)
        }
    }

    static func extendLifetime<Result>(_ objList: Any..., execute body: @escaping () -> Result) -> Result {
        extendLifetime(objList, execute: body)
    }

    static func extendLifetime<Result>(_ objList: [Any], execute body: @escaping () -> Result) -> Result {
        var execution: () -> Result = body
        for oneObj in objList {
            execution = { [execution] in
                return withExtendedLifetime(oneObj, execution)
            }
        }
        return execution()
    }
}
