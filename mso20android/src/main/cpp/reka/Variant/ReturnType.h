#pragma once

#include "CallbackSignature.h"

namespace Mso {


namespace Details {


template <class T>
struct ExtractReturnType;

template <class Ret, class... Args>
struct ExtractReturnType<Ret (*)(Args...)>
{
	using Type = Ret;
};


template <class Callback>
struct ReturnType : public ExtractReturnType<typename Mso::CallbackSignature<Callback>>
{
};


} // namespace Details


template <class Callback>
using ReturnType = typename Details::ReturnType<Callback>::Type;


} // namespace Mso