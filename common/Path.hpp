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

#pragma once

#include <WCDB/String.hpp>

// TODO std::filesystem is available since C++17 + Clang with Xcode 11.

namespace WCDB {

namespace Path {

String addExtention(const String &base, const String &extention);
String addComponent(const String &base, const String &component);
String getFileName(const String &base);
String getDirectoryName(const String &base);

String normalize(const String &path);

} //namespace Path

} //namespace WCDB
