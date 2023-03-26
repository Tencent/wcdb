//
// Created by sanhuazhang on 2019/07/05
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

#import "SizeBasedFactory.h"
#import "NSObject+TestCase.h"
#import "Random+TestCaseObject.h"
#import "Random+WCDB.h"
#import "Random.h"
#import "TestCaseAssertion.h"
#import "TestCaseObject+WCTTableCoding.h"
#import "TestCaseObject.h"
#import "WCTDatabase+TestCase.h"
#if TEST_WCDB_OBJC
#import <WCDBOBjc/WCDBObjc.h>
#elif TEST_WCDB_CPP
#import <WCDBCpp/WCDBCpp.h>
#else
#import <WCDB/WCDBObjc.h>
#endif

@implementation SizeBasedFactory

- (instancetype)initWithDirectory:(NSString*)directory
{
    if (self = [super initWithDirectory:directory]) {
        self.delegate = self;
    }
    return self;
}

- (void)preparePrototype:(WCTDatabase*)prototype currentQuality:(double)quality
{
    WCDB_UNUSED(quality);

    int numberOfObjects = (int) (self.quality * self.tolerance / 4096.0 * 10);
    if (numberOfObjects <= 0) {
        numberOfObjects = 1;
    }

    NSArray* objects = [Random.shared testCaseObjectsWithCount:numberOfObjects startingFromIdentifier:0];
    NSString* tableName = Random.shared.tableName;
    TestCaseAssertTrue([prototype runTransaction:^BOOL(WCTHandle* handle) {
        return [handle createTable:tableName withClass:TestCaseObject.class]
               && [handle insertObjects:objects intoTable:tableName];
    }]);
}

- (double)qualityOfPrototype:(WCTDatabase*)prototype
{
    NSFileManager* fileManager = [NSFileManager defaultManager];
    if ([fileManager fileExistsAtPath:prototype.walPath]) {
        TestCaseAssertTrue([prototype truncateCheckpoint]);
    }
    return [fileManager getFileSizeIfExists:prototype.path];
}

- (NSString*)categoryOfPrototype
{
    return @"size";
}

@end
