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

//type
extern const NSString *WCTBenchmarkTypeBaselineRead;
extern const NSString *WCTBenchmarkTypeBaselineWrite;
extern const NSString *WCTBenchmarkTypeBaselineBatchWrite;

extern const NSString *WCTBenchmarkTypeMultithreadReadRead;
extern const NSString *WCTBenchmarkTypeMultithreadReadWrite;
extern const NSString *WCTBenchmarkTypeMultithreadWriteWrite;

extern const NSString *WCTBenchmarkTypeSyncWrite;

extern const NSString *WCTBenchmarkTypeCipherRead;
extern const NSString *WCTBenchmarkTypeCipherWrite;

extern const NSString *WCTBenchmarkTypeTracerRead;
extern const NSString *WCTBenchmarkTypeTracerWrite;

extern const NSString *WCTBenchmarkTypeInitialization;

//database type
extern const NSString *WCTBenchmarkDatabaseWCDB;

extern const NSString *WCTBenchmarkDatabaseFMDB;
