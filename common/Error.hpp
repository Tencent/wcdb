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

#ifndef Error_hpp
#define Error_hpp

#include <functional>
#include <map>
#include <sstream>
#include <string>

namespace WCDB {

class Error {
#pragma mark - Level
public:
    enum class Level : int {
        Ignore = 1,
        Debug = 2,
        Warning = 3,
        Error = 4,
        Fatal = 5,
    };
    Level level;
    static constexpr const char *LevelName(const Level &level)
    {
        switch (level) {
            case Level::Ignore:
                return "IGNORE";
            case Level::Debug:
                return "DEBUG";
            case Level::Warning:
                return "WARNING";
            case Level::Error:
                return "ERROR";
            case Level::Fatal:
                return "FATAL";
        }
    }

#pragma mark - Initialize
public:
    Error();
    Error(const std::string &message, Level level = Level::Error);

    void clear();

#pragma mark - Code
public:
    int code;
    bool isOK() const;

    static constexpr const int error = std::numeric_limits<int>::max();

#pragma mark - Type
public:
    std::string type;

#pragma mark - Info
public:
    std::string message;

    class Infos {
    public:
        void set(const std::string &key, int64_t value);
        void set(const std::string &key, const std::string &value);

        const std::map<std::string, int64_t> getIntInfos() const;
        const std::map<std::string, std::string> getStringInfos() const;

        void clear();

    protected:
        std::map<std::string, int64_t> m_ints;
        std::map<std::string, std::string> m_strings;
    };
    Infos infos;

#pragma mark - Description
    std::string getDescription() const;
};

} //namespace WCDB

#endif /* Error_hpp */
