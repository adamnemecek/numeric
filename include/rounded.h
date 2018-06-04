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

#include <cmath>
#include <cstdint>
#include <cfloat>

#include "limits.h"
#include "traits.h"


namespace am {
namespace num {


/*****************************************************************************
 *
 * ROUNDING METHODS
 *
 *****************************************************************************/
struct round_to_nearest_int
{
    template<class T>
    T operator () (const T& x) const noexcept {
        using std::nearbyint;
        return (is_integral<T>::value) ? x : nearbyint(x);
    }
};



//-------------------------------------------------------------------
template<class T>
struct round_to_nearest
{
    round_to_nearest(T unit = T(0) ) :
        unit_(std::move(unit))
    {
        if(unit_ <= T(0)) {
            unit_ = is_integral<T>::value
                       ? T(1)
                       : std::numeric_limits<T>::epsilon();
        }
    }

    T operator () (T x) const noexcept {
        using std::remainder;
        return (x - remainder(x, unit_));
    }

private:
    T unit_;
};






/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class,class> class rounded;

template<class>
struct is_rounded :
    std::false_type
{};

template<class T, class R>
struct is_rounded<rounded<T,R>> :
    std::true_type
{};




/*************************************************************************//***
 *
 * @brief  represents a number that is restricted to an interval
 *
 * @tparam NumberType        value type
 * @tparam RoundingMethod
 *
 * @details
 * inherits from RoundingMethod to make use of emtpy base class optimization
 *
 *****************************************************************************/
template<
    class NumberType,
    class RoundingMethod
>
class rounded :
    private RoundingMethod
{
public:

    static_assert(is_number<NumberType>::value,
        "rounded<T,R>: T must be a number type");

    static_assert(!is_rounded<NumberType>::value,
        "rounded<T,R>: T must not be a rounded<> type itself");


    //---------------------------------------------------------------
    using value_type      = NumberType;
    using numeric_type    = value_type;
    using rounding_method = RoundingMethod;


    //---------------------------------------------------------------
    /// @brief default constructor
    constexpr
    rounded() = default;

    //-----------------------------------------------------
    constexpr
    rounded(const value_type& v, rounding_method rm = rounding_method()) :
        rounding_method(rm),
        v_{corrected(v, rm)}
    {}

    constexpr
    rounded(value_type&& v, rounding_method rm = rounding_method()) :
        rounding_method(rm),
        v_{corrected(std::move(v), rm)}
    {}

    explicit constexpr
    rounded(rounding_method rp) :
        rounding_method(std::move(rp)),
        v_()
    {}

    constexpr
    rounded(const rounded&) = default;

    constexpr
    rounded(rounded&&) = default;


    //---------------------------------------------------------------
    rounded&
    operator = (const rounded&) = default;
    
    rounded&
    operator = (rounded&& src) {
        rounding_method::operator = (src);
        v_ = std::move(src.v_);
        return *this;
    }

    rounded&
    operator = (const value_type& v) {
        v_ = corrected(v);
        return *this;
    }

    rounded&
    operator = (value_type&& v) {
        v_ = corrected(std::move(v));
        return *this;
    }


    //---------------------------------------------------------------
    constexpr const value_type&
    value() const noexcept {
        return v_;
    }

    constexpr
    operator const value_type& () const noexcept
    {
        return v_;
    }


    //---------------------------------------------------------------
    rounded&
    operator += (const value_type& v) {
        v_ = corrected(v_ + v);
        return *this;
    }

    rounded&
    operator -= (const value_type& v) {
        v_ = corrected(v_ - v);
        return *this;
    }

    rounded&
    operator *= (const value_type& v) {
        v_ = corrected(v_ * v);
        return *this;
    }
    
    rounded&
    operator /= (const value_type& v) {
        v_ = corrected(v_ /= v);
        return *this;
    }


    //---------------------------------------------------------------
    rounded&
    operator ++ () {
        v_ = corrected(v_ + 1);
        return *this;
    }
    
    rounded&
    operator -- () {
        v_ = corrected(v_ - 1);
        return *this;
    }

    rounded
    operator ++ (int) {
        auto old = *this;
        ++*this;
        return old;
    }
    
    rounded
    operator -- (int) {
        auto old = *this;
        --*this;
        return old;
    }


    //---------------------------------------------------------------
    rounded&
    operator += (const rounded& o) {
        v_ = corrected(v_ + o.value());
        return *this;
    }
    
    rounded&
    operator -= (const rounded& o) {
        v_ = corrected(v_ - o.value());
        return *this;
    }
    
    rounded&
    operator *= (const rounded& o) {
        v_ = corrected(v_ * o.value());
        return *this;
    }
    
    rounded&
    operator /= (const rounded& o) {
        v_ = corrected(v_ / o.value());
        return *this;
    }
   
    rounded&
    operator %= (const rounded& o) {
        v_ = corrected(v_ % o.value());
        return *this;
    }


private:
    //---------------------------------------------------------------
    constexpr value_type
    corrected(value_type v) const noexcept {
        return rounding_method::operator()(std::move(v));
    }

    /// @brief needed for constexpr construction
    static constexpr value_type
    corrected(value_type v, const rounding_method& rm) noexcept {
        return rm(std::move(v));
    }

    value_type v_;
};






/*****************************************************************************
 *
 * DERIVED TYPES
 *
 *****************************************************************************/
template<class T>
using rounded_to_nearest = rounded<T,round_to_nearest<T>>;

template<class T>
using rounded_to_nearest_int = rounded<T,round_to_nearest_int>;




/*****************************************************************************
 *
 * FACTORIES
 *
 *****************************************************************************/
template<class T1, class T2, class = std::enable_if_t<
    is_number<std::decay_t<T1>>::value &&
    is_number<std::decay_t<T2>>::value &&
    !is_rounded<std::decay_t<T1>>::value &&
    !is_rounded<std::decay_t<T2>>::value>>
inline constexpr auto
make_rounded_to_nearest(T1&& x, T2&& unit)
{
    return rounded_to_nearest<common_numeric_t<std::decay_t<T1>,std::decay_t<T2>>>{
        std::forward<T1>(x), std::move(unit)};
}




/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class Istream, class T, class R>
inline Istream&
operator >> (Istream& is, rounded<T,R>& x)
{
    T v;
    is >> v;
    x = v;
    return is;
}

//---------------------------------------------------------
template<class Ostream, class T, class R>
inline Ostream&
operator << (Ostream& os, const rounded<T,R>& x)
{
    return (os << x.value());
}

//---------------------------------------------------------
template<class Ostream, class T, class R>
inline Ostream&
print(Ostream& os, const rounded<T,R>& x)
{
    return (os << x.value());
}




/*****************************************************************************
 *
 * COMPARISON
 *
 *****************************************************************************/
template<class T1, class B1, class T2, class B2,
    class T3 = common_numeric_t<T1,T2>>
inline constexpr bool
approx_equal(const rounded<T1,B1>& a, const rounded<T2,B2>& b,
    const T3& tol = tolerance<T3>)
{
    return approx_equal(a.value(), b.value(), tol);
}

//---------------------------------------------------------
template<class T, class B>
inline constexpr bool
approx_1(const rounded<T,B>& x, const T& tol = tolerance<T>)
{
    return approx_1(x.value(), tol);
}

//---------------------------------------------------------
template<class T, class B>
inline constexpr bool
approx_0(const rounded<T,B>& x, const T& tol = tolerance<T>)
{
    return approx_0(x.value(), tol);
}



//-------------------------------------------------------------------
template<class T1, class R, class T2, class = 
    std::enable_if_t<is_number<T2>::value && !is_rounded<T2>::value>>
inline bool
operator == (const rounded<T1,R>& x, const T2& r)
{
    return (x.value() == r);
}
//---------------------------------------------------------
template<class T1, class R, class T2, class = 
    std::enable_if_t<is_number<T2>::value && !is_rounded<T2>::value>>
inline bool
operator == (const T2& r, const rounded<T1,R>& x)
{
    return (r == x.value());
}
//---------------------------------------------------------
template<class T1, class R1, class T2, class R2>
inline bool
operator == (const rounded<T1,R1>& a, const rounded<T2,R2>& b)
{
    return (a.value() == b.value());
}



//-------------------------------------------------------------------
template<class T1, class R, class T2, class = 
    std::enable_if_t<is_number<T2>::value && !is_rounded<T2>::value>>
inline bool
operator != (const rounded<T1,R>& x, const T2& r)
{
    return (x.value() != r);
}
//---------------------------------------------------------
template<class T1, class R, class T2, class = 
    std::enable_if_t<is_number<T2>::value && !is_rounded<T2>::value>>
inline bool
operator != (const T2& r, const rounded<T1,R>& x)
{
    return (r != x.value());
}
//---------------------------------------------------------
template<class T1, class R1, class T2, class R2>
inline bool
operator != (const rounded<T1,R1>& a, const rounded<T2,R2>& b)
{
    return (a.value() != b.value());
}



//-------------------------------------------------------------------
template<class T1, class R, class T2, class = 
    std::enable_if_t<is_number<T2>::value && !is_rounded<T2>::value>>
inline bool
operator > (const rounded<T1,R>& x, const T2& r)
{
    return (x.value() > r);
}
//---------------------------------------------------------
template<class T1, class R, class T2, class = 
    std::enable_if_t<is_number<T2>::value && !is_rounded<T2>::value>>
inline bool
operator > (const T2& r, const rounded<T1,R>& x)
{
    return (r > x.value());
}
//---------------------------------------------------------
template<class T1, class R1, class T2, class R2>
inline bool
operator > (const rounded<T1,R1>& a, const rounded<T2,R2>& b)
{
    return (a.value() > b.value());
}


//---------------------------------------------------------
template<class T1, class R, class T2, class = 
    std::enable_if_t<is_number<T2>::value && !is_rounded<T2>::value>>
inline bool
operator >= (const rounded<T1,R>& x, const T2& r)
{
    return (x.value() >= r);
}
//---------------------------------------------------------
template<class T1, class R, class T2, class = 
    std::enable_if_t<is_number<T2>::value && !is_rounded<T2>::value>>
inline bool
operator >= (const T2& r, const rounded<T1,R>& x)
{
    return (r >= x.value());
}
//---------------------------------------------------------
template<class T1, class R1, class T2, class R2>
inline bool
operator >= (const rounded<T1,R1>& a, const rounded<T2,R2>& b)
{
    return (a.value() >= b.value());
}


//---------------------------------------------------------
template<class T1, class R, class T2, class = 
    std::enable_if_t<is_number<T2>::value && !is_rounded<T2>::value>>
inline bool
operator < (const rounded<T1,R>& x, const T2& r)
{
    return (x.value() < r);
}
//---------------------------------------------------------
template<class T1, class R, class T2, class = 
    std::enable_if_t<is_number<T2>::value && !is_rounded<T2>::value>>
inline bool
operator < (const T2& r, const rounded<T1,R>& x)
{
    return (r < x.value());
}
//---------------------------------------------------------
template<class T1, class R1, class T2, class R2>
inline bool
operator < (const rounded<T1,R1>& a, const rounded<T2,R2>& b)
{
    return (a.value() < b.value());
}


//---------------------------------------------------------
template<class T1, class R, class T2, class = 
    std::enable_if_t<is_number<T2>::value && !is_rounded<T2>::value>>
inline bool
operator <= (const rounded<T1,R>& x, const T2& r)
{
    return (x.value() <= r);
}
//---------------------------------------------------------
template<class T1, class R, class T2, class = 
    std::enable_if_t<is_number<T2>::value && !is_rounded<T2>::value>>
inline bool
operator <= (const T2& r, const rounded<T1,R>& x)
{
    return (r <= x.value());
}
//---------------------------------------------------------
template<class T1, class R1, class T2, class R2>
inline bool
operator <= (const rounded<T1,R1>& a, const rounded<T2,R2>& b)
{
    return (a.value() <= b.value());
}




/*****************************************************************************
 *
 * ARITHMETIC
 *
 *****************************************************************************/
template<class T1, class R1, class T2, class R2>
inline constexpr auto
operator + (const rounded<T1,R1>& x, const rounded<T2,R2>& y)
{
    return (x.value() + y.value());
}

//---------------------------------------------------------
template<class T1, class R, class T2, class = 
    std::enable_if_t<is_number<T2>::value && !is_rounded<T2>::value>>
inline constexpr auto
operator + (const rounded<T1,R>& x, const T2& y)
{
    return (x.value() + y);
}
//---------------------------------------------------------
template<class T1, class R, class T2, class = 
    std::enable_if_t<is_number<T2>::value && !is_rounded<T2>::value>>
inline constexpr auto
operator + (const T2& y, const rounded<T1,R>& x)
{
    return (y + x.value());
}



//-------------------------------------------------------------------
template<class T1, class R1, class T2, class R2>
inline constexpr auto
operator - (const rounded<T1,R1>& x, const rounded<T2,R2>& y)
{
    return (x.value() - y.value());
}

//---------------------------------------------------------
template<class T1, class R, class T2, class = 
    std::enable_if_t<is_number<T2>::value && !is_rounded<T2>::value>>
inline constexpr auto
operator - (const rounded<T1,R>& x, const T2& y)
{
    return (x.value() - y);
}
//---------------------------------------------------------
template<class T1, class R, class T2, class = 
    std::enable_if_t<is_number<T2>::value && !is_rounded<T2>::value>>
inline constexpr auto
operator - (const T2& y, const rounded<T1,R>& x)
{
    return (y - x.value());
}



//-------------------------------------------------------------------
template<class T1, class R1, class T2, class R2>
inline constexpr auto
operator * (const rounded<T1,R1>& x, const rounded<T2,R2>& y)
{
    return (x.value() * y.value());
}

//---------------------------------------------------------
template<class T1, class R, class T2, class = 
    std::enable_if_t<is_number<T2>::value && !is_rounded<T2>::value>>
inline constexpr auto
operator * (const rounded<T1,R>& x, const T2& y)
{
    return (x.value() * y);
}
//---------------------------------------------------------
template<class T1, class R, class T2, class = 
    std::enable_if_t<is_number<T2>::value && !is_rounded<T2>::value>>
inline constexpr auto
operator * (const T2& y, const rounded<T1,R>& x)
{
    return (y * x.value());
}



//-------------------------------------------------------------------
template<class T1, class R1, class T2, class R2>
inline constexpr auto
operator / (const rounded<T1,R1>& x, const rounded<T2,R2>& y)
{
    return (x.value() / y.value());
}

//---------------------------------------------------------
template<class T1, class R, class T2, class = 
    std::enable_if_t<is_number<T2>::value && !is_rounded<T2>::value>>
inline constexpr auto
operator / (const rounded<T1,R>& x, const T2& y)
{
    return (x.value() / y);
}
//---------------------------------------------------------
template<class T1, class R, class T2, class = 
    std::enable_if_t<is_number<T2>::value && !is_rounded<T2>::value>>
inline constexpr auto
operator / (const T2& y, const rounded<T1,R>& x)
{
    return (y / x.value());
}



//-------------------------------------------------------------------
template<class T1, class R1, class T2, class R2>
inline constexpr auto
operator % (const rounded<T1,R1>& x, const rounded<T2,R2>& y)
{
    return (x.value() % y.value());
}

//---------------------------------------------------------
template<class T1, class R, class T2, class = 
    std::enable_if_t<is_number<T2>::value && !is_rounded<T2>::value>>
inline constexpr auto
operator % (const rounded<T1,R>& x, const T2& y)
{
    return (x.value() % y);
}
//---------------------------------------------------------
template<class T1, class R, class T2, class = 
    std::enable_if_t<is_number<T2>::value && !is_rounded<T2>::value>>
inline constexpr auto
operator % (const T2& y, const rounded<T1,R>& x)
{
    return (y % x.value());
}



//-------------------------------------------------------------------
template<class T, class R>
inline constexpr
rounded<T,R>
operator - (const rounded<T,R>& x)
{
    return rounded<T,R>{-x.value()};
}




/*****************************************************************************
 *
 * MATH FUNCTIONS
 *
 *****************************************************************************/
template<class T, class R>
inline bool
isfinite(const rounded<T,R>& x)
{
    using std::isfinite;
    return isfinite(x.value());
}

//---------------------------------------------------------
template<class T, class R>
inline bool
isinf(const rounded<T,R>& x)
{
    using std::isinf;
    return isinf(x.value());
}

//---------------------------------------------------------
template<class T, class R>
inline bool
isnan(const rounded<T,R>& x)
{
    using std::isnan;
    return isnan(x.value());
}

//---------------------------------------------------------
template<class T, class R>
inline bool
isnormal(const rounded<T,R>& x)
{
    using std::isnormal;
    return isnormal(x.value());
}

//---------------------------------------------------------
template<class T, class R>
inline rounded<T,R>
abs(const rounded<T,R>& x)
{
    using std::abs;
    return rounded<T,R>(abs(x.value()));
}




/*****************************************************************************
 *
 * TRAITS SPECIALIZATIONS
 *
 *****************************************************************************/
template<class T, class R>
struct is_number<rounded<T,R>> : std::true_type {};

template<class T, class R>
struct is_number<rounded<T,R>&> : std::true_type {};

template<class T, class R>
struct is_number<rounded<T,R>&&> : std::true_type {};

template<class T, class R>
struct is_number<const rounded<T,R>&> : std::true_type {};

template<class T, class R>
struct is_number<const rounded<T,R>> : std::true_type {};



//-------------------------------------------------------------------
template<class T, class R>
struct is_floating_point<rounded<T,R>> :
    std::integral_constant<bool, is_floating_point<T>::value>
{};



//-------------------------------------------------------------------
template<class T1, class R, class T2>
struct common_numeric_type<rounded<T1,R>,T2>
{
    using type = common_numeric_t<T1,T2>;
};
//---------------------------------------------------------
template<class T1, class R, class T2>
struct common_numeric_type<T2,rounded<T1,R>>
{
    using type = common_numeric_t<T1,T2>;
};
//---------------------------------------------------------
/// @brief
template<class T1, class R1, class T2, class R2>
struct common_numeric_type<rounded<T1,R1>,rounded<T2,R2>>
{
    using type = common_numeric_t<T1,T2>;
};


}  // namespace num
}  // namespace am


namespace std {


/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class T, class R>
class numeric_limits<am::num::rounded<T,R>>
{
    using val_t = am::num::rounded<T,R>;

public:
    static constexpr bool is_specialized = true;

    static constexpr val_t
    min() {return val_t(numeric_limits<T>::min()); }

    static constexpr val_t
    max() {return val_t(numeric_limits<T>::max()); }

    static constexpr val_t
    lowest() {return val_t(numeric_limits<T>::lowest()); }

    static constexpr int digits = numeric_limits<T>::digits;
    static constexpr int digits10 = numeric_limits<T>::digits10;
    static constexpr int max_digits10 = numeric_limits<T>::max_digits10;
    static constexpr bool is_signed = numeric_limits<T>::is_signed;
    static constexpr bool is_integer = numeric_limits<T>::is_integer;
    static constexpr bool is_exact = numeric_limits<T>::is_exact;
    static constexpr int radix = numeric_limits<T>::radix;

    static constexpr val_t
    epsilon() noexcept { return std::numeric_limits<T>::epsilon(); }

    static constexpr val_t
    round_error() noexcept { return numeric_limits<T>::round_error(); }

    static constexpr int min_exponent   = numeric_limits<T>::min_exponent;
    static constexpr int min_exponent10 = numeric_limits<T>::min_exponent10;
    static constexpr int max_exponent   = numeric_limits<T>::max_exponent;
    static constexpr int max_exponent10 = numeric_limits<T>::max_exponent10;

    static constexpr bool has_infinity = numeric_limits<T>::has_infinity;
    static constexpr bool has_quiet_NaN = numeric_limits<T>::has_quiet_NaN;
    static constexpr bool has_signaling_NaN = numeric_limits<T>::has_signaling_NaN;
    static constexpr std::float_denorm_style has_denorm = numeric_limits<T>::has_denorm;
    static constexpr bool has_denorm_loss = numeric_limits<T>::has_denorm_loss;

    static constexpr val_t
    infinity() noexcept {return val_t(numeric_limits<T>::infinity()); }

    static constexpr val_t
    quiet_NaN() noexcept {return val_t(numeric_limits<T>::quiet_NaN()); }

    static constexpr val_t
    signaling_NaN() noexcept {return val_t(numeric_limits<T>::signaling_NaN()); }

    static constexpr val_t
    denorm_min() noexcept {return val_t(numeric_limits<T>::denorm_min()); }

    static constexpr bool is_iec559 = numeric_limits<T>::is_iec559;
    static constexpr bool is_bounded = numeric_limits<T>::is_bounded;
    static constexpr bool is_modulo = numeric_limits<T>::is_modulo;

    static constexpr bool traps = numeric_limits<T>::traps;
    static constexpr bool tinyness_before = numeric_limits<T>::tinyness_before;
    static constexpr std::float_round_style round_style = numeric_limits<T>::round_style;
};

} // namespace std

