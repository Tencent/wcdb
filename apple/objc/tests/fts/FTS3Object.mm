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

#import "FTS3Object.h"
#import "FTS3Object+WCTTableCoding.h"
#import "TestCase.h"
#import <WCDB/WCDB.h>

@implementation FTS3Object

WCDB_IMPLEMENTATION(FTS3Object)
WCDB_SYNTHESIZE(content)
WCDB_SYNTHESIZE(extension)

WCDB_VIRTUAL_TABLE_MODULE(WCTModuleFTS3)
WCDB_VIRTUAL_TABLE_TOKENIZE(WCTTokenizerOneOrBinary)

- (BOOL)isEqual:(NSObject*)object
{
    if (object.class != self.class) {
        return NO;
    }
    FTS3Object* other = (FTS3Object*) object;
    return [NSObject isObject:self.content nilEqualToObject:other.content] && [NSObject isObject:self.extension nilEqualToObject:other.extension];
}

@end

@implementation FTS3SymbolObject

WCDB_IMPLEMENTATION(FTS3SymbolObject)
WCDB_SYNTHESIZE(content)

WCDB_VIRTUAL_TABLE_MODULE(WCTModuleFTS3)
WCDB_VIRTUAL_TABLE_TOKENIZE_WITH_PARAMETERS(WCTTokenizerOneOrBinary, WCTTokenizerParameter_NeedSymbol, WCTTokenizerParameter_OneWord)

- (BOOL)isEqual:(NSObject*)object
{
    if (object.class != self.class) {
        return NO;
    }
    FTS3SymbolObject* other = (FTS3SymbolObject*) object;
    return [NSObject isObject:self.content nilEqualToObject:other.content];
}

@end
