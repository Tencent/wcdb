//
// Created by qiuwenchen on 2022/12/4.
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

#if TEST_WCDB_OBJC
#import <WCDBOBjc/WCDBCpp.h>
#elif TEST_WCDB_CPP
#import <WCDBCpp/WCDBCpp.h>
#else
#import <WCDB/WCDBCpp.h>
#endif

class CPPNewFieldObject {
public:
    int primaryValue;
    int uniqueValue;
    int insertValue;
    int updateValue;
    int selectValue;
    int multiSelectValue;
    int deleteValue;
    int indexValue;
    WCDB_CPP_ORM_DECLARE_AND_START_IMPLEMENT(CPPNewFieldObject)
    WCDB_CPP_SYNTHESIZE(primaryValue)
    WCDB_CPP_SYNTHESIZE(uniqueValue)
    WCDB_CPP_SYNTHESIZE(insertValue)
    WCDB_CPP_SYNTHESIZE(updateValue)
    WCDB_CPP_SYNTHESIZE(selectValue)
    WCDB_CPP_SYNTHESIZE(multiSelectValue)
    WCDB_CPP_SYNTHESIZE(deleteValue)
    WCDB_CPP_PRIMARY(primaryValue)
    WCDB_CPP_UNIQUE(uniqueValue)
    WCDB_CPP_ORM_IMPLEMENTATION_END
};
