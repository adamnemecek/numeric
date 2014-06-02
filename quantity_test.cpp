/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2014 André Müller
 *
 *****************************************************************************/

#ifdef AM_USE_TESTS

#include <stdexcept>
#include <cstdint>
#include <iostream>

#include "quantity.h"
#include "quantity_test.h"


namespace am {
namespace num {
namespace test {


//-------------------------------------------------------------------
void quantity_init_correctness()
{
    auto c1 = quantity<std::int_least8_t>{std::int_least8_t(0)};
    auto c2 = quantity<std::int_least16_t>{std::int_least16_t(1)};
    auto c3 = quantity<std::int_least32_t>{5};
    auto c4 = quantity<std::int_least64_t>{8};
    auto c5 = quantity<std::intmax_t>{123};
    auto c6 = quantity<long long int>{12345678};

    if ((intmax_t(c1) != 0) ||
        (intmax_t(c2) != 1) ||
        (intmax_t(c3) != 5) ||
        (intmax_t(c4) != 8) ||
        (intmax_t(c5) != 123) ||
        (intmax_t(c6) != 12345678) )
    {
        throw std::logic_error("am::quantity init");
    }
}


//-------------------------------------------------------------------
void quantity_arithmetic_correctness()
{
    auto a = quantity<int>{0};
    auto b = quantity<int>{1};
    auto c = quantity<long long>{5};
    auto d = quantity<long long int>{10};

    auto e = quantity<int>::infinity();
    auto f = quantity<long long int>::infinity();

    if(!(
        ((a * a) == 0) &&     ((b * a) == 0) &&     ((c * a) == 0) &&
        ((a * b) == 0) &&     ((b * b) == 1) &&     ((c * b) == 5) &&
        ((a * c) == 0) &&     ((b * c) == 5) &&     ((c * c) == 25) &&
        ((a * d) == 0) &&     ((b * d) == 10) &&    ((c * d) == 50) &&
        ((a * e) == 0) &&     isinf(b * e) &&       isinf(c * e) &&
        ((a * f) == 0) &&     isinf(b * f) &&       isinf(c * f) &&

        ((d * a) == 0) &&     ((e * a) == 0) &&      ((f * a) == 0) &&
        ((d * b) == 10) &&    isinf(e * b) &&        isinf(f * b) &&
        ((d * c) == 50) &&    isinf(e * c) &&        isinf(f * c) &&
        ((d * d) == 100) &&   isinf(e * d) &&        isinf(f * d) &&
        isinf(d * e) &&       isinf(e * e) &&        isinf(f * e) &&
        isinf(d * f) &&       isinf(e * f) &&        isinf(f * f) &&

        ((a - a) == 0) &&     ((b - a) == 1) &&      ((c - a) == 5) &&
        ((a - b) == 0) &&     ((b - b) == 0) &&      ((c - b) == 4) &&
        ((a - c) == 0) &&     ((b - c) == 0) &&      ((c - c) == 0) &&
        ((a - d) == 0) &&     ((b - d) == 0) &&      ((c - d) == 0) &&
        ((a - e) == 0) &&     ((b - e) == 0) &&      ((c - e) == 0) &&
        ((a - f) == 0) &&     ((b - f) == 0) &&      ((c - f) == 0) &&

        ((d - a) == 10) &&    isinf(e - a) &&        isinf(f - a) &&
        ((d - b) == 9) &&     isinf(e - b) &&        isinf(f - b) &&
        ((d - c) == 5) &&     isinf(e - c) &&        isinf(f - c) &&
        ((d - d) == 0) &&     isinf(e - d) &&        isinf(f - d) &&
        ((d - e) == 0) &&     ((e - e) == 0) &&      ((f - e) == 0) &&
        ((d - f) == 0) &&     ((e - f) == 0) &&      ((f - f) == 0) &&

        ((a + a) == 0) &&     ((b + a) == 1) &&      ((c + a) == 5) &&
        ((a + b) == 1) &&     ((b + b) == 2) &&      ((c + b) == 6) &&
        ((a + c) == 5) &&     ((b + c) == 6) &&      ((c + c) == 10) &&
        ((a + d) == 10) &&    ((b + d) == 11) &&     ((c + d) == 15) &&
        isinf(a + e) &&       isinf(b + e) &&        isinf(c + e) &&
        isinf(a + f) &&       isinf(b + f) &&        isinf(c + f) &&

        ((d + a) == 10) &&    isinf(e + a) &&        isinf(f + a) &&
        ((d + b) == 11) &&    isinf(e + b) &&        isinf(f + b) &&
        ((d + c) == 15) &&    isinf(e + c) &&        isinf(f + c) &&
        ((d + d) == 20) &&    isinf(e + d) &&        isinf(f + d) &&
        isinf(d + e) &&       isinf(e + e) &&        isinf(f + e) &&
        isinf(d + f) &&       isinf(e + f) &&        isinf(f + f) &&

        true ))
    {
        throw std::logic_error("am::quantity arithmetic");
    }
}



//-------------------------------------------------------------------
void quantity_comparison_correctness()
{
    auto a = quantity<int>{0};
    auto b = quantity<int>{1};
    auto c = quantity<int>{5};
    auto d = quantity<int>::infinity();
    auto e = quantity<long long int>::infinity();

    //== != <= < > >=
    if(!(
        (a == a) && (b == b) && (c == c) && (d == d) && (e == e)
        &&
        (a != b) && (a != c) && (a != d) && (a != e) &&
        (b != a) && (b != c) && (b != d) && (b != e) &&
        (c != a) && (c != b) && (c != d) && (c != e) &&
        (d != a) && (d != b) && (d != c) && (d == e) &&
        (e != a) && (e != b) && (e != c) && (e == d)
        &&
        (a <  b) && (a <  c) && (a <  d) && (a <  e) &&
        (a <= a) && (a <= b) && (a <= c) && (a <= d) && (a <= e)
        &&
        (b <  c) && (b <  d) && (b <  e) &&
        (b <= b) && (b <= c) && (b <= d) && (b <= e)
        &&
        (c <  d) && (c <  e) &&
        (c <= c) && (c <= d) && (c <= e)
        &&
        (d <= d) && (d <= e)
        &&
        (e >  a) && (e >  b) && (e >  c) &&
        (e >= a) && (e >= b) && (e >= c) && (e >= d) && (e >= e)
        &&
        (d >  a) && (d >  b) && (d >  c) &&
        (d >= a) && (d >= b) && (d >= c) && (d >= d) && (d >= e)
        &&
        (c >  a) && (c >  b) &&
        (c >= a) && (c >= b) && (c >= c)
        &&
        (b >  a) &&
        (b >= a) && (b >= b)
        &&
        (a >= a)
    ))
    {
        throw std::logic_error("am::quantity comparison");
    }
}



//-------------------------------------------------------------------
void quantity_correctness()
{
    quantity_init_correctness();
    quantity_arithmetic_correctness();
    quantity_comparison_correctness();
}


}  // namespace test
}  // namespace num
}  // namespace am


#endif
