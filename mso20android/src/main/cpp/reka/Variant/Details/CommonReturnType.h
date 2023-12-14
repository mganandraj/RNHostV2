#pragma once

#include <Variant/ReturnType.h>

namespace Mso { namespace Details {


struct InconsistentReturnType
{
};


template <class A, class B>
struct SameOrUndefined
{
	using Type = InconsistentReturnType;
};

template <class A>
struct SameOrUndefined<A, A>
{
	using Type = A;
};



template <class First, class... Rest>
struct GetCommonReturnType
{
	using FirstReturnType = Mso::ReturnType<First>;
	using RestReturnType = typename GetCommonReturnType<Rest...>::Type;

	using Type = typename SameOrUndefined<FirstReturnType, RestReturnType>::Type;
};


template <class Callback>
struct GetCommonReturnType<Callback>
{
	using Type = Mso::ReturnType<Callback>;
};


template <class... Callbacks>
using CommonReturnType = typename GetCommonReturnType<Callbacks...>::Type;


}}