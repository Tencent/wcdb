//
// Created by qiuwenchen(陈秋文) on 2020/10/29.
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

#import <WCDB/WCDB.h>

@interface FTS5Object : NSObject <WCTTableCoding>

@property (nonatomic, retain) NSString *content;
@property (nonatomic, retain) NSString *extension;
WCDB_PROPERTY(content)
WCDB_PROPERTY(extension)

@end

@interface FTS5PinyinObject : NSObject <WCTTableCoding>

@property (nonatomic, retain) NSString *content;
WCDB_PROPERTY(content)

@end

@interface FTS5SymbolObject : NSObject <WCTTableCoding>

@property (nonatomic, retain) NSString *content;
WCDB_PROPERTY(content)

@end
