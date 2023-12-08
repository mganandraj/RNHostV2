/*
 *  wc16_wstring.h
 *  mbustrings
 *
 *  Created by TJN on 8/5/09.
 *  Copyright 2009 Microsoft Inc. All rights reserved.
 *
 */

#ifndef WC16_WSTREAM_H
#define WC16_WSTREAM_H

#include "wc16.h"

#pragma push_macro("noexcept")
#pragma push_macro("min")
#pragma push_macro("max")
#undef noexcept
#undef min
#undef max
#include <istream>
#include <ostream>
#include <sstream>
#include <locale>
#pragma pop_macro("max")
#pragma pop_macro("min")
#pragma pop_macro("noexcept")
#define wostream w16ostream
#define wistream w16istream
#define wostringstream w16ostringstream
#define wstringstream w16stringstream

#include "wc16_traits.h"

namespace wc16 {

typedef std::basic_ostream< wchar_t, wchar16_traits > wostream;
typedef std::basic_istream< wchar_t, wchar16_traits > wistream;
typedef std::basic_ostringstream<wchar_t, wchar16_traits> wostringstream;

} // namespace wc16

namespace std 
{
typedef ::wc16::wostream w16ostream;
typedef ::wc16::wistream w16istream;
typedef ::wc16::wostringstream w16ostringstream;

typedef basic_istringstream<wchar_t, ::wc16::wchar16_traits> 	w16istringstream;
typedef basic_stringstream<wchar_t, ::wc16::wchar16_traits> 	w16stringstream;	

extern template class std::basic_istream<wchar_t, ::wc16::wchar16_traits>;

template <> wostream& wostream::operator<<(bool val);
template <> wostream& wostream::operator<<(short val);
template <> wostream& wostream::operator<<(unsigned short val);
template <> wostream& wostream::operator<<(int val);
template <> wostream& wostream::operator<<(unsigned int val);
template <> wostream& wostream::operator<<(long val);
template <> wostream& wostream::operator<<(unsigned long val);
template <> wostream& wostream::operator<<(long long val);
template <> wostream& wostream::operator<<(unsigned long long val);
template <> wostream& wostream::operator<<(float val);
template <> wostream& wostream::operator<<(double val);
template <> wostream& wostream::operator<<(long double val);
template <> wostream& wostream::operator<<(const void *val);

}

#endif // WC16_WSTREAM_H
