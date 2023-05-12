//
// Created by 陈秋文 on 2022/12/4.
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

#import "NewPropertyObject.h"
#import <Foundation/Foundation.h>

@implementation NewPropertyObject

WCDB_IMPLEMENTATION(NewPropertyObject)
WCDB_SYNTHESIZE(primaryValue)
WCDB_SYNTHESIZE(uniqueValue)
WCDB_SYNTHESIZE(insertValue)
WCDB_SYNTHESIZE(updateValue)
WCDB_SYNTHESIZE(selectValue)
WCDB_SYNTHESIZE(multiSelectValue)
WCDB_SYNTHESIZE(deleteValue)

WCDB_PRIMARY(primaryValue)
WCDB_UNIQUE(uniqueValue)

@end
