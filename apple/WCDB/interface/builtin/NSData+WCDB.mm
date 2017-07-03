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

#ifdef WCDB_BUILTIN_COLUMN_CODING

#import <Foundation/Foundation.h>
#import <WCDB/WCDB.h>
#import <WCDB/WCTCompatible.h>

@interface NSData (WCDB) <WCTColumnCoding>
@end

@implementation NSData (WCDB)

+ (instancetype)unarchiveWithWCTValue:(NSData *)value
{
    NSData *result = nil;
    if (value) {
        //Compatible for old builtin column coding before v1.0.1
        static BOOL s_compatible = [WCTCompatible sharedCompatible].isBuiltinNSDataColumnCodingCompatibleEnabled;
        if (!s_compatible) {
            result = value;
        } else {
            result = [NSKeyedUnarchiver unarchiveObjectWithData:value];
            if (!result) {
                result = value;
            }
        }
    }
    return result;
}

- (NSData *)archivedWCTValue
{
    return self;
}

+ (WCTColumnType)columnTypeForWCDB
{
    return WCTColumnTypeBinary;
}

@end

@interface NSMutableData (WCDB) <WCTColumnCoding>
@end

@implementation NSMutableData (WCDB)

+ (instancetype)unarchiveWithWCTValue:(NSData *)value
{
    NSData *result = nil;
    if (value) {
        //Compatible for old builtin column coding before v1.0.1
        static BOOL s_compatible = [WCTCompatible sharedCompatible].isBuiltinNSDataColumnCodingCompatibleEnabled;
        if (!s_compatible) {
            result = value;
        } else {
            result = [NSKeyedUnarchiver unarchiveObjectWithData:value];
            if (!result) {
                result = value;
            }
        }
    }
    return result ? [NSMutableData dataWithData:result] : nil;
}

- (NSData *)archivedWCTValue
{
    return self;
}

+ (WCTColumnType)columnTypeForWCDB
{
    return WCTColumnTypeBinary;
}

@end

#endif //WCDB_BUILTIN_COLUMN_CODING
