#pragma once


namespace Mso {


namespace Details {


template <class T>
struct RemoveParentClassFromSignature;

template <class ReturnType, class ParentType, class... Parameters>
struct RemoveParentClassFromSignature<ReturnType (ParentType::*)(Parameters...) const>
{
	using Type = ReturnType (*)(Parameters...);
};

template <class ReturnType, class ParentType, class... Parameters>
struct RemoveParentClassFromSignature<ReturnType (ParentType::*)(Parameters...) const noexcept>
{
	using Type = ReturnType (*)(Parameters...);
};

template <class Callback>
struct CallbackSignature : RemoveParentClassFromSignature<decltype(&Callback::operator())>
{
};

// Specialization for static functions.
template <class ReturnType, class... Parameters>
struct CallbackSignature<ReturnType (&)(Parameters...)>
{
	using Type = ReturnType (*)(Parameters...);
};


} // namespace Details


template <class Callback>
using CallbackSignature = typename Details::CallbackSignature<Callback>::Type;


}