//!

#pragma once

#include <array>
#include <tuple>
#include <feel/feelcore/typetraits.hpp>

namespace Feel::Core
{

// defined invalid value with respect to an arbitray signed integer type
template<typename T>
inline const T invalid_v = T(-1);

template<typename T>
constexpr bool is_valid_value( T && t ) { return t != invalid_v<std::decay_t<T>>; }

//! create an std array from variadic args
template<typename... Ts>
constexpr auto make_array(Ts&&... ts)
{
    using CT = std::common_type_t<Ts...>;
    return std::array<CT, sizeof...(Ts)>{std::forward<CT>(ts)...};
}

//! convert a std::index_sequence into a std::tuple of std::integral_constant
template<std::size_t... I>
auto indexSequenceToTupleIntegralConstant( std::index_sequence<I...> )
{
    return std::make_tuple( std::integral_constant<std::size_t,I>{}...);
}

namespace utilities
{

template <class Tuple, class F, std::size_t... I>
void for_each_impl(Tuple&& tuple, F&& f, std::index_sequence<I...>)
{
    (f(std::get<I>(tuple)), ...);
}

template <class Tuple, class F>
void for_each(Tuple&& tuple, F&& f)
{
    for_each_impl(std::forward<Tuple>(tuple), std::forward<F>(f),
                  std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{});
}

}

//! some other utilities (currently commented)
#if 0

template <std::size_t... Is, typename F, typename T>
auto map_filter_tuple(F f, T& t)
{
    return std::make_tuple(f(std::get<Is>(t))...);
}
template <std::size_t... Is, typename F, typename T>
auto map_filter_tuple(std::index_sequence<Is...>, F f, T& t)
{
    return std::make_tuple(f(std::get<Is>(t))...);
}
template <typename S, typename F, typename T>
auto map_filter_tuple(F&& f, T& t)
{
    return map_filter_tuple(S{}, std::forward<F>(f), t);
}
#endif



template<class T>
unwrap_ref_decay_t<T>& unwrap_ref( T & t )
{
    return t;
}

}
