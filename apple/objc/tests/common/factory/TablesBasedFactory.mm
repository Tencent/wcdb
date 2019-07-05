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

#import "TablesBasedFactory.h"
#import "Random+TestCaseObject.h"
#import "Random+WCDB.h"
#import "Random.h"
#import "TestCaseAssertion.h"
#import "TestCaseObject.h"
#import <WCDB/WCDB.h>

@implementation TablesBasedFactory

- (instancetype)initWithDirectory:(NSString*)directory
{
    if (self = [super initWithDirectory:directory]) {
        self.delegate = self;
    }
    return self;
}

- (void)preparePrototype:(WCTDatabase*)prototype currentQuality:(double)quality
{
    int maxNumberOfTables = (int) self.quality;
    int existingNumberOfTables = (int) quality;
    int numberOfTables = (int) (self.quality * 0.05);
    if (maxNumberOfTables - existingNumberOfTables < numberOfTables) {
        numberOfTables = maxNumberOfTables - existingNumberOfTables;
    }

    for (int i = 0; i < numberOfTables; ++i) {
        TestCaseAssertTrue([prototype createTable:Random.shared.tableName withClass:TestCaseObject.class]);
    }
}

- (double)qualityOfPrototype:(WCTDatabase*)prototype
{
    if ([[NSFileManager defaultManager] fileExistsAtPath:prototype.path]) {
        NSString* pattern = [NSString stringWithFormat:@"%s%%", WCDB::Syntax::builtinTablePrefix];

        WCTValue* count = [prototype getValueOnResultColumn:WCTMaster.allProperties.count() fromTable:WCTMaster.tableName where:WCTMaster.name.notLike(pattern)];
        TestCaseAssertNotNil(count);
        return count.numberValue.intValue;
    }
    return 0;
}

- (NSString*)categoryOfPrototype
{
    return @"tables";
}

@end
