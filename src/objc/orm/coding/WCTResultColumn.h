//
// Created by sanhuazhang on 2019/05/02
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

#import "WCTColumnBinding.h"
#import "WCTCommon.h"

class WCDB_API WCTResultColumn final : public WCDB::ResultColumn, public WCTColumnBindingHolder {
public:
    WCTResultColumn();
    WCTResultColumn(const WCTProperty& property);
    WCTResultColumn(const WCDB::ResultColumn& resultColumn,
                    const WCTColumnBinding& columnBinding);
    ~WCTResultColumn() override;
};

namespace WCDB {

template<>
class WCDB_API SyntaxList<WCTResultColumn> final : public _SyntaxList<WCTResultColumn> {
public:
    using _SyntaxList<WCTResultColumn>::_SyntaxList;
    ~SyntaxList() override;

    WCTResultColumns
    resultColumnsByAddingNewResultColumns(const WCTResultColumns& resultColumns) const;
    WCTResultColumns& addingNewResultColumns(const WCTResultColumns& resultColumns);

    WCTResultColumns
    resultColumnsByRemovingResultColumns(const WCTResultColumns& resultColumns) const;
    WCTResultColumns& removingResultColumns(const WCTResultColumns& resultColumns);

private:
    static bool isEqual(const WCTResultColumn& left, const WCTResultColumn& right);
};

} // namespace WCDB
