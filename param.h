/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 *2008-2014  André Müller
 *
 *****************************************************************************/

#ifndef AM_GENERIC_PARAM_H_
#define AM_GENERIC_PARAM_H_


#include <type_traits>


namespace am {
namespace gen {


namespace detail {

//-------------------------------------------------------------------
template<class T>
constexpr auto
check_has_param(int)
    -> decltype(
        std::declval<typename T::param_type>(),
        std::declval<T>().param(),
        std::true_type{});
//---------------------------------------------------------
template<class>
constexpr auto
check_has_param(long)
    -> std::false_type;

}  // namespace detail



//-------------------------------------------------------------------
template<class T>
struct has_param : public
    decltype(detail::check_has_param<T>(0))
{};



//-------------------------------------------------------------------
template<class T, bool has = has_param<T>::value>
struct param {
    using type = T;
};

template<class T>
struct param<T,true> {
    using type = typename T::param_type;
};

template<class T>
using param_t = typename param<T>::type;



}  // namespace gen
}  // namespace am


#endif
