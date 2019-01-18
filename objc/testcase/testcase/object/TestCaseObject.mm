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

#import <TestCase/NSObject+TestCase.h>
#import <TestCase/TestCaseObject+WCTTableCoding.h>
#import <TestCase/TestCaseObject.h>
#import <WCDB/WCDB.h>

@implementation TestCaseObject

WCDB_IMPLEMENTATION(TestCaseObject)
WCDB_SYNTHESIZE(TestCaseObject, identifier)
WCDB_SYNTHESIZE(TestCaseObject, content)

WCDB_PRIMARY_ASC_AUTO_INCREMENT(TestCaseObject, identifier)

- (BOOL)isEqual:(NSObject*)object
{
    if (object.class != self.class) {
        return NO;
    }
    TestCaseObject* other = (TestCaseObject*) object;
    return self.identifier == other.identifier && [NSObject isObject:self.content nilEqualToObject:other.content];
}

- (NSUInteger)hash
{
    NSMutableData* data = [NSMutableData data];
    [data appendBytes:&(_identifier) length:sizeof(_identifier)];
    [data appendData:[_content dataUsingEncoding:NSUTF8StringEncoding]];
    return data.hash;
}

+ (instancetype)objectWithIdentifier:(int)identifier andContent:(NSString*)content
{
    TestCaseObject* object = [[TestCaseObject alloc] init];
    object.identifier = identifier;
    object.content = content;
    return object;
}

+ (instancetype)partialObjectWithIdentifier:(int)identifier
{
    return [self objectWithIdentifier:identifier andContent:nil];
}

+ (instancetype)autoIncrementObjectWithContent:(NSString*)content
{
    TestCaseObject* object = [self objectWithIdentifier:0 andContent:content];
    object.isAutoIncrement = YES;
    return object;
}

@end
