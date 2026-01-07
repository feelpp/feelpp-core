//!

#pragma once

#include <memory>
#include <type_traits>
#include <vector>

namespace Feel
{

template <class T>
struct is_smart_pointer : std::bool_constant<false> {};
template <class T>
struct is_smart_pointer<std::shared_ptr<T> > : std::bool_constant<true> {};
template <class T>
struct is_smart_pointer<std::unique_ptr<T> > : std::bool_constant<true> {};
template<class T>
constexpr bool is_smart_pointer_v = is_smart_pointer<T>::value;

template<typename T>
struct is_pointer_or_smart_pointer : std::bool_constant<is_smart_pointer_v<T>||std::is_pointer_v<T>> {};

template<class T, class Enable = void>
struct remove_smart_pointer { using type = T; };
template<class T>
struct remove_smart_pointer<T,std::enable_if_t<is_smart_pointer_v<T>>> { using type = typename T::element_type; };
template<class T>
using remove_smart_pointer_t = typename remove_smart_pointer<T>::type;

template<class T>
struct unwrap_reference { using type = T; };
template<class U>
struct unwrap_reference<std::reference_wrapper<U>> { using type = U&; };
template<class T>
using unwrap_reference_t = typename unwrap_reference<T>::type;
template<class T>
struct unwrap_ref_decay : unwrap_reference<std::decay_t<T>> {};
template<class T>
using unwrap_ref_decay_t = typename unwrap_ref_decay<T>::type;



template <typename T, typename = void>
struct is_iterable : std::false_type {};
template <typename T>
struct is_iterable<T, std::void_t<decltype(std::declval<T>().begin()),decltype(std::declval<T>().end())>>
    : std::true_type {};
template <typename T>
constexpr bool is_iterable_v = is_iterable<T>::value;


template <typename T, typename = void>
struct is_sequence_container : std::false_type {};
template <typename T>
struct is_sequence_container<T, std::void_t<decltype( std::begin( std::declval<T>() ),
                                                      std::declval<T>().end()) >> : std::true_type {};
template <typename T>
constexpr bool is_sequence_container_v = is_sequence_container<T>::value;

template <typename T, typename = void>
struct is_sequence_containers_with_index_access : std::false_type {};
template <typename T>
struct is_sequence_containers_with_index_access<T, std::void_t<decltype(std::declval<T>().begin(),
                                                                        std::declval<T>().at(0)) >> : std::true_type {};
template <typename T>
constexpr bool is_sequence_containers_with_index_access_v = is_sequence_containers_with_index_access<T>::value;


template <class T>
struct is_std_vector : std::bool_constant<false> {};
template <class T>
struct is_std_vector<std::vector<T> > : std::bool_constant<true> {};
template <class T>
using is_std_vector_t = is_std_vector<T>;
template <class T>
inline constexpr  bool is_std_vector_v = is_std_vector_t<T>::value;



}
