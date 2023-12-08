#pragma once
// Bare minimum implementation of wstring_convert as a wrapper over Windows charset conversion APIs.
// Needed on Android because std::locale is broken on Android

/*
Note: This functionality is also provided by <locale>, and <codecvt> from llvm_libc++ stl 
library. to avoid ambigous linker error for these symbols we are moulding it to different name 
so that PAL version of these API be called.
*/

#define codecvt_utf8 _plat_codecvt_utf8
#define codecvt_utf16 _plat_codecvt_utf16
#define codecvt_utf8_utf16 _plat_codecvt_utf8_utf16
#define wstring_convert _plat_wstring_convert

namespace std 
{

std::string convert_to_utf8(const wchar_t* start, const wchar_t* end);
std::wstring convert_to_utf16(const char * start, const char * end);

template<class Elem = wchar_t>
class codecvt_utf8
{
public:
    std::string to_bytes(const Elem* start, const Elem* end)
    {
        return convert_to_utf8(start, end);
    }

    w16string from_bytes(const char * start, const char * end)
    {
        return w16string(start, end);
    }
};

template<class Elem = wchar_t>
class codecvt_utf16
{
public:
    std::string to_bytes(const Elem* start, const Elem* end)
    {
        return std::string(start, end);
    }

    w16string from_bytes(const char * start, const char * end)
    {
        return convert_to_utf16(start, end);
    }
};

template<class Elem = wchar_t>
class codecvt_utf8_utf16
{
    std::string to_bytes(const Elem* start, const Elem* end)
    {
        return _utf8Convertor.to_utf8(start, end);
    }
    w16string from_bytes(const char * start, const char * end)
    {
        return _utf16Convertor.to_utf16(start, end);
    }
private:
    codecvt_utf8<Elem> _utf8Convertor;
    codecvt_utf16<Elem> _utf16Convertor;
};

template<class Codecvt, class Elem = wchar_t>
class wstring_convert
{
public:
    typedef basic_string<char, char_traits<char>> byte_string;
    typedef w16string wide_string;

    explicit wstring_convert(Codecvt* pcvt = new Codecvt) : _cvtPtr(pcvt) {}

    ~wstring_convert()
    {
        delete _cvtPtr;
    }

    wstring_convert(const wstring_convert&) = delete;
    wstring_convert & operator=(const wstring_convert &) = delete;

    wide_string from_bytes(const char* cstr)
    {
        return from_bytes(cstr, cstr + char_traits<char>::length(cstr));
    }

    wide_string from_bytes(const byte_string& str)
    {
        return from_bytes(str.data(), str.data() + str.size());
    }

    wide_string from_bytes(const char* start, const char* end)
    {
        return _cvtPtr->from_bytes(start, end);
    }

    byte_string to_bytes(const Elem* wstr)
    {
        return to_bytes(wstr, wstr + char_traits<Elem>::length(wstr));
    }
    byte_string to_bytes(const wide_string& wstr)
    {
        return to_bytes(wstr.data(), wstr.data() + wstr.size());
    }

    byte_string to_bytes(const Elem* start, const Elem* end)
    {
        return _cvtPtr->to_bytes(start, end);
    }
private:
    Codecvt* _cvtPtr;
};

}
