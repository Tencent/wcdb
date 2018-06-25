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

#include <WCDB/Assertion.hpp>
#include <WCDB/Fraction.hpp>

namespace WCDB {

namespace Repair {

Fraction::Fraction() : m_numerator(0), m_denominator(1)
{
}

Fraction::Fraction(int64_t value) : m_numerator(value), m_denominator(1)
{
}

Fraction::Fraction(int64_t numerator, int64_t denominator)
    : m_numerator(numerator), m_denominator(denominator)
{
    WCTAssert(denominator != 0, "Denominator can't be 0.");
}

int64_t Fraction::euclidean(int64_t a, int64_t b)
{
    return b == 0 ? a : euclidean(b, a % b);
}

void Fraction::reduce()
{
    int64_t numerator = m_numerator;
    int64_t denominator = m_denominator;
    int64_t gcd = euclidean(m_numerator, m_denominator);
    if (gcd > 1) {
        m_numerator /= gcd;
        m_denominator /= gcd;
    }
    //    printf("%lld/%lld reduce to %lld/%lld\n", numerator, denominator, m_numerator, m_denominator);
}

Fraction Fraction::operator+(const Fraction &operand) const
{
    int64_t numerator, denominator;
    if (m_denominator == operand.m_denominator) {
        numerator = m_numerator + operand.m_numerator;
        denominator = m_denominator;
    } else {
        numerator = (m_numerator * operand.m_denominator) +
                    (operand.m_numerator * m_denominator);
        denominator = m_denominator * operand.m_denominator;
    }
    //    printf("%lld/%lld + %lld/%lld = %lld/%lld\n", m_numerator, m_denominator, operand.m_numerator, operand.m_denominator, numerator, denominator);
    Fraction result(numerator, denominator);
    result.reduce();
    return result;
}

Fraction Fraction::operator*(const Fraction &operand) const
{
    Fraction result(m_numerator * operand.m_numerator,
                    m_denominator * operand.m_denominator);
    //    printf("%lld/%lld * %lld/%lld = %lld/%lld\n", m_numerator, m_denominator, operand.m_numerator, operand.m_denominator, result.m_numerator, result.m_denominator);
    result.reduce();
    return result;
}

Fraction &Fraction::operator+=(const Fraction &operand)
{
    int64_t numerator = m_numerator;
    int64_t denominator = m_denominator;
    if (m_denominator == operand.m_denominator) {
        m_numerator += operand.m_numerator;
    } else {
        m_numerator = (m_numerator * operand.m_denominator) +
                      (operand.m_numerator * m_denominator);
        m_denominator *= operand.m_denominator;
    }
    //    printf("%lld/%lld + %lld/%lld = %lld/%lld\n", numerator, denominator, operand.m_numerator, operand.m_denominator, m_numerator, m_denominator);
    reduce();
    return *this;
}

Fraction Fraction::operator/(const Fraction &operand) const
{
    Fraction result(m_numerator * operand.m_denominator,
                    m_denominator * operand.m_numerator);
    //    printf("%lld/%lld / %lld/%lld = %lld/%lld\n", m_numerator, m_denominator, operand.m_numerator, operand.m_denominator, result.m_numerator, result.m_denominator);
    result.reduce();
    return result;
}

bool Fraction::operator<(const Fraction &operand) const
{
    return (m_numerator * operand.m_denominator) <
           (operand.m_numerator * m_denominator);
}

double Fraction::value() const
{
    return (double) m_numerator / m_denominator;
}

int64_t Fraction::numberator() const
{
    return m_numerator;
}

int64_t Fraction::denominator() const
{
    return m_denominator;
}

} //namespace Repair

} //namespace WCDB
