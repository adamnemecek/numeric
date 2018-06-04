/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/

#pragma once

#include <complex>
#include <type_traits>
#include <iterator>
#include <utility>

#include "traits.h"
#include "limits.h"


namespace am {
namespace num {


/*****************************************************************************
 *
 * NUMERICAL PRECISION HELPER FUNCTIONS
 *
 *****************************************************************************/
template<class T1, class T2>
inline constexpr bool
approx_equal(const T1& a, const T2& b)
{
    return (
        (tolerance<T1> < tolerance<T2>)
            ? ((a >= (b - tolerance<T2>)) &&
               (a <= (b + tolerance<T2>)) )
            : ((b >= (a - tolerance<T1>)) &&
               (b <= (a + tolerance<T1>)) )
        );
}

//---------------------------------------------------------
template<class T1, class T2, class T3>
inline constexpr bool
approx_equal(const T1& a, const T2& b, const T3& tolerance)
{
    return ((a >= (b - tolerance)) &&
            (a <= (b + tolerance)) );
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
approx_equal(
    const std::complex<T1>& a,
    const std::complex<T2>& b)
{
    return (
        approx_equal(a.real(), b.real()) &&
        approx_equal(a.imag(), b.imag()) );
}

//---------------------------------------------------------
template<class T1, class T2, class T3>
inline constexpr bool
approx_equal(
    const std::complex<T1>& a,
    const std::complex<T2>& b,
    const T3& tolerance)
{
    return (
        approx_equal(a.real(), b.real(), tolerance) &&
        approx_equal(a.imag(), b.imag(), tolerance) );
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
abs_approx_equal(const T1& a, const T2& b)
{
    using std::abs;
    return approx_equal(abs(a), abs(b));
}

//---------------------------------------------------------
template<class T1, class T2, class T3>
inline constexpr bool
abs_approx_equal(const T1& a, const T2& b, const T3& tolerance)
{
    using std::abs;
    return approx_equal(abs(a), abs(b), tolerance);
}



//-------------------------------------------------------------------
template<class T>
inline constexpr bool
approx_0(const T& a, const T& tol = tolerance<T>)
{
    return (
        (a >= (T(0) - tol)) &&
        (a <= (T(0) + tol)) );
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
approx_0(const std::complex<T>& a, const T& tol = tolerance<T>)
{
    return (
        approx_0(a.real(), tol) &&
        approx_0(a.imag(), tol) );
}




//-------------------------------------------------------------------
template<class T>
inline constexpr bool
approx_1(const T& a, const T& tol = tolerance<T>)
{
    return (
        (a >= (T(1) - tol)) &&
        (a <= (T(1) + tol)) );
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
approx_1(const std::complex<T>& a, const T& tol = tolerance<T>)
{
    return (
        approx_1(a.real(), tol) &&
        approx_0(a.imag(), tol) );
}



//-------------------------------------------------------------------
/// @brief compares two ranges for numerical equality
//-------------------------------------------------------------------
template<class InputIter, class T, class = typename
    std::enable_if<
        is_number<typename std::iterator_traits<InputIter>::value_type>::value &&
        is_number<T>::value>::type
>
bool
approx_equal(InputIter begin1, InputIter end1,
             InputIter begin2, 
             const T& tolerance)
{
    using std::abs;

    for(;begin1 != end1; ++begin1, ++begin2) {
        if(abs(*begin1 - *begin2) > tolerance) return false;
    }

    return true;
}


template<class InputIter, class T, class = typename
    std::enable_if<
        is_number<typename std::iterator_traits<InputIter>::value_type>::value &&
        is_number<T>::value>::type
>
bool
approx_equal(InputIter begin1, InputIter end1,
             InputIter begin2, InputIter,
             const T& tolerance)
{
    return approx_equal(begin1, end1, begin2, tolerance);
}


}  // namespace num
}  // namespace am

