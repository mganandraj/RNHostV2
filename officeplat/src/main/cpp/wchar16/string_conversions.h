#pragma once

#include "wc16_wstring.h"

#include <cstdlib>
#include <climits>
#include <cerrno>
#include <stdexcept>

#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#include <limits>
#pragma pop_macro("min")
#pragma pop_macro("max")

// Declaring swprintf_s etc. rather than including its header (_stdio.h) and its  
// dependency (typedefs.h). Including these headers break "android" project which 
// is one of the consumers of wc16_wstring.h
// In "android" project, these 2 headers get included below wc16_wstring.h

extern "C" int __cdecl sprintf_s(char * _DstBuf, size_t _DstSize, const char * _Format, ...);
extern "C" int __cdecl swprintf_s(wchar_t * _Dst, size_t _SizeInWords, const wchar_t * _Format, ...);
extern "C" int __cdecl sscanf_s(const char *_Src, const char * _Format, ...);
extern "C" int __cdecl swscanf_s(const wchar_t *_Src, const wchar_t * _Format, ...);


/*Bypassing the ambigous error due to changed llvm string.h */
#define stoi   _plat_stoi  
#define stol   _plat_stol
#define stoul  _plat_stoul
#define stoll  _plat_stoll
#define stoull _plat_stoull
#define stod   _plat_stod
#define stof   _plat_stof
#define stold  _plat_stold

namespace android
{
    // Generic conversion functions
    template <typename _TRetType, typename _TCharType, typename... _TBase>
        _TRetType ConvertFromString(
                _TRetType (*fConverter)(const _TCharType *, _TCharType **, _TBase...),
                const char * const __szCallingFunc,
                const _TCharType * __pStr,
                size_t * __idx,
                _TBase... __base)
        {
            static_assert(
                    (sizeof...(__base) == 1 && std::is_integral<_TRetType>::value)
                    ||
                    (sizeof...(__base) == 0 && std::is_floating_point<_TRetType>::value),
                    "Incorrect Function pointer passed or base not allowed");

            _TCharType * pEnd = nullptr;
            errno = 0;
            _TRetType result = fConverter(__pStr, &pEnd, __base...);

#if (__has_feature(cxx_exceptions))
            if (errno == ERANGE)
                throw std::out_of_range(__szCallingFunc);
            else if (errno == EINVAL || pEnd == __pStr)
                throw std::invalid_argument(__szCallingFunc);
#else
            // Caller can query errno
#endif
            if (pEnd && __idx)
                *__idx = pEnd - __pStr;

            return result;
        }

    template <typename _TIntType>
        std::string to_string_IntType(_TIntType __val, const char * formatter)
        {
            static_assert(std::is_integral<_TIntType>::value, "Pass integral type");
            const size_t arraySize = 4*sizeof(_TIntType);
            char strVal[arraySize];
            sprintf_s(strVal, arraySize, formatter, __val);
            return std::string(strVal);
        }

    template <typename _TFloatType>
        std::string to_string_FloatType(_TFloatType __val, const char * formatter)
        {
            static_assert(std::is_floating_point<_TFloatType>::value, "Pass floating point type");
            const size_t arraySize = std::numeric_limits<_TFloatType>::max_exponent10 + 20;
            char strVal[arraySize];
            sprintf_s(strVal, arraySize, formatter, __val);
            return std::string(strVal);
        }

    // Wrappers
    // strtod/strtof have __attribute__((pcs("aapcs"))) and hence pointers
    // to these functions cannot be passed to the generic template worker
    inline double strtod_wrapper(const char* __str, char** __endptr = 0) 
    {
        return strtod(__str, __endptr);
    }

    inline float strtof_wrapper(const char* __str, char** __endptr = 0) 
    {
        return strtof(__str, __endptr);
    }

    inline long double strtold_wrapper(const char* __str, char** __endptr = 0) 
    {
        long double result = 0;
        int charsConverted = 0;
        int fieldsConverted = sscanf_s(__str, "%Lf%n", &result, &charsConverted);

        if (EOF == fieldsConverted)
        {
             return 0;
        }

        if (__endptr)
        {
            *__endptr = const_cast<char *>(__str + charsConverted);
        }

        return result;
    }

    template <typename _TRetType>
    inline _TRetType wcstoX_wrapper(const wchar_t* __str, const wchar_t* formatter, wchar_t** __endptr = 0) 
    {
        _TRetType result = 0;
        int charsConverted = 0;
		int fieldsConverted = swscanf_s(__str, formatter, &result, &charsConverted);

        if (EOF == fieldsConverted)
        {
            return 0;
        }

        if (__endptr)
        {
            *__endptr = const_cast<wchar_t *>(__str + charsConverted);
        }

        return result;
    }

    inline long long wcstoll(const wchar_t* __str, wchar_t** __endptr, int  /*__base*/) 
    {
        // %lld not working
        return wcstoX_wrapper<long>(__str, L"%ld%n", __endptr);
    }

    inline unsigned long long wcstoull(const wchar_t* __str, wchar_t** __endptr, int  /*__base*/) 
    {
        // %llu not working
        return wcstoX_wrapper<unsigned long>(__str, L"%lu%n", __endptr);
    }

    inline double wcstof(const wchar_t* __str, wchar_t** __endptr) 
    {
        return wcstoX_wrapper<float>(__str, L"%f%n", __endptr);
    }

    inline double wcstod(const wchar_t* __str, wchar_t** __endptr) 
    {
        return wcstoX_wrapper<double>(__str, L"%lf%n", __endptr);
    }

    inline long double wcstold(const wchar_t* __str, wchar_t** __endptr) 
    {
        return wcstoX_wrapper<long double>(__str, L"%Lf%n", __endptr);
    }

    // Definition of to_wstring() are taken
    // from apex\main\winsrc\public\apple\stl\_string_suffix.h
    // 256 should be more than enough for the longest numbers
    enum { k_to_wstring_buff_max = 256 };

    template <typename _Type>
        std::wstring to_wstring_Impl(_Type __val, const wchar_t * formatter)
        {
            wchar_t buf[k_to_wstring_buff_max];
            swprintf_s(buf, k_to_wstring_buff_max, formatter, __val);
            return std::wstring(buf);
        }

    inline std::string to_string_Impl(const std::wstring& wstr)
    {
        size_t strSize = wcstombs(nullptr, wstr.c_str(), 0) + 1;
        std::string str(strSize, '\0');
        // Not checking return type. Contract of string conversion APIs is that their callers have already verified input string
        // wcstombs also does basic parameter validation
        wcstombs(&str[0], wstr.c_str(), strSize);
        return str;
    }
}

namespace std
{
    ///////////////      stoX      /////////////////

    // (string)

    inline int stoi(const string& __str, size_t* __idx = 0, int __base = 10) 
    {
        return static_cast<int>(android::ConvertFromString(&strtol, __func__, __str.c_str(), __idx, __base));
    }

    inline long stol(const string& __str, size_t* __idx = 0, int __base = 10) 
    {
        return android::ConvertFromString(&strtol, __func__, __str.c_str(), __idx, __base);
    }

    inline unsigned long stoul(const string& __str, size_t* __idx = 0, int __base = 10) 
    {
        return android::ConvertFromString(&strtoul, __func__, __str.c_str(), __idx, __base);
    }

    inline long long stoll(const string& __str, size_t* __idx = 0, int __base = 10) 
    {
        return android::ConvertFromString(&strtoll, __func__, __str.c_str(), __idx, __base);
    }

    inline unsigned long long stoull(const std::string& __str, size_t* __idx = 0, int __base = 10)
    {
        return android::ConvertFromString(&strtoull, __func__, __str.c_str(), __idx, __base);
    }

    inline double stod(const string& __str, size_t* __idx = 0) 
    {
        return android::ConvertFromString(&android::strtod_wrapper, __func__, __str.c_str(), __idx);
    }

    inline float stof(const string& __str, size_t* __idx = 0) 
    {
        return android::ConvertFromString(&android::strtof_wrapper, __func__, __str.c_str(), __idx);
    }

    inline long double stold(const string& __str, size_t* __idx = 0) 
    {
        return android::ConvertFromString(&android::strtold_wrapper, __func__, __str.c_str(), __idx);
    }

    // (wstring)

    inline int stoi(const wstring& __str, size_t* __idx = 0, int __base = 10)
    {
        return static_cast<int>(android::ConvertFromString(&wcstol, __func__, __str.c_str(), __idx, __base));
    }

    inline long stol(const wstring& __str, size_t* __idx = 0, int __base = 10)
    {
        return android::ConvertFromString(&wcstol, __func__, __str.c_str(), __idx, __base);
    }

    inline unsigned long stoul(const wstring& __str, size_t* __idx = 0, int __base = 10)
    {
        return android::ConvertFromString(&wcstoul, __func__, __str.c_str(), __idx, __base);
    }

    inline long long stoll(const wstring& __str, size_t* __idx = 0, int __base = 10)
    {
        std::string str{android::to_string_Impl(__str)};
        long long retValue = android::ConvertFromString(&strtoll, __func__, str.c_str(), __idx, __base);
        return retValue;
    }

    inline unsigned long long stoull(const wstring& __str, size_t* __idx = 0, int __base = 10)
    {
        std::string str{android::to_string_Impl(__str)};
        unsigned long long retValue = android::ConvertFromString(&strtoull, __func__, str.c_str(), __idx, __base);
        return retValue;
    }

    inline float stof(const wstring& __str, size_t* __idx = 0) 
    {
        return android::ConvertFromString(&android::wcstof, __func__, __str.c_str(), __idx);
    }

    inline double stod(const wstring& __str, size_t* __idx = 0) 
    {
        return android::ConvertFromString(&android::wcstod, __func__, __str.c_str(), __idx);
    }

    inline long double stold(const wstring& __str, size_t* __idx = 0) 
    {
        return android::ConvertFromString(&android::wcstold, __func__, __str.c_str(), __idx);
    }

    ///////////////      to_[w]string      /////////////////

    // (string)

  /* Now these functions are provided by libc++
    inline string to_string(int __val) 
    {
        return android::to_string_IntType(__val, "%d");
    }

    inline string to_string(unsigned __val) 
    {
        return android::to_string_IntType(__val, "%u");
    }

    inline string to_string(long __val) 
    {
        return android::to_string_IntType(__val, "%ld");
    }

    inline string to_string(unsigned long __val) 
    {
        return android::to_string_IntType(__val, "%lu");
    }

    inline string to_string(long long __val) 
    {
        return android::to_string_IntType(__val, "%lld");
    }

    inline string to_string(unsigned long long __val) 
    {
        return android::to_string_IntType(__val, "%llu");
    }

    inline string to_string(float __val) 
    {
        return android::to_string_FloatType(__val, "%f");
    }

    inline string to_string(double __val) 
    {
        return android::to_string_FloatType(__val, "%f");
    }

    inline string to_string(long double __val) 
    {
        return android::to_string_FloatType(__val, "%Lf");
    }
	*/

    // (wstring)

    inline wstring _wc16_to_wstring(int __val)
    {
        return android::to_wstring_Impl(__val, L"%d");
    }

    inline wstring _wc16_to_wstring(unsigned int __val)
    {
        return android::to_wstring_Impl(__val, L"%u");
    }

    inline wstring _wc16_to_wstring(long __val)
    {
        return android::to_wstring_Impl(__val, L"%ld");
    }

    inline wstring _wc16_to_wstring(unsigned long __val)
    {
        return android::to_wstring_Impl(__val, L"%lu");
    }

    inline wstring _wc16_to_wstring(long long __val)
    {
        return android::to_wstring_Impl(__val, L"%lld");
    }

    inline wstring _wc16_to_wstring(unsigned long long __val)
    {
        return android::to_wstring_Impl(__val, L"%llu");
    }

    inline wstring _wc16_to_wstring(float __val)
    {
        return android::to_wstring_Impl(__val, L"%f");
    }

    inline wstring _wc16_to_wstring(double __val)
    {
        return android::to_wstring_Impl(__val, L"%f");
    }

    inline wstring _wc16_to_wstring(long double __val)
    {
        return android::to_wstring_Impl(__val, L"%Lf");
    }

#ifdef to_wstring
#undef to_wstring
#endif
#define to_wstring _wc16_to_wstring

}

inline long long _wc16_wcstoll(const wchar_t* __str, wchar_t** __endptr, int __base)
{
    return android::wcstoll(__str, __endptr, __base);
}

inline double _wc16_wcstof(const wchar_t* __str, wchar_t** __endptr)
{
    return android::wcstof(__str, __endptr);
}

#ifdef wcstoll
#undef wcstoll
#endif
#define wcstoll _wc16_wcstoll

#ifdef wcstof
#undef wcstof
#endif
#define wcstof _wc16_wcstof
