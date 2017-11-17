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

extension String: TableOrSubqueryConvertible{
    public func asTableOrSubquery() -> Subquery {
        return Subquery(withTable: self)
    }
}

//Path
extension String {
    var lastPathComponent: String {
        return URL(fileURLWithPath: self).lastPathComponent
    }

    func stringByAppending(pathComponent: String) -> String {
        return URL(fileURLWithPath: self).appendingPathComponent(pathComponent).path
    }
}

extension String {
    var cString: UnsafePointer<Int8>? {
        return UnsafePointer<Int8>((self as NSString).utf8String)
    }
    
    func substring(with range: CFRange) -> String {
        return String(self[self.range(from: range.location, to: range.location+range.length)])
    }
    
    init?(bytes: UnsafeRawPointer, count: Int, encoding: String.Encoding) {
        self.init(data: Data(bytes: bytes, count: count), encoding: encoding)
    }
    
    func range(from: Int, to: Int) -> Range<String.Index> {
        return self.index(self.startIndex, offsetBy: from)..<self.index(self.startIndex, offsetBy: to)
    }
}
