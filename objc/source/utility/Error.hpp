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
#include <string>

namespace WCDB {

class Error {
public:
    Error();

    static constexpr int type = 1;
    virtual int getType() const;

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

    enum class Code {
        Error = -1,
    };
    int code;
    bool isOK() const;

    std::string message;

    virtual std::string getDescription() const;

protected:
    static void log(const std::string &message, Level level);
    void addToDescription(std::string &description,
                          const char *key,
                          int64_t value) const;
    void addToDescription(std::string &description,
                          const char *key,
                          const std::string &value) const;
};

} //namespace WCDB

#endif /* Error_hpp */
