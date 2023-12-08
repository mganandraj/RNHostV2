/************************************************************\
 wc16 - 2-byte string utilities
 
 The size of wchar_t can vary depending on the compiler, so
 we must avoid it. This namespace provides 2-byte string
 utilities that mimic the wchar utilities. They use the same
 name, but are in the wc16 namespace
 
 Functions have been added as needed. Feel free to add more

 Note that this is similar to the following proposal:
 
 http://www.mail-archive.com/linux-utf8@humbolt.nl.linux.org/msg00000.html
 
 Eventually, we might want to redo this header to use utf16_t and utf32_t, which are arguably
 better names for what we are trying to accomplish.
\************************************************************/

#pragma once

#ifndef MBUSTR_WC16_H
#define MBUSTR_WC16_H

#include <stdlib.h>
#include <stdio.h>

// compile time check to make sure that wchar_t is 16-bit

// extern const int __wchar_check[ sizeof(wchar_t) / (sizeof(wchar_t) == 2) ];

typedef wchar_t   wchar16_t;
typedef wchar16_t wint16_t;

// alias for compatibility with older code
typedef wchar16_t wchar16;
typedef wint16_t  wint16;

#if MBU_DO_ALIGN_TO_MSVC
#include "mbu_align_to_msvc_off.h"
#endif

#ifdef __cplusplus

namespace wc16
{
	// wmem.h
	int      wmemcmp(    const wchar_t* , const wchar_t* , size_t );
	wchar_t* wmemchr(    const wchar_t* src , wchar_t val, size_t n );
	wchar_t* wmemmove(   wchar_t* dst, const wchar_t* src, size_t n );   	
	wchar_t* wmemcpy(    wchar_t* , const wchar_t*, size_t );
	wchar_t* wmemset(    wchar_t* dst, wchar_t val, size_t n );
   
	// wstring.h
	size_t   wcslen(  const wchar_t* str );
	int      wcscmp(  const wchar_t* str1, const wchar_t* str2 );
	int      wcsicmp( const wchar_t* str1, const wchar_t *str2 );
//FIXME(steshaw) 6/6/07 - banned routine needs to be removed
	int      wcsncmp( const wchar_t * str1, const wchar_t * str2, size_t n);
	int      wcsnicmp( const wchar_t * str1, const wchar_t * str2, size_t n);
//FIXME(steshaw) 6/6/07 - banned routine needs to be removed
	wchar_t* wcsncpy( wchar_t* dst, const wchar_t* src, size_t n);
	wchar_t* wcschr(  const wchar_t* str, const wchar_t in_chr);
	wchar_t* wcsstr(  const wchar_t* str, const wchar_t* pat);
	size_t   wcsspn(  const wchar_t* str, const wchar_t* set);
	size_t   wcscspn( const wchar_t* str, const wchar_t* set);

	// wchar.h
	wchar_t  btowc( int );
	wchar_t* _wcslwr( wchar_t *str );
	wchar_t* _wcsupr( wchar_t *str );

	// cwctype
	wchar_t  wc16towlower(wchar_t);
	wchar_t  wc16towupper(wchar_t);
   
  	int      wc16iswpunct( wchar_t );
  	int      wc16iswblank( wchar_t );
  	int      wc16iswspace( wchar_t );
    int      wc16iswdigit( wchar_t );
    int      wc16iswalpha( wchar_t );
    int      wc16iswlower( wchar_t );
    int      wc16iswupper( wchar_t );
    int      wc16iswctype( wchar_t, wchar_t );

	//  cstdlib
	size_t wcstombs(char * s, const wchar_t * pwcs, size_t n);
	size_t mbstowcs(wchar_t * pwcs, const char * s, size_t n);
	
	// wstring.h
//FIXME(steshaw) 6/6/07 - banned routine needs to be removed
	wchar_t * wcsncat(wchar_t *dst, const wchar_t *src, size_t n);
	wchar_t * wcsrchr(const wchar_t * str, wchar_t in_chr);
	
	// wstdio.h
	wchar_t  getwc(FILE *);
	wchar_t  fgetwc(FILE * file);
	wchar_t* fgetws(wchar_t * s, int n, FILE * file);
	
   // wcstoul
	unsigned long  wcstoul( const wchar_t *, wchar_t **, int );
	long           wcstol(  const wchar_t *, wchar_t **, int );
	long long      wcstoll( const wchar_t *, wchar_t **, int );
	unsigned long long wcstoull(const wchar_t *, wchar_t **, int );

} // namespace wc16

#endif //__cplusplus

#ifdef __cplusplus
extern "C"
{
#endif

	// wmem.h
	int wc16_wmemcmp(       const wchar_t * , const wchar_t * , size_t );
	wchar_t *wc16_wmemchr(  const wchar_t *, wchar_t , size_t );
	wchar_t *wc16_wmemmove( wchar_t *, const wchar_t *, size_t );	
	wchar_t *wc16_wmemcpy(  wchar_t * , const wchar_t * , size_t );
	wchar_t *wc16_wmemset(  wchar_t *, wchar_t , size_t );

	// wstring.h
	size_t   wc16_wcslen(   const wchar_t * str );
	int      wc16_wcscmp(   const wchar_t * str1, const wchar_t * str2 );
	int      wc16_wcsicmp(  const wchar_t * str1, const wchar_t * str2 );
//FIXME(steshaw) 6/6/07 - banned routine needs to be removed
	wchar_t* wc16_wcscpy(   wchar_t * dst, const wchar_t * src );
	int      wc16_wcsncmp(  const wchar_t * str1, const wchar_t * str2, size_t n );
	int      wc16_wcsnicmp(  const wchar_t * str1, const wchar_t * str2, size_t n );
//FIXME(steshaw) 6/6/07 - banned routine needs to be removed
	wchar_t* wc16_wcsncpy(  wchar_t * dst, const wchar_t * src, size_t n );
	wchar_t* wc16_wcschr(   const wchar_t * str, const wchar_t in_chr );
	wchar_t* wc16_wcsstr(   const wchar_t * str, const wchar_t * pat );
	size_t   wc16_wcsspn(   const wchar_t * str, const wchar_t * set );
	size_t   wc16_wcscspn(  const wchar_t * str, const wchar_t * set );
   
	// wchar.h
	wchar_t wc16_btowc(    int );

	// cwctype
	wchar_t wc16_towlower(wchar_t);
	wchar_t wc16_towupper(wchar_t);

	//  cstdlib
	size_t wc16_wcstombs(   char* s, const wchar_t* pwcs, size_t n );
	size_t wc16_mbstowcs(   wchar_t* pwcs, const char* s, size_t n );
	
	// wstring.h
//FIXME(steshaw) 6/6/07 - banned routine needs to be removed
	wchar_t* wc16_wcsncat(  wchar_t* dst, const wchar_t *src, size_t n );
	wchar_t* wc16_wcsrchr(  const wchar_t* str, wchar_t in_chr );
	
	// wstdio.h
	wchar_t  wc16_getwc(    FILE * );
	wchar_t  wc16_fgetwc(   FILE * file );
	wchar_t* wc16_fgetws(  wchar_t * s, int n, FILE * file );
	
   // wcstoul
	unsigned long  wc16_wcstoul(  const wchar_t *, wchar_t **, int );
	long           wc16_wcstol(   const wchar_t *, wchar_t **, int );
    long long      wc16_wcstoll(  const wchar_t *, wchar_t **, int );
    unsigned long long wc16_wcstoull(const wchar_t *, wchar_t **, int );

#ifdef __cplusplus
} // extern "C"
#endif

#if MBU_DO_ALIGN_TO_MSVC
#include "mbu_align_to_msvc_on.h"
#endif

#endif // MBUSTR_WC16_H
