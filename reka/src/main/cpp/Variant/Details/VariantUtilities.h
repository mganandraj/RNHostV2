#pragma once

#include <cstdint>
#include <type_traits>
#include <tuple>

namespace Mso { namespace Details {


template <size_t N, class... Parameters>
using NthType = std::tuple_element_t<N, std::tuple<Parameters...>>;


// We currently use 8-byte integers for storing type indexes, which limits Mso::variant to 255 possible members.
// We could increase this limit some day if necessary, and ideally we'd do it with some pay-for-play feature
// where only variants with 256+ members use multiple bytes for type indexes.
using IndexType = uint8_t;

constexpr IndexType InvalidIndex = static_cast<IndexType>(-1);


template <class... T>
struct ParameterPack
{
};


// Avoid warning C4307 "integral constant overflow"
constexpr IndexType IncrementIndex(IndexType i) noexcept
{
	return static_cast<IndexType>(i + 1);
}


template <class A, class B, class... Rest>
struct IndexOfType
{
	static const IndexType Value =
		std::is_same<A, B>::value ?
			0 :
			(IndexOfType<A, Rest...>::Value == InvalidIndex ?
				InvalidIndex :
				IncrementIndex(IndexOfType<A, Rest...>::Value));
};

template <class A, class B>
struct IndexOfType<A, B>
{
	static const IndexType Value = std::is_same<A, B>::value ? 0 : InvalidIndex;
};


template <template <class> class Trait, class Head, class... Tail>
struct VariadicTrait
{
	static const auto Value = Trait<Head>::value && VariadicTrait<Trait, Tail...>::Value;
};

template <template <class> class Trait, class T>
struct VariadicTrait<Trait, T>
{
	static const auto Value = Trait<T>::value;
};


template <class A, class B>
using LargerOf = std::conditional_t<(sizeof(A) > sizeof(B)), A, B>;


template <class...>
struct LargestTypeImpl;

template <class A, class B, class... Rest>
struct LargestTypeImpl<A, B, Rest...> : LargestTypeImpl<LargerOf<A, B>, Rest...>
{
};

template <class A>
struct LargestTypeImpl<A>
{
	using Type = A;
};

template <class... Types>
using LargestType = typename LargestTypeImpl<Types...>::Type;


}}