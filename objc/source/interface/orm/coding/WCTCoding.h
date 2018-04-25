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
#import <WCDB/NSData+NoCopyData.h>
#import <WCDB/NSString+CppString.h>
#import <WCDB/WCTCommon.h>
#import <WCDB/WCTProperty.h>

NS_ASSUME_NONNULL_BEGIN

typedef WCDB::Column (^WCTColumnNamed)(NSString *_Nonnull);

@protocol WCTTableCoding
@required
+ (const WCTBinding &)objectRelationalMapping;
+ (const WCTPropertyList &)allProperties;
+ (const WCDB::Expression::All &)allResults;
+ (WCTColumnNamed)columnNamed;
@optional
@property(nonatomic, assign) long long lastInsertedRowID;
@property(nonatomic, assign) BOOL isAutoIncrement;
@end

@protocol WCTColumnCoding
@required
+ (nullable instancetype)unarchiveWithWCTValue:(nullable id /* NSData*, NSString*, NSNumber*, nil */)value;
- (nullable id /* NSData*, NSString*, NSNumber*, nil */)archivedWCTValue;
+ (WCDB::ColumnType)columnType;
@end

NS_ASSUME_NONNULL_END
