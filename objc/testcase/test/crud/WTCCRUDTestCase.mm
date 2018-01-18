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

#import "WTCCRUDTestCase.h"

@implementation WTCCRUDTestCase

- (instancetype)init
{
    if (self = [super init]) {
        NSMutableArray<WTCCRUDObject*> *preInsertedObjects = [[NSMutableArray<WTCCRUDObject*> alloc] init];
        WTCCRUDObject* object1 = [[WTCCRUDObject alloc] init];
        object1.variable1 = 1;
        object1.variable2 = @"object1";
        [preInsertedObjects addObject:object1];
        WTCCRUDObject* object2 = [[WTCCRUDObject alloc] init];
        object2.variable1 = 2;
        object2.variable2 = @"object2";
        [preInsertedObjects addObject:object2];
        _preInsertedObjects = preInsertedObjects;
    }
    return self;
}

- (void)setUp
{
    [super setUp];
    _database = [[WCTDatabase alloc] initWithPath:self.recommendedPath];
    _database.tag = self.recommendedTag;
    
    XCTAssertTrue([_database createTableAndIndexesOfName:WTCCRUDObject.Name withClass:WTCCRUDObject.class]);
    
    XCTAssertTrue([_database insertObjects:self.preInsertedObjects into:WTCCRUDObject.Name]);
}

@end
