/**

	Alternate char_traits that remaps wchar_t to wc16 on non-windows platforms.
	Used in templated usages of std::basic_view, etc.

	Aliases for string types using Mso::char_traits_t

	namespace Mso
	{
		char_traits_t
		basic_string_view
		basic_string
		is_basic_string
		is_basic_string_v
	}

	Example:

	template<typename TChar>
	bool TryHandleInput(const Mso::basic_string_view<TChar>& input) noexcept { ... }

	template<typename TChar>
	bool TryHandleInput(const Other::string_like_thing<TChar, Mso::char_traits_t<TChar>>& input) noexcept { ... }

	static_assert(Mso::is_basic_string_v<std::string>);
*/
#pragma once
#ifndef LIBLET_STLEXTENSIONS_STRING_ADAPTER_H
#define LIBLET_STLEXTENSIONS_STRING_ADAPTER_H

#include <compileradapters/compilerwarnings.h>

//#ifdef MS_TARGET_ANDROID
//#include "wc16_wstring.h"
//#endif

// BEGIN_DISABLE_WARNING_DEPRECATED_FUNCTION();
#include <string>
// END_DISABLE_WARNING_DEPRECATED_FUNCTION();

namespace Mso {

template<typename TChar>
struct char_traits
{
	using type = std::char_traits<TChar>;
};

// GAMMA : Removing wc16 customizations
//#if defined(MS_TARGET_ANDROID) || defined(MS_TARGET_APPLE)
//template<>
//struct char_traits<wchar_t>
//{
//	using type = ::wc16::wchar16_traits;
//};
//#endif

template<typename TChar>
using char_traits_t = typename char_traits<TChar>::type;

template<typename TChar>
using basic_string_view = std::basic_string_view<TChar, char_traits_t<TChar>>;

template<typename TChar>
using basic_string = std::basic_string<TChar, char_traits_t<TChar>>;

template <typename T>
struct is_basic_string : std::false_type {};

template <typename... T>
struct is_basic_string<std::basic_string<T...>> : std::true_type {};

template <typename T>
inline constexpr bool is_basic_string_v = is_basic_string<T>::value;

} // namespace Mso

#endif // LIBLET_STLEXTENSIONS_STRING_ADAPTER_H
