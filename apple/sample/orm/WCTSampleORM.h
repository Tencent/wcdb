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
#import <WCDB/WCDB.h>

typedef NS_ENUM(NSUInteger, WCTSampleORMType) {
    WCTSampleORMType1,
    WCTSampleORMType2,
};

@interface WCTSampleORM : NSObject <WCTTableCoding>

@property(nonatomic, assign) int identifier;
@property(nonatomic, retain) NSString *desc;
@property(nonatomic, assign) float value;
@property(nonatomic, retain) NSString *timestamp;
@property(nonatomic, assign) WCTSampleORMType type;

WCDB_PROPERTY(identifier)
WCDB_PROPERTY(desc)
WCDB_PROPERTY(value)
WCDB_PROPERTY(timestamp)
WCDB_PROPERTY(type)

@end
