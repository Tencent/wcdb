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
import WCDB_Private

internal final class ValueWrap<T> {
    var value: T
    init(_ value: T) {
        self.value = value
    }
}

internal final class ObjectBridge {

    @discardableResult
    static func createRecyclableCPPObject<T>(_ obj: T) -> Recyclable<T> {
        return Recyclable<T>(obj, onRecycled: {
            let innerValue = unsafeBitCast(obj, to: UnsafeMutablePointer<CPPObject>.self)
            WCDBReleaseCPPObject(innerValue)
        })
    }

    static func extractTypedSwiftObject<T>(_ obj: UnsafeMutableRawPointer?) -> T? {
        guard let obj = obj else { return nil}
        return Unmanaged<AnyObject>.fromOpaque(obj).takeUnretainedValue() as? T
    }

    static func getUntypeSwiftObject(_ obj: AnyObject) -> UnsafeMutableRawPointer {
        return Unmanaged<AnyObject>.passRetained(obj).toOpaque()
    }

    static let objectDestructor: @convention(c) (UnsafeMutableRawPointer) -> Void = { obj in
        Unmanaged<AnyObject>.fromOpaque(obj).release()
    }

    static func releaseSwiftObject(_ obj: UnsafeMutableRawPointer) {
        Unmanaged<AnyObject>.fromOpaque(obj).release()
    }

    static func initializeCPPAPI() {
        WCDBSetReleaseSwiftObjectFunction(objectDestructor)
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
