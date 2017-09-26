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

#import "WCTSampleConvenient.h"
#import "WCTSampleConvenient+WCTTableCoding.h"
#import <WCDB/WCDB.h>

@implementation WCTSampleConvenient

WCDB_IMPLEMENTATION(WCTSampleConvenient)
WCDB_SYNTHESIZE(WCTSampleConvenient, intValue)
WCDB_SYNTHESIZE(WCTSampleConvenient, stringValue)

WCDB_PRIMARY_AUTO_INCREMENT(WCTSampleConvenient, intValue)

@end
