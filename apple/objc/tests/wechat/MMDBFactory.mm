//
// Created by sanhuazhang on 2019/12/9.
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

#import "MMDBFactory.h"
#import "MMDBMainData.h"
#import <WCDB/WCDB.h>
#import "TestCaseAssertion.h"
#import "Random+MMDBMainData.h"

@implementation MMDBFactory {
    NSString* _tableName;
}

- (instancetype)initWithDirectory:(NSString*)directory
{
    if (self = [super initWithDirectory:directory]) {
        self.delegate = self;
    }
    return self;
}

- (NSString*)tableName
{
    return @"benchmark";
}

- (void)preparePrototype:(WCTDatabase*)prototype currentQuality:(double)quality
{
    int maxNumberOfObjects = (int) self.quality;
    int existingNumberOfObjects = (int) quality;
    int numberOfObjects = (int) (self.quality * 0.1);
    if (maxNumberOfObjects - existingNumberOfObjects < numberOfObjects) {
        numberOfObjects = maxNumberOfObjects - existingNumberOfObjects;
    }

    if (existingNumberOfObjects == 0) {
        TestCaseAssertTrue([prototype createTable:self.tableName withClass:DBMessage.class]);
    }

    NSArray* objects = [[Random shared] autoIncrementDBMessages:numberOfObjects];
    TestCaseAssertTrue([prototype insertObjects:objects intoTable:self.tableName]);
}

- (double)qualityOfPrototype:(WCTDatabase*)prototype
{
    if ([[NSFileManager defaultManager] fileExistsAtPath:prototype.path]) {
        WCTValue* count = [prototype getValueOnResultColumn:DBMessage.allProperties.count() fromTable:self.tableName];
        TestCaseAssertNotNil(count);
        return count.numberValue.intValue;
    }
    return 0;
}

- (NSString*)categoryOfPrototype
{
    return @"MMDB";
}

@end
