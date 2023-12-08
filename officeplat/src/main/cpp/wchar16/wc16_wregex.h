#ifndef WC16_WREGEX_H
#define WC16_WREGEX_H

#include <string>
#include <cctype>
#include <wctype.h>

#include <cwchar>
#include <cstdlib>

#include <utility>

#include "../jniproxy/JString.h"
#include "../jniproxy/JClass.h"
#include "../jniproxy/JVMEnv.h"
#include "../jniproxy/JNIRefUtil.h"
#include "../jniproxy/JniUtility.h"

#include "wc16_wstring.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wheader-hygiene"
using namespace NAndroid;
#pragma clang diagnostic pop
#pragma push_macro("max")
#pragma push_macro("min")
#undef max
#undef min
#include <vector>
#pragma pop_macro("max")
#pragma pop_macro("min")

#ifdef _LIBCPP_REGEX
#error We need to include this header before including regex
#endif // _LIBCPP_REGEX

#ifdef regex_iterator
    #undef regex_iterator
#endif
#define regex_iterator    std_regex_iterator

#ifdef match_results
    #undef match_results
#endif
#define match_results    std_match_results

#ifdef basic_regex
    #undef basic_regex
#endif
#define basic_regex        std_basic_regex

#pragma push_macro("max")
#pragma push_macro("min")
#undef max
#undef min
#include <regex>
#pragma pop_macro("max")
#pragma pop_macro("min")

#undef regex_iterator
#define regex_iterator    w16_regex_iterator

#undef match_results
#define match_results    w16_match_results

#undef basic_regex
#define basic_regex        w16_basic_regex

#ifdef wregex
    #undef wregex
#endif
#define wregex            w16_wregex

#ifdef wsmatch
    #undef wsmatch
#endif
#define wsmatch            w16_wsmatch

#ifdef wcmatch
    #undef wcmatch
#endif
#define wcmatch            w16_wcmatch

namespace wc16 {

template <class _CharT> class basic_regex;

template<>
class basic_regex<wchar_t>
{
public:
    typedef std::regex_constants::syntax_option_type flag_type;
    // constants:
    static const std::regex_constants::syntax_option_type icase = std::regex_constants::icase;
    static const std::regex_constants::syntax_option_type nosubs = std::regex_constants::nosubs;
    static const std::regex_constants::syntax_option_type optimize = std::regex_constants::optimize;
    static const std::regex_constants::syntax_option_type collate = std::regex_constants::collate;
    static const std::regex_constants::syntax_option_type ECMAScript = std::regex_constants::ECMAScript;
    static const std::regex_constants::syntax_option_type basic = std::regex_constants::basic;
    static const std::regex_constants::syntax_option_type extended = std::regex_constants::extended;
    static const std::regex_constants::syntax_option_type awk = std::regex_constants::awk;
    static const std::regex_constants::syntax_option_type grep = std::regex_constants::grep;
    static const std::regex_constants::syntax_option_type egrep = std::regex_constants::egrep;
    
private:
    std::wstring pattern;
    std::regex_constants::syntax_option_type regexOptions;

public:
    basic_regex():pattern()
    {
        regexOptions = std::regex_constants::ECMAScript;
    }

    basic_regex(const std::wstring &_s, std::regex_constants::syntax_option_type _f = std::regex_constants::ECMAScript):pattern(_s)
    {
        regexOptions = _f;
    }

    std::wstring getPattern() const
    {
        return pattern;
    }

    std::regex_constants::syntax_option_type getRegexOptions() const
    {
        return regexOptions;
    }

    void assign(const std::wstring &_s, std::regex_constants::syntax_option_type _f = std::regex_constants::ECMAScript)
    {
        pattern = _s;
        regexOptions = _f;
    }
};

}    // namespace wc16

namespace std {

template <class _CharT>
class w16_basic_regex : public wc16::basic_regex<_CharT>
{
};

template <>
class w16_basic_regex<wchar_t> : public wc16::basic_regex<wchar_t>
{
public:
    w16_basic_regex() = default;
    w16_basic_regex(const std::wstring &_s, std::regex_constants::syntax_option_type _f = std::regex_constants::ECMAScript): wc16::basic_regex<wchar_t>(_s, _f)
    {
    }
};

typedef w16_basic_regex<wchar_t> w16_wregex;

}    // namespace std

namespace wc16 {

// TODO: Fix this stub - Bug 1333710
template<class BidIt> 
struct sub_match : public std::pair<BidIt, BidIt>
{
    std::wstring str() const {return std::wstring();}
};

template <class BidIt>
class match_results
{
public:
    typedef std::vector<sub_match<BidIt>> value_type;
    value_type _result;
    typedef typename value_type::const_iterator const_iterator;
private:
    BidIt position_start;
public:
    match_results():_result(),position_start()
    {
    }
    void set_postion_start(BidIt position)
    {
        position_start = position;
    }
    const sub_match<BidIt>& operator[](unsigned long _n) const
    {
        return _result[_n];
    }
    
    unsigned long size() const {return _result.size();}
    bool empty() const {return size() == 0;}
    
    int position(int _sub = 0) const
    {
        return (*this)[_sub].first - position_start;
    }
    int length(int _sub = 0)const
    {
        int len = (*this)[_sub].second - (*this)[_sub].first;
        return len;
    }
    std::wstring str(int _sub = 0) const
    {
        std::wstring restring((*this)[_sub].first,(*this)[_sub].second);
        return restring;
    }

    // TODO: Fix this stub - Bug 1333710
    bool ready() const 
    {
        return true;
    }

    // TODO: Fix this stub - Bug 1333710
    const sub_match<BidIt>& prefix() const
    {
        return _result[0];
    }

    // TODO: Fix this stub - Bug 1333710
    const sub_match<BidIt>& suffix() const
    {
        return _result[0];
    }

    /*
     *  Gets an iterator to the start of the %sub_match collection.
     */
    const_iterator begin() const
    {
        return _result.begin();
    }

    /**
     * Gets an iterator to the start of the %sub_match collection.
     */
    const_iterator cbegin() const
    {
        return this->begin();
    }

    /**
     * Gets an iterator to one-past-the-end of the collection.
     */
    const_iterator end() const
    {
        return _result.end();
    }

    /**
     * Gets an iterator to one-past-the-end of the collection.
     */
    const_iterator cend() const
    {
        return this->end();
    }
};

}    // namespace wc16

namespace std {

template <class BidIt>
class w16_match_results : public wc16::match_results<BidIt>
{
};

template <>
class w16_match_results<wchar_t *> : public wc16::match_results<wchar_t *>
{
};

template <>
class w16_match_results<const wchar_t *> : public wc16::match_results<const wchar_t *>
{
};

template <>
class w16_match_results<std::w16string::const_iterator> : public wc16::match_results<std::w16string::const_iterator>
{
};

typedef w16_match_results<const wchar_t *> wcmatch; 
typedef w16_match_results<std::w16string::const_iterator> wsmatch;
}    // namespace std

template <typename _TIter>
bool FRegexSearchImplT(_TIter _first, _TIter _last, const wc16::basic_regex<wchar_t>& _regex, wc16::match_results<_TIter> * _matches = nullptr)
{
    JNIEnv *env = nullptr;
    JVMEnv::attachCurrentJNIEnv(&env);
    bool result = false;
    NAndroid::JLocalFrameCleanup localFrameCleaner(env, 2);
    NAndroid::JniUtility::ExceptionCheckAndClear();

    std::wstring inputWString(_first, _last);
    JString inputString(inputWString.c_str());
    JString patternString(_regex.getPattern().c_str());
    static JClass jRegexClass("com/microsoft/office/plat/regex/Regex");
    static jmethodID jRegexCtor = env->GetMethodID(jRegexClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
    JObject jRegexMatchObject(env->NewObject(jRegexClass, jRegexCtor, static_cast<jstring>(patternString), static_cast<jstring>(inputString))); 
    if (JniUtility::ExceptionCheckAndClear())
    {
        return result;
    }

    if (_matches)
    {
       _matches->_result.clear();
       bool retValue = false;

       JniUtility::CallBooleanMethodV(jRegexMatchObject, retValue, "FindNextMatch", "()Z");
       while (true == retValue)
       {
          int startIndex = 0, endIndex = 0, groupCount = 0;
          JniUtility::CallIntMethodV(jRegexMatchObject, startIndex, "GetMatchStart", "()I");
          JniUtility::CallIntMethodV(jRegexMatchObject, endIndex, "GetMatchEnd", "()I");

          wc16::sub_match<_TIter> subMatch;
          subMatch.first = _first + (jint)startIndex;
          subMatch.second = _first + (jint)endIndex;
          _matches->_result.push_back(subMatch);

          JniUtility::CallIntMethodV(jRegexMatchObject, groupCount, "GetGroupCount", "()I");
          if (groupCount > 0)
          {
               /* Index 0 of a regex match object represents matched string.
                  Not specifying any index, by default, picks up matched string only (same as Index 0).
                  Index 1 onwards regex match object contains captured group matches, if any.
                  Group count represents the number of captured group matches */

               int matchingGroupIndex = 1;
               while (matchingGroupIndex <= groupCount)
               {
                   startIndex = 0, endIndex = 0;
                   JniUtility::CallIntMethodV(jRegexMatchObject, startIndex, "GetGroupMatchStart", "(I)I", matchingGroupIndex);
                   JniUtility::CallIntMethodV(jRegexMatchObject, endIndex, "GetGroupMatchEnd", "(I)I", matchingGroupIndex);

                   wc16::sub_match<_TIter> matchGroup;
                   matchGroup.first = _first + (jint)startIndex;
                   matchGroup.second = _first + (jint)endIndex;
                   _matches->_result.push_back(matchGroup);
                   matchingGroupIndex++;
               }
          }

          JniUtility::CallBooleanMethodV(jRegexMatchObject, retValue, "FindNextMatch", "()Z");
       }

       result = (_matches->_result.size() != 0);
   }
   else
   {
       JniUtility::CallBooleanMethodV(jRegexMatchObject, result, "HasMatch", "()Z");
   }

   return result;
}

namespace wc16 {

    inline bool regex_search(const wchar_t* _first,
                             const wchar_t* _last,
                             const wc16::basic_regex<wchar_t>& _regex,
                             std::regex_constants::match_flag_type  /*_flags*/)
    {
        return FRegexSearchImplT(_first, _last, _regex);
    }

    inline bool regex_search(std::wstring::const_iterator _first,
                             std::wstring::const_iterator _last,
                             const wc16::basic_regex<wchar_t>& _regex,
                             std::regex_constants::match_flag_type  /*_flags*/)
    {
        return FRegexSearchImplT(_first, _last, _regex);
    }

    inline bool regex_search(wchar_t* _first, wchar_t* _last,
                             wc16::match_results<wchar_t *>& _matches,
                             const wc16::basic_regex<wchar_t>& _regex,
                             std::regex_constants::match_flag_type  /*_flags*/)
    {
        return FRegexSearchImplT(_first, _last, _regex, &_matches);
    }

    inline bool regex_search(const wchar_t* _first,
                             const wchar_t* _last, wc16::match_results<const wchar_t *>& _matches,
                             const wc16::basic_regex<wchar_t>& _regex,
                             std::regex_constants::match_flag_type  /*_flags*/)
    {
         return FRegexSearchImplT(_first, _last, _regex, &_matches);
    }

    inline bool regex_search(std::wstring::const_iterator _first,
                             std::wstring::const_iterator _last,
                             wc16::match_results<std::wstring::const_iterator>& _matches,
                             const wc16::basic_regex<wchar_t>& _regex, std::regex_constants::match_flag_type  /*_flags*/)
    {
         return FRegexSearchImplT(_first, _last, _regex, &_matches);
    }
}

namespace std {    

// No match_results

inline bool regex_search(const wchar_t* _s, 
                         const wc16::basic_regex<wchar_t>& _e, 
                         std::regex_constants::match_flag_type _flags = std::regex_constants::match_default)
{
      return wc16::regex_search(_s, _s + wc16::wcslen(_s), _e, _flags);
}

inline bool regex_search(const std::wstring& _s, 
                         const wc16::basic_regex<wchar_t>& _e, 
                         std::regex_constants::match_flag_type _flags = std::regex_constants::match_default)
{
    return wc16::regex_search(_s.begin(), _s.end(), _e, _flags);
}

// With extra match_results

inline bool regex_search(std::wstring::const_iterator _first, 
                         std::wstring::const_iterator _last, 
                         wc16::match_results<const wchar_t *>& _m, 
                         const wc16::basic_regex<wchar_t>& _e, 
                         std::regex_constants::match_flag_type _flags = std::regex_constants::match_default)
{
     return wc16::regex_search(&(*_first), &(*_last), _m, _e, _flags);
}

inline bool regex_search(const wchar_t* _s, 
                         wc16::match_results<const wchar_t *>& _m, 
                         const wc16::basic_regex<wchar_t>& _e, 
                         std::regex_constants::match_flag_type _flags = std::regex_constants::match_default)
{
     return wc16::regex_search(_s, _s + wc16::wcslen(_s), _m, _e, _flags);
}

inline bool regex_search(std::wstring& _s, 
                         wc16::match_results<const wchar_t *>& _m, 
                         const wc16::basic_regex<wchar_t>& _e, 
                         std::regex_constants::match_flag_type _flags = std::regex_constants::match_default)
{
     return wc16::regex_search(_s.c_str(), _s.c_str() + _s.length(), _m, _e, _flags);
}

inline bool regex_search(std::wstring& _s, 
                         wc16::match_results<std::wstring::const_iterator> &_m, 
                         const wc16::basic_regex<wchar_t>& _e, 
                         std::regex_constants::match_flag_type _flags = std::regex_constants::match_default)
{
     return wc16::regex_search(_s.begin(), _s.end(), _m, _e, _flags);
}


// TODO : Implementation for regex_match need to be provided (similar to regex_search in wc16_wregex.cpp)
bool regex_match(const wchar_t* _first, const wchar_t* _last, const wc16::basic_regex<wchar_t>& _e, std::regex_constants::match_flag_type _flags = std::regex_constants::match_default);

bool regex_match(std::wstring::const_iterator _first, std::wstring::const_iterator _last, const wc16::basic_regex<wchar_t>& _e, std::regex_constants::match_flag_type _flags = std::regex_constants::match_default);

inline bool regex_match(const wchar_t* _s, const wc16::basic_regex<wchar_t>& _e, std::regex_constants::match_flag_type _flags = std::regex_constants::match_default)
{
    return regex_match(_s, _s + wc16::wcslen(_s), _e, _flags);
}

inline bool regex_match(const std::wstring& _s, const wc16::basic_regex<wchar_t>& _e, std::regex_constants::match_flag_type _flags = std::regex_constants::match_default)
{
    return regex_match(_s.begin(), _s.end(), _e, _flags);
}

// With extra match_results

bool regex_match(const wchar_t* _first, const wchar_t* _last, wc16::match_results<const wchar_t *>& _m, const wc16::basic_regex<wchar_t>& _e, std::regex_constants::match_flag_type _flags = std::regex_constants::match_default);

bool regex_match(wchar_t* _first, wchar_t* _last, wc16::match_results<wchar_t *>& _m, const wc16::basic_regex<wchar_t>& _e, std::regex_constants::match_flag_type _flags = std::regex_constants::match_default);

bool regex_match(std::wstring::const_iterator _first, std::wstring::const_iterator _last, wc16::match_results<std::wstring::const_iterator>& _m, const wc16::basic_regex<wchar_t>& _e, std::regex_constants::match_flag_type _flags = std::regex_constants::match_default);

inline bool regex_match(std::wstring::const_iterator _first, std::wstring::const_iterator _last, wc16::match_results<const wchar_t *>& _m, const wc16::basic_regex<wchar_t>& _e, std::regex_constants::match_flag_type _flags = std::regex_constants::match_default)
{
    return regex_match(&(*_first), &(*_last), _m, _e, _flags);
}

inline bool regex_match(const wchar_t* _s, wc16::match_results<const wchar_t *>& _m, const wc16::basic_regex<wchar_t>& _e, std::regex_constants::match_flag_type _flags = std::regex_constants::match_default)
{
    return regex_match(_s, _s + wc16::wcslen(_s), _m, _e, _flags);
}

inline bool regex_match(std::wstring& _s, wc16::match_results<const wchar_t *>& _m, const wc16::basic_regex<wchar_t>& _e, std::regex_constants::match_flag_type _flags = std::regex_constants::match_default)
{
    return regex_match(_s.c_str(), _s.c_str() + _s.length(), _m, _e, _flags);
}

inline bool regex_match(std::wstring& _s, wc16::match_results<std::wstring::const_iterator> &_m, const wc16::basic_regex<wchar_t>& _e, std::regex_constants::match_flag_type _flags = std::regex_constants::match_default)
{
    return regex_match(_s.begin(), _s.end(), _m, _e, _flags);
}
}    // namespace std

namespace wc16 {

template <class BidIt, class _CharT=wchar_t>
class regex_iterator
{
public:
    typedef std::w16_match_results<BidIt> value_type;
    value_type _match;
    
public:
    regex_iterator() : _match()
    {
    }
    
    regex_iterator(BidIt _a, BidIt _b, const basic_regex<_CharT>& _re, std::regex_constants::match_flag_type _flags = std::regex_constants::match_default)
    {
       wc16::regex_search(_a, _b, _match, _re, _flags);
    }

    const value_type& operator*() const {return _match;}
    const value_type* operator->() const {return &_match;}
    
    bool operator!=(const regex_iterator& _x) const
    {
        return !(*this == _x);
    }
    bool operator==(const regex_iterator& _x) const
    {
        if ((_match.empty()) && (_x._match.empty()))
            return true;
        if ((_match.empty()) || (_x._match.empty()))
            return false;
        return ((_match[0].first == _x._match[0].first) && (_match[0].second == _x._match[0].second));
    }
    regex_iterator & operator++()
    {
        if (!_match.empty())
        {
            _match._result.erase(_match._result.begin());
        }
        
        return *this;
    }
};

}    // namespace wc16

namespace std {

template <class BidIt>
class w16_regex_iterator : public wc16::regex_iterator<BidIt>
{
};

template <>
class w16_regex_iterator<wchar_t *> : public wc16::regex_iterator<wchar_t *, wchar_t>
{
public:
    w16_regex_iterator() = default;
    
    w16_regex_iterator(wchar_t * _a, wchar_t * _b, const basic_regex<wchar_t>& _re, std::regex_constants::match_flag_type _flags = std::regex_constants::match_default)
        : wc16::regex_iterator<wchar_t *, wchar_t>(_a, _b, _re, _flags)
    {
    }
};

template <>
class w16_regex_iterator<const wchar_t *> : public wc16::regex_iterator<const wchar_t *, wchar_t>
{
public:
    w16_regex_iterator() = default;
    
    w16_regex_iterator(const wchar_t * _a, const wchar_t * _b, const basic_regex<wchar_t>& _re, std::regex_constants::match_flag_type _flags = std::regex_constants::match_default) : wc16::regex_iterator<const wchar_t *, wchar_t>(_a, _b, _re, _flags)
    {
    }
};

}    // namespace std

#endif // WC16_WREGEX_H