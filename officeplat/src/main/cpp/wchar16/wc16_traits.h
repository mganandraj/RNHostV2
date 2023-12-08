/*
 *  wc16_wstring.h
 *  mbustrings
 *
 *  Created by sandipn on 04/12/1
 *  Copyright 2009 Microsoft Inc. All rights reserved.
 *
 */

#ifndef WC16_TRAITS_H
#define WC16_TRAITS_H

#include "wc16.h"
#include <cstdio>
#include <cwchar>
#include <iosfwd>
#include <utility>

namespace wc16
{

struct wchar16_traits
{
   typedef wchar_t      char_type;
   typedef int       int_type;
   typedef std::streamoff    off_type;
   typedef std::streampos    pos_type;
   typedef mbstate_t    state_type;

   static constexpr void
   assign(char_type& c1, const char_type& c2) noexcept
   { c1 = c2; }

   static constexpr bool
   eq(const char_type& c1, const char_type& c2) noexcept
   { return c1 == c2; }

   static constexpr bool
   lt(const char_type& c1, const char_type& c2) noexcept
   { return c1 < c2; }

   static constexpr int
   compare(const char_type* __s1, const char_type* __s2, size_t __n) noexcept
	{
		 for (; __n; --__n, ++__s1, ++__s2)
		 {
			  if (lt(*__s1, *__s2))
					return -1;
			  if (lt(*__s2, *__s1))
					return 1;
		 }
		 return 0;
	}

   static constexpr size_t
   length(const char_type* __s) noexcept
	{
		size_t __len = 0;
		for (; !eq(*__s, char_type(0)); ++__s)
			++__len;
		return __len;
	}

   static constexpr const char_type*
   find(const char_type* __s, size_t __n, const char_type& __a) noexcept
	{
		 for (; __n; --__n)
		 {
			  if (eq(*__s, __a))
					return __s;
			  ++__s;
		 }
		 return 0;
	}

   static constexpr char_type*
   move(char_type* __s1, const char_type* __s2, size_t __n) noexcept
	{
		 char_type* __r = __s1;
		 if (__s1 < __s2)
		 {
			  for (; __n; --__n, ++__s1, ++__s2)
					assign(*__s1, *__s2);
		 }
		 else if (__s2 < __s1)
		 {
			  __s1 += __n;
			  __s2 += __n;
			  for (; __n; --__n)
					assign(*--__s1, *--__s2);
		 }
		 return __r;
	}

   static constexpr char_type*
   copy(char_type* __s1, const char_type* __s2, size_t __n) noexcept
	{
		 _LIBCPP_ASSERT(__s2 < __s1 || __s2 >= __s1+__n, "char_traits::copy overlapped range");
		 char_type* __r = __s1;
		 for (; __n; --__n, ++__s1, ++__s2)
			  assign(*__s1, *__s2);
		 return __r;
	}

   static constexpr char_type*
   assign(char_type* __s, size_t __n, char_type __a) noexcept
	{
		 char_type* __r = __s;
		 for (; __n; --__n, ++__s)
			  assign(*__s, __a);
		 return __r;
	}

   static constexpr char_type
   to_char_type(const int_type& c) noexcept
   { return static_cast<char_type>(c); }

   static constexpr int_type
   to_int_type(const char_type& c) noexcept
   { return static_cast<int_type>(c); }

   static constexpr bool
   eq_int_type(const int_type& c1, const int_type& c2) noexcept
   { return c1 == c2; }

   static constexpr int_type eof() noexcept
   { return static_cast<int_type>(WEOF); }

   static constexpr int_type not_eof(const int_type& c) noexcept
   { return eq_int_type(c, eof()) ? 0 : c; }
};

} // namespace wc16


//NDK upgrade r21 changes
template<>
struct _LIBCPP_TEMPLATE_VIS
    std::hash<std::__ndk1::basic_string<wchar_t, ::wc16::wchar16_traits> >
    : public unary_function<
          std::__ndk1::basic_string<wchar_t, ::wc16::wchar16_traits>, size_t>
{
    size_t
    operator()(const std::__ndk1::basic_string<wchar_t, ::wc16::wchar16_traits>& __val) const _NOEXCEPT
    { return __do_string_hash(__val.data(), __val.data() + __val.size()); }
};

template<>
struct _LIBCPP_TEMPLATE_VIS std::hash<std::__ndk1::basic_string_view<wchar_t, ::wc16::wchar16_traits> >
    : public unary_function<
          std::__ndk1::basic_string_view<wchar_t, ::wc16::wchar16_traits>, size_t>
{
    _LIBCPP_INLINE_VISIBILITY
    size_t operator()(const std::__ndk1::basic_string_view<wchar_t, ::wc16::wchar16_traits> __val) const _NOEXCEPT {
        return __do_string_hash(__val.data(), __val.data() + __val.size());
    }
};

#endif // WC16_TRAITS_H

