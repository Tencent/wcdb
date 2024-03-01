// Created by qiuwenchen on 2023/6/25.
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

package com.tencent.wcdbtest.orm.testclass

import com.tencent.wcdb.WCDBField
import com.tencent.wcdb.WCDBTableCoding

@WCDBTableCoding
class AutoAddColumnObject {
    @WCDBField(isPrimary = true)
    var primaryValue = 0

    @WCDBField(isUnique = true)
    var uniqueValue = 0

    @WCDBField
    var insertValue = 0

    @WCDBField
    var updateValue = 0

    @WCDBField
    var selectValue = 0

    @WCDBField
    var multiSelectValue = 0

    @WCDBField
    var deleteValue = 0

    @WCDBField
    var indexValue = 0
}