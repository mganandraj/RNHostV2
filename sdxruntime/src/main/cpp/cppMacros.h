// clang-format off
/**
	Various low level macros that are globally useful
*/
#pragma once
#ifndef COMPILERADAPTERS_CPPMACROS_H
#define COMPILERADAPTERS_CPPMACROS_H

/**
	Broadly used macros
*/
#ifndef FImplies
#define FImplies(a, b)  (!(a) || (b))
#endif

#ifndef FBiImplies
#define FBiImplies(a, b)  (!(a) == !(b))
#endif

#ifndef RgC
#define RgC(ary) (ary), _countof(ary)
#endif

#define RgCb(ary) (ary),((_countof(ary))*sizeof(ary[0]))

#define NODECORATION_(x) x

/**
	Statement - used for control flow macros like Check, IfFailGo
	Make the macro act like a statement.
*/
#define Statement(x) /*__pragma(warning(push)) __pragma(warning(disable:4127 25037))*/ do { x; } while(0) /*__pragma(warning(pop))*/

/**
	Prevent the compiler from automatically providing implementations of various
	class features. Use the macro in your class public: section.
	TODO: probably need to update with new Move functions
*/
#if !defined (EXPLICIT_SPECIAL_MEMBER_FUNCTIONS)
#define MSO_NO_COPYCONSTR( C )  C(const C&) = delete;
#define MSO_NO_ASSIGNMENT( C ) const C& operator=(const C&) = delete;
#define MSO_NO_DEFAULTCONSTR( C ) explicit C() = delete;

#ifndef DECLARE_COPYCONSTR_AND_ASSIGNMENT
#define DECLARE_COPYCONSTR_AND_ASSIGNMENT( C ) MSO_NO_COPYCONSTR( C ); MSO_NO_ASSIGNMENT( C )
#endif

#else
#define MSO_NO_COPYCONSTR( C ) static_assert(false, "use `" #C "(const " #C "&) = delete;` instead")
#define MSO_NO_ASSIGNMENT( C ) static_assert(false, "use `const " #C "& operator=(const " #C "&) = delete;` instead")
#define MSO_NO_DEFAULTCONSTR( C ) static_assert(false, "use `explicit " #C "() = delete;` instead")

#ifndef DECLARE_COPYCONSTR_AND_ASSIGNMENT
#define DECLARE_COPYCONSTR_AND_ASSIGNMENT( C ) MSO_NO_COPYCONSTR( C ); MSO_NO_ASSIGNMENT( C )
#endif
#endif

/**
	Macros to help share enums between C++ and CX.
	Enums must derive from int (the default)
	Flag enums must derive from unsigned int
*/
#if defined(_WINRT_COMP) && !defined(__clang__)
#define EXPOSE_WINRT_ENUM public
#define EXPOSE_WINRT_FLAGS_ENUM [::Platform::Metadata::Flags] EXPOSE_WINRT_ENUM
#else
#define EXPOSE_WINRT_ENUM
#define EXPOSE_WINRT_FLAGS_ENUM
#endif

/**
	Macros to force placement of symbols into a particular segment.
	For example on iOS ensure large globals aren't placed in the text
	segment because there is a size limit enforced by Apple.
 */
#if MS_TARGET_APPLE
#define DATA_SEGMENT_CONST __attribute__((section("__DATA,__const")))
#else
#define DATA_SEGMENT_CONST
#endif

/**
	workaround MSVC compiler bug
https://developercommunity.visualstudio.com/t/C17-static-member-specialization-req/10385128
https://dev.azure.com/devdiv/DevDiv/_workitems/edit/1831699
*/
#ifdef __clang__
#define STATIC_SPECIALIZATION
#else
#define STATIC_SPECIALIZATION static
#endif // __clang__


/**
	C++20 migration macros
*/
#if _MSC_FULL_VER > 194000000
#error Remove macros and include below now that C++20 has been adopted
#endif
#ifdef MS_TARGET_WINDOWS
#include <vcruntime.h>
#endif
// Bug 7062091: [post C++20] Remove EXPLICIT_THIS from liblet_compileradapters
// https://godbolt.org/z/YsPP5jKrM
#if _HAS_CXX20
#define EXPLICIT_THIS , this
#define INITIALIZE_ATOMIC
#else
#define EXPLICIT_THIS
#define INITIALIZE_ATOMIC ATOMIC_FLAG_INIT
#endif

/**
	Optimizations
*/
#define PRAGMA_OPT_OFF(opt) optimize(#opt, off)
#define PRAGMA_OPT_ON(opt) optimize(#opt, on)
#define PRAGMA_OPT_DEFAULT optimize("", on)
#define PRAGMA_OPT_NONE optimize("", off)
#if !DEBUG
	#ifdef _WIN64
		#define PRAGMA_OPT_SPEED PRAGMA_OPT_ON(gt)
	#else
		#define PRAGMA_OPT_SPEED PRAGMA_OPT_ON(gty)
	#endif
		#define PRAGMA_OPT_SIZE PRAGMA_OPT_DEFAULT
#else
	#define PRAGMA_OPT_SPEED comment(user,"")
	#define PRAGMA_OPT_SIZE comment(user,"")
#endif

#endif // COMPILERADAPTERS_CPPMACROS_H
