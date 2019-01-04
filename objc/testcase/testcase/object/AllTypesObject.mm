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

#import <TestCase/AllTypesObject+WCTTableCoding.h>
#import <TestCase/AllTypesObject.h>
#import <TestCase/NSObject+TestCase.h>
#import <WCDB/WCDB.h>

@implementation AllTypesObject

WCDB_IMPLEMENTATION(AllTypesObject)

WCDB_SYNTHESIZE(AllTypesObject, type)

WCDB_SYNTHESIZE(AllTypesObject, intValue)
WCDB_SYNTHESIZE(AllTypesObject, unsignedIntValue)
WCDB_SYNTHESIZE(AllTypesObject, int32Value)
WCDB_SYNTHESIZE(AllTypesObject, int64Value)
WCDB_SYNTHESIZE(AllTypesObject, uint32Value)
WCDB_SYNTHESIZE(AllTypesObject, uint64Value)
WCDB_SYNTHESIZE(AllTypesObject, integerValue)
WCDB_SYNTHESIZE(AllTypesObject, uintegerValue)

WCDB_SYNTHESIZE(AllTypesObject, floatValue)
WCDB_SYNTHESIZE(AllTypesObject, doubleValue)
WCDB_SYNTHESIZE(AllTypesObject, numberValue)
WCDB_SYNTHESIZE(AllTypesObject, dateValue)

WCDB_SYNTHESIZE(AllTypesObject, stringValue)

WCDB_SYNTHESIZE(AllTypesObject, dataValue)
WCDB_SYNTHESIZE(AllTypesObject, codingValue)

WCDB_PRIMARY(AllTypesObject, type)

+ (AllTypesObject *)maxObject
{
#define ASSIGN_WITH_MAX_VALUE(object, property) object.property = std::numeric_limits<decltype(object.property)>::max()

    AllTypesObject *object = [[AllTypesObject alloc] init];
    object.type = @"max";
    ASSIGN_WITH_MAX_VALUE(object, intValue);
    ASSIGN_WITH_MAX_VALUE(object, unsignedIntValue);
    ASSIGN_WITH_MAX_VALUE(object, int32Value);
    ASSIGN_WITH_MAX_VALUE(object, int64Value);
    ASSIGN_WITH_MAX_VALUE(object, uint32Value);
    ASSIGN_WITH_MAX_VALUE(object, uint64Value);
    ASSIGN_WITH_MAX_VALUE(object, integerValue);
    ASSIGN_WITH_MAX_VALUE(object, uintegerValue);

    ASSIGN_WITH_MAX_VALUE(object, floatValue);
    ASSIGN_WITH_MAX_VALUE(object, doubleValue);
    object.numberValue = [NSNumber numberWithDouble:std::numeric_limits<double>::max()];
    object.dateValue = [NSDate dateWithTimeIntervalSince1970:std::numeric_limits<double>::max()];

    object.stringValue = @"";

    object.dataValue = [NSData data];
    object.codingValue = [NSURL URLWithString:NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES)[0]];

    return object;
}

+ (AllTypesObject *)minObject
{
#define ASSIGN_WITH_MIN_VALUE(object, property) object.property = std::numeric_limits<decltype(object.property)>::min()

    AllTypesObject *object = [[AllTypesObject alloc] init];
    object.type = @"min";
    ASSIGN_WITH_MIN_VALUE(object, intValue);
    ASSIGN_WITH_MIN_VALUE(object, unsignedIntValue);
    ASSIGN_WITH_MIN_VALUE(object, int32Value);
    ASSIGN_WITH_MIN_VALUE(object, int64Value);
    ASSIGN_WITH_MIN_VALUE(object, uint32Value);
    ASSIGN_WITH_MIN_VALUE(object, uint64Value);
    ASSIGN_WITH_MIN_VALUE(object, integerValue);
    ASSIGN_WITH_MIN_VALUE(object, uintegerValue);

    ASSIGN_WITH_MIN_VALUE(object, floatValue);
    ASSIGN_WITH_MIN_VALUE(object, doubleValue);
    object.numberValue = [NSNumber numberWithDouble:std::numeric_limits<double>::min()];
    object.dateValue = [NSDate dateWithTimeIntervalSince1970:std::numeric_limits<double>::min()];

    object.stringValue = @"";

    object.dataValue = [NSData data];
    object.codingValue = [NSURL URLWithString:NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES)[0]];

    return object;
}

+ (AllTypesObject *)nilObject
{
    AllTypesObject *object = [[AllTypesObject alloc] init];
    object.type = @"nil";
    object.intValue = 0;
    object.unsignedIntValue = 0;
    object.int32Value = 0;
    object.int64Value = 0;
    object.uint32Value = 0;
    object.uint64Value = 0;
    object.integerValue = 0;
    object.uintegerValue = 0;

    object.floatValue = 0;
    object.doubleValue = 0;
    object.numberValue = nil;
    object.dateValue = nil;

    object.stringValue = nil;

    object.dataValue = nil;
    object.codingValue = nil;

    return object;
}

+ (AllTypesObject *)emptyObject
{
    AllTypesObject *object = [[AllTypesObject alloc] init];
    object.type = @"empty";
    object.intValue = 0;
    object.unsignedIntValue = 0;
    object.int32Value = 0;
    object.int64Value = 0;
    object.uint32Value = 0;
    object.uint64Value = 0;
    object.integerValue = 0;
    object.uintegerValue = 0;

    object.floatValue = 0;
    object.doubleValue = 0;
    object.numberValue = @(0);
    object.dateValue = [NSDate dateWithTimeIntervalSince1970:0];

    object.stringValue = @"";

    object.dataValue = [NSData data];
    object.codingValue = [NSURL URLWithString:@""];

    return object;
}

- (BOOL)isEqual:(NSObject *)object
{
    if (object.class != self.class) {
        return NO;
    }
    AllTypesObject *other = (AllTypesObject *) object;
    return self.intValue == other.intValue
           && self.unsignedIntValue == other.unsignedIntValue
           && self.int32Value == other.int32Value
           && self.int64Value == other.int64Value
           && self.uint32Value == other.uint32Value
           && self.uint64Value == other.uint64Value
           && self.integerValue == other.integerValue
           && self.uintegerValue == other.uintegerValue
           && self.floatValue == other.floatValue
           && self.doubleValue == other.doubleValue
           && [NSObject isObject:self.numberValue nilEqualToObject:other.numberValue]
           && [NSObject isObject:self.dateValue nilEqualToObject:other.dateValue]
           && [NSObject isObject:self.stringValue nilEqualToObject:other.stringValue]
           && [NSObject isObject:self.dataValue nilEqualToObject:other.dataValue]
           && [NSObject isObject:self.codingValue nilEqualToObject:other.codingValue];
}

@end
