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

#import <WCDB/WCTConvenient.h>
#import <WCDB/WCTDatabase.h>

@interface WCTDatabase (ConvenientSelect) <WCTConvenientSelect>

@end

@interface WCTDatabase (ConvenientPartialSelect) <WCTConvenientPartialSelect>

@end

@interface WCTDatabase (ConvenientRowSelect) <WCTConvenientRowSelect>

@end

@interface WCTDatabase (ConvenientInsert) <WCTConvenientInsert>

@end

@interface WCTDatabase (
ConvenientInsertOrReplace) <WCTConvenientInsertOrReplace>

@end

@interface WCTDatabase (ConvenientPartialInsert) <WCTConvenientPartialInsert>

@end

@interface WCTDatabase (
ConvenientPartialInsertOrReplace) <WCTConvenientPartialInsertOrReplace>

@end

@interface WCTDatabase (ConvenientUpdate) <WCTConvenientUpdate>

@end

@interface WCTDatabase (ConvenientRowUpdate) <WCTConvenientRowUpdate>

@end

@interface WCTDatabase (ConvenientDelete) <WCTConvenientDelete>

@end

@interface WCTDatabase (ConvenientStatement) <WCTConvenientStatement>

@end
