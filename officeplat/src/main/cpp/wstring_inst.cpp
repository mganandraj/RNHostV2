#include "wchar16/wc16_wstring.h"

_LIBCPP_BEGIN_NAMESPACE_STD

template class basic_string<wchar_t, wc16::wchar16_traits>;

template void w16string::__init(wchar_t const*, wchar_t const*);
template void w16string::__init(wchar_t *, wchar_t *);
template void w16string::__init(char const*, char const*);
template void w16string::__init(char*, char*);
template void w16string::__init(__wrap_iter<wchar_t const*>, __wrap_iter<wchar_t const*>);
template void w16string::__init(__wrap_iter<char const*>, __wrap_iter<char const*>);
template void w16string::__init(__wrap_iter<char*>, __wrap_iter<char*>);
template void w16string::__init(__wrap_iter<unsigned char*>, __wrap_iter<unsigned char*>);
template void w16string::__init(std::reverse_iterator<__wrap_iter<wchar_t*>>, std::reverse_iterator<__wrap_iter<wchar_t*>>);
template void w16string::__init(unsigned char const*, unsigned char const*);
template void w16string::__init(__wrap_iter<unsigned char const*>, __wrap_iter<unsigned char const*>);
template void w16string::__init(__wrap_iter<char16_t const*>, __wrap_iter<char16_t const*>);
template void w16string::__init(unsigned char*, unsigned char*);
template __attribute__ ((__visibility__("default"))) w16string& w16string::operator=(w16string&&);

template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(char const*, char const*);
template std::__ndk1::basic_string<char, std::__ndk1::char_traits<char>, std::__ndk1::allocator<char> >::basic_string(char*, char*, std::__ndk1::allocator<char> const&);

template std::__ndk1::_MetaBase<__is_cpp17_forward_iterator<wchar_t const*>::value>::_EnableIfImpl<void> std::__ndk1::basic_string<char, std::__ndk1::char_traits<char>, std::__ndk1::allocator<char> >::__init<wchar_t const*>(wchar_t const*, wchar_t const*);
template std::__ndk1::basic_string<char, std::__ndk1::char_traits<char>, std::__ndk1::allocator<char> >::basic_string(wchar_t const*, wchar_t const*);

template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(wchar_t*, wchar_t*, std::__ndk1::allocator<wchar_t> const&);

template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(wchar_t const*, wchar_t const*);

template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(std::__ndk1::__wrap_iter<wchar_t const*>, std::__ndk1::__wrap_iter<wchar_t const*>);

template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(std::__ndk1::__wrap_iter<char*>, std::__ndk1::__wrap_iter<char*>);
template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(std::__ndk1::__wrap_iter<char const*>, std::__ndk1::__wrap_iter<char const*>, std::__ndk1::allocator<wchar_t> const&);
template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(std::__ndk1::__wrap_iter<char const*>, std::__ndk1::__wrap_iter<char const*>);
template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(wchar_t*, wchar_t*);
template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(wchar_t const*, wchar_t const*, std::__ndk1::allocator<wchar_t> const&);
template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(char*, char*);
template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(std::__ndk1::__wrap_iter<unsigned char*>, std::__ndk1::__wrap_iter<unsigned char*>);
template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(char*, char*, std::__ndk1::allocator<wchar_t> const&);
template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(std::__ndk1::__wrap_iter<char16_t const*>, std::__ndk1::__wrap_iter<char16_t const*>);
template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(std::__ndk1::reverse_iterator<std::__ndk1::__wrap_iter<wchar_t*> >, std::__ndk1::reverse_iterator<std::__ndk1::__wrap_iter<wchar_t*> >);
template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(std::__ndk1::__wrap_iter<char*>, std::__ndk1::__wrap_iter<char*>, std::__ndk1::allocator<wchar_t> const&);
template std::__ndk1::basic_string<wchar_t, wc16::wchar16_traits, std::__ndk1::allocator<wchar_t> >::basic_string(std::__ndk1::__wrap_iter<unsigned char const*>, std::__ndk1::__wrap_iter<unsigned char const*>);
_LIBCPP_END_NAMESPACE_STD
