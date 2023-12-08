/*
 *  wc16_wstring.h
 *  mbustrings
 *
 *  Created by TJN on 8/5/09.
 *  Copyright 2009 Microsoft Inc. All rights reserved.
 *
 */
#ifndef WC16_WSTRING_H
#define WC16_WSTRING_H

#include "wc16.h"

#pragma push_macro("noexcept")
#pragma push_macro("min")
#pragma push_macro("max")
#undef noexcept
#undef min
#undef max
#include <string>
#include <locale>
#pragma pop_macro("max")
#pragma pop_macro("min")
#pragma pop_macro("noexcept")

#include "wc16_traits.h"

_LIBCPP_BEGIN_NAMESPACE_STD

using w16string_view = std::basic_string_view< wchar_t, ::wc16::wchar16_traits >;

using w16string = std::basic_string< wchar_t, ::wc16::wchar16_traits >;

extern template std::__ndk1::_MetaBase<std::__is_cpp17_forward_iterator<char const*>::value>::_EnableIfImpl<void> std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<char const*>(char const*, char const*);

extern template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(char const*, char const*);

extern template std::__ndk1::_MetaBase<std::__is_cpp17_forward_iterator<wchar_t*>::value>::_EnableIfImpl<void> std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<wchar_t*>(wchar_t*, wchar_t*);

extern template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(wchar_t*, wchar_t*, std::__ndk1::allocator<wchar_t> const&);

extern template std::__ndk1::_MetaBase<std::__is_cpp17_forward_iterator<wchar_t const*>::value>::_EnableIfImpl<void> std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<wchar_t const*>(wchar_t const*, wchar_t const*);

extern template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(wchar_t const*, wchar_t const*);

extern template std::__ndk1::_MetaBase<__is_cpp17_forward_iterator<std::__ndk1::__wrap_iter<wchar_t const*> >::value>::_EnableIfImpl<void> std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<std::__ndk1::__wrap_iter<wchar_t const*> >(std::__ndk1::__wrap_iter<wchar_t const*>, std::__ndk1::__wrap_iter<wchar_t const*>);
extern template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(std::__ndk1::__wrap_iter<wchar_t const*>, std::__ndk1::__wrap_iter<wchar_t const*>);

extern template std::__ndk1::_MetaBase<__is_cpp17_forward_iterator<std::__ndk1::__wrap_iter<char*> >::value>::_EnableIfImpl<void> std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<std::__ndk1::__wrap_iter<char*> >(std::__ndk1::__wrap_iter<char*>, std::__ndk1::__wrap_iter<char*>);
extern template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(std::__ndk1::__wrap_iter<char*>, std::__ndk1::__wrap_iter<char*>);

extern template std::__ndk1::_MetaBase<__is_cpp17_forward_iterator<std::__ndk1::__wrap_iter<char const*> >::value>::_EnableIfImpl<void> std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<std::__ndk1::__wrap_iter<char const*> >(std::__ndk1::__wrap_iter<char const*>, std::__ndk1::__wrap_iter<char const*>);
extern template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(std::__ndk1::__wrap_iter<char const*>, std::__ndk1::__wrap_iter<char const*>, std::__ndk1::allocator<wchar_t> const&);

extern template std::__ndk1::_MetaBase<__is_cpp17_forward_iterator<std::__ndk1::__wrap_iter<char const*> >::value>::_EnableIfImpl<void> std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<std::__ndk1::__wrap_iter<char const*> >(std::__ndk1::__wrap_iter<char const*>, std::__ndk1::__wrap_iter<char const*>);
extern template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(std::__ndk1::__wrap_iter<char const*>, std::__ndk1::__wrap_iter<char const*>);

extern template std::__ndk1::_MetaBase<__is_cpp17_forward_iterator<wchar_t*>::value>::_EnableIfImpl<void> std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<wchar_t*>(wchar_t*, wchar_t*);
extern template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(wchar_t*, wchar_t*);


extern template std::__ndk1::_MetaBase<__is_cpp17_forward_iterator<wchar_t const*>::value>::_EnableIfImpl<void> std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<wchar_t const*>(wchar_t const*, wchar_t const*);
extern template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(wchar_t const*, wchar_t const*, std::__ndk1::allocator<wchar_t> const&);

extern template std::__ndk1::_MetaBase<__is_cpp17_forward_iterator<char*>::value>::_EnableIfImpl<void> std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<char*>(char*, char*);
extern template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(char*, char*);

extern template  std::__ndk1::_MetaBase<__is_cpp17_forward_iterator<std::__ndk1::__wrap_iter<unsigned char*> >::value>::_EnableIfImpl<void> std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<std::__ndk1::__wrap_iter<unsigned char*> >(std::__ndk1::__wrap_iter<unsigned char*>, std::__ndk1::__wrap_iter<unsigned char*>);
extern template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(std::__ndk1::__wrap_iter<unsigned char*>, std::__ndk1::__wrap_iter<unsigned char*>);



extern template std::__ndk1::_MetaBase<__is_cpp17_forward_iterator<char*>::value>::_EnableIfImpl<void> std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<char*>(char*, char*);
extern template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(char*, char*, std::__ndk1::allocator<wchar_t> const&);

extern template std::__ndk1::_MetaBase<__is_cpp17_forward_iterator<std::__ndk1::__wrap_iter<char16_t const*> >::value>::_EnableIfImpl<void> std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<std::__ndk1::__wrap_iter<char16_t const*> >(std::__ndk1::__wrap_iter<char16_t const*>, std::__ndk1::__wrap_iter<char16_t const*>);
extern template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(std::__ndk1::__wrap_iter<char16_t const*>, std::__ndk1::__wrap_iter<char16_t const*>);;


extern template std::__ndk1::_MetaBase<__is_cpp17_forward_iterator<std::__ndk1::reverse_iterator<std::__ndk1::__wrap_iter<wchar_t*> > >::value>::_EnableIfImpl<void> std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<std::__ndk1::reverse_iterator<std::__ndk1::__wrap_iter<wchar_t*> > >(std::__ndk1::reverse_iterator<std::__ndk1::__wrap_iter<wchar_t*> >, std::__ndk1::reverse_iterator<std::__ndk1::__wrap_iter<wchar_t*> >);
extern template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(std::__ndk1::reverse_iterator<std::__ndk1::__wrap_iter<wchar_t*> >, std::__ndk1::reverse_iterator<std::__ndk1::__wrap_iter<wchar_t*> >);

extern template std::__ndk1::_MetaBase<__is_cpp17_forward_iterator<std::__ndk1::__wrap_iter<char*> >::value>::_EnableIfImpl<void> std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<std::__ndk1::__wrap_iter<char*> >(std::__ndk1::__wrap_iter<char*>, std::__ndk1::__wrap_iter<char*>);
extern template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(std::__ndk1::__wrap_iter<char*>, std::__ndk1::__wrap_iter<char*>, std::__ndk1::allocator<wchar_t> const&);


extern template std::__ndk1::_MetaBase<__is_cpp17_forward_iterator<std::__ndk1::__wrap_iter<unsigned char const*> >::value>::_EnableIfImpl<void> std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<std::__ndk1::__wrap_iter<unsigned char const*> >(std::__ndk1::__wrap_iter<unsigned char const*>, std::__ndk1::__wrap_iter<unsigned char const*>);
extern template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(std::__ndk1::__wrap_iter<unsigned char const*>, std::__ndk1::__wrap_iter<unsigned char const*>);

extern template class std::basic_string< wchar_t, ::wc16::wchar16_traits >;
extern template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(char const*, char const*);
extern template std::__ndk1::enable_if<__is_cpp17_forward_iterator<wchar_t const*>::value, void>::type std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<wchar_t const*>(wchar_t const*, wchar_t const*);
extern template std::__ndk1::enable_if<__is_cpp17_forward_iterator<wchar_t*>::value, void>::type std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<wchar_t*>(wchar_t*, wchar_t*);
extern template std::__ndk1::enable_if<__is_cpp17_forward_iterator<std::__ndk1::reverse_iterator<std::__ndk1::__wrap_iter<wchar_t*> > >::value, void>::type std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<std::__ndk1::reverse_iterator<std::__ndk1::__wrap_iter<wchar_t*> > >(std::__ndk1::reverse_iterator<std::__ndk1::__wrap_iter<wchar_t*> >, std::__ndk1::reverse_iterator<std::__ndk1::__wrap_iter<wchar_t*> >);
extern template std::__ndk1::enable_if<__is_cpp17_forward_iterator<unsigned char const*>::value, void>::type std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<unsigned char const*>(unsigned char const*, unsigned char const*);
extern template std::__ndk1::enable_if<__is_cpp17_forward_iterator<std::__ndk1::__wrap_iter<unsigned char const*> >::value, void>::type std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<std::__ndk1::__wrap_iter<unsigned char const*> >(std::__ndk1::__wrap_iter<unsigned char const*>, std::__ndk1::__wrap_iter<unsigned char const*>);
extern template std::__ndk1::enable_if<__is_cpp17_forward_iterator<std::__ndk1::__wrap_iter<char const*> >::value, void>::type std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<std::__ndk1::__wrap_iter<char const*> >(std::__ndk1::__wrap_iter<char const*>, std::__ndk1::__wrap_iter<char const*>);
extern template std::__ndk1::enable_if<__is_cpp17_forward_iterator<std::__ndk1::__wrap_iter<wchar_t const*> >::value, void>::type std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<std::__ndk1::__wrap_iter<wchar_t const*> >(std::__ndk1::__wrap_iter<wchar_t const*>, std::__ndk1::__wrap_iter<wchar_t const*>);
extern template std::__ndk1::enable_if<__is_cpp17_forward_iterator<char const*>::value, void>::type std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<char const*>(char const*, char const*);
extern template std::__ndk1::enable_if<__is_cpp17_forward_iterator<char*>::value, void>::type std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<char*>(char*, char*);
extern template std::__ndk1::enable_if<__is_cpp17_forward_iterator<std::__ndk1::__wrap_iter<char*> >::value, void>::type std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<std::__ndk1::__wrap_iter<char*> >(std::__ndk1::__wrap_iter<char*>, std::__ndk1::__wrap_iter<char*>);
extern template std::__ndk1::enable_if<__is_cpp17_forward_iterator<std::__ndk1::__wrap_iter<unsigned char*> >::value, void>::type std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<std::__ndk1::__wrap_iter<unsigned char*> >(std::__ndk1::__wrap_iter<unsigned char*>, std::__ndk1::__wrap_iter<unsigned char*>);
extern template std::__ndk1::enable_if<__is_cpp17_forward_iterator<std::__ndk1::__wrap_iter<char16_t const*> >::value, void>::type std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<std::__ndk1::__wrap_iter<char16_t const*> >(std::__ndk1::__wrap_iter<char16_t const*>, std::__ndk1::__wrap_iter<char16_t const*>);
extern template std::__ndk1::enable_if<__is_cpp17_forward_iterator<unsigned char*>::value, void>::type std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::__init<unsigned char*>(unsigned char*, unsigned char*);
extern template __attribute__ ((__visibility__("default"))) std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >& std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::operator=(std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >&&);

_LIBCPP_END_NAMESPACE_STD

namespace wc16 {

// Legacy types (please use std::w16string* instead)
using w16string_view = std::w16string_view;
using w16string = std::w16string;

} // namespace

#define BOOST_HASH_HAS_W16STRING 1
#define wstring_view w16string_view
#define wstring w16string

// to_[w]string and stoX functions
#include "string_conversions.h"

#endif // WC16_WSTRING_H
