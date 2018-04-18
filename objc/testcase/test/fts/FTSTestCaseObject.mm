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

#import "FTSTestCaseObject.h"
#import "FTSTestCaseObject+WCTTableCoding.h"
#import <WCDB/WCDB.h>

@implementation FTSTestCaseObject

WCDB_IMPLEMENTATION(FTSTestCaseObject)
WCDB_SYNTHESIZE(FTSTestCaseObject, message)
WCDB_SYNTHESIZE(FTSTestCaseObject, extension)

WCDB_VIRTUAL_TABLE_MODULE_FTS3(FTSTestCaseObject)
WCDB_VIRTUAL_TABLE_ARGUMENT_TOKENIZE_WCDB(FTSTestCaseObject)

- (instancetype)initWithMessage:(NSString *)message
{
    return [self initWithMessage:message andExtension:nil];
}

- (instancetype)initWithMessage:(NSString *)message
                   andExtension:(NSString *)extension
{
    if (self = [super init]) {
        assert(message != nil);
        _message = message;
        _extension = extension;
    }
    return self;
}

- (BOOL)isEqualToObject:(FTSTestCaseObject *)object
{
    if (![self.message isEqualToString:object.message]) {
        return NO;
    }
    if (self.extension != nil) {
        return [self.extension isEqualToString:object.extension];
    }
    return object.extension == nil;
}

@end
