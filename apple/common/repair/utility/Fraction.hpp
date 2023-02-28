//
// Created by sanhuazhang on 2018/06/21
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

#pragma once

#include <cstdint>

namespace WCDB {

namespace Repair {

class Fraction final {
public:
    Fraction();
    Fraction(int64_t numerator, int64_t denominator);
    Fraction(int64_t value);

    Fraction operator+(const Fraction &operand) const;
    Fraction operator*(const Fraction &operand) const;
    Fraction operator/(const Fraction &operand) const;
    Fraction &operator+=(const Fraction &fraction);

    bool operator<(const Fraction &operand) const;

    double value() const;

    int64_t numberator() const;
    int64_t denominator() const;

protected:
    void reduce();
    int64_t euclidean(int64_t a, int64_t b) const;

private:
    int64_t m_numerator;
    int64_t m_denominator;
};

} //namespace Repair

} //namespace WCDB
