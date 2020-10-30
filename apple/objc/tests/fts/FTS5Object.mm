//
// Created by qiuwenchen(陈秋文) on 2020/10/29.
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

#import <Foundation/Foundation.h>
#import "FTS5Object.h"
#import "TestCase.h"

@implementation FTS5Object

WCDB_IMPLEMENTATION(FTS5Object)
WCDB_SYNTHESIZE(content)
WCDB_SYNTHESIZE(extension)

WCDB_VIRTUAL_TABLE_MODULE(WCTModuleFTS5)
WCDB_VIRTUAL_TABLE_TOKENIZE(WCTTokenizerOneWord_FTS5)

- (BOOL)isEqual:(NSObject*)object
{
    if (object.class != self.class) {
        return NO;
    }
    FTS5Object* other = (FTS5Object*) object;
    return [NSObject isObject:self.content nilEqualToObject:other.content] && [NSObject isObject:self.extension nilEqualToObject:other.extension];
}

@end
