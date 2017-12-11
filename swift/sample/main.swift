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

import Foundation
import WCDBSwift

let machPath = CommandLine.arguments.first!
let baseDirectory = URL(fileURLWithPath: machPath).deletingLastPathComponent().appendingPathComponent("SampleDB").path
print("Base Directory: \(baseDirectory)")
sample_orm_main(baseDirectory: baseDirectory)
sample_chailcall_main(baseDirectory: baseDirectory)
sample_convenient_main(baseDirectory: baseDirectory)
sample_transaction_main(baseDirectory: baseDirectory)
sample_advance_main(baseDirectory: baseDirectory)
sample_core_main(baseDirectory: baseDirectory)
sample_file_main(baseDirectory: baseDirectory)
sample_repair_main(baseDirectory: baseDirectory)
sample_statistics_main(baseDirectory: baseDirectory)
sample_fts_main(baseDirectory: baseDirectory)
