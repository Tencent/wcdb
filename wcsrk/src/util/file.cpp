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

#include "file.hpp"

File::File(const std::string& p, Mode mode)
: path(p)
{
    const char* fmode;
    switch (mode) {
        case Mode::ReadOnly:
            fmode = "rb";
            break;           
        default:
            break;
    }
    m_file = fopen(p.c_str(), fmode);
}

bool File::isOpened() const
{
    return m_file!=nullptr;
}

bool File::read(unsigned char* data, const off_t& offset, const size_t& size)
{
    if (fseek(m_file, offset, SEEK_SET)!=0) {
        return false;
    }
    return fread(data, size, 1, m_file)==1;
}

size_t File::getSize()
{
    if (fseek(m_file, 0, SEEK_END)==0) {
        return ftell(m_file);
    }
    return 0;
}

void File::close()
{
    if (m_file) {
        fclose(m_file);
        m_file = nullptr;
    }
}

File::~File()
{
    close();
}
