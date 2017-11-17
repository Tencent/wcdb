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

extension Date: CodableStructColumn {
    public typealias FundamentalType = Int64
    
    public init?(withTypedValue optionalValue: Int64?) {
        guard let value = optionalValue else {
            return nil
        }
        self.init(timeIntervalSince1970: TimeInterval(value))
    }
    
    public func archivedTypedValue() -> Int64? {
        return Int64(self.timeIntervalSince1970)
    }
}
