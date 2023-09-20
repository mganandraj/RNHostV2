#pragma once

#include <string>
#include <map>
#include "RekaApi.h"

#include <optional>

#pragma pack(push, _CRT_PACKING)

namespace Mso { namespace JSHost {

template <class... TArgs>
std::string ArgsToJson(const TArgs&... args) noexcept;

template <class TArg, class TOwner>
struct Event
{
	Event(const char* name) noexcept
		: m_eventName{name}
	{
	}

	void SetHasListeners(bool /*hasListeners*/) noexcept
	{
	}

	void Raise(IRekaContext& serviceContext, TArg&& arg) noexcept
	{
		serviceContext.CallJavaScript(TOwner::Name, m_eventName.c_str(), Mso::JSHost::ArgsToJson(std::move(arg)).c_str());
	}

private:
	std::string m_eventName;
};

template <class T>
struct Indexer : std::map<std::wstring, T>
{
};

template <class... TTypes>
struct Intersection : TTypes...
{
};

// TODO: We shouldn't reinvent the wheel; do we have an std::optional that works on all platforms / compilers we target?
template <class TValue>
using Optional = std::optional<TValue>;

}} // namespace Mso::JSHost

#pragma pack(pop)
