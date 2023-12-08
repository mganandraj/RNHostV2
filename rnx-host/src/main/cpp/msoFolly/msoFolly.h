// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// This file is a helper to allow devmain code to include folly

#include <iostream>

#pragma push_macro("STRICT")
#pragma push_macro("CHECK")
#undef CHECK
#pragma push_macro("Check")
#undef Check
#pragma push_macro("check")
#undef check
#pragma push_macro("max")
#undef max
#pragma push_macro("min")
#undef min
#pragma push_macro("OUT")
#undef OUT
#pragma push_macro("IN")
#undef IN

#if MS_TARGET_APPLE || MS_TARGET_ANDROID
#pragma push_macro("_WIN32")
#undef _WIN32
#endif // MS_TARGET_APPLE || MS_TARGET_ANDROID

/**
	Configuration
*/
#define FOLLY_NO_CONFIG 1

#if MS_TARGET_APPLE

#define FOLLY_MOBILE 1
#define FOLLY_USE_LIBCPP 1

#include <folly/portability.h>
#undef FOLLY_HAS_COROUTINES
#define FOLLY_HAS_COROUTINES 0

#elif MS_TARGET_ANDROID

#define FOLLY_SSE 0
#define FOLLY_SSE_MINOR 0
#define FOLLY_HAVE_MEMRCHR 1
#define FOLLY_NEON 0

#define FOLLY_MOBILE 1
#define FOLLY_USE_LIBCPP 1

#include <folly/portability.h>

#undef FOLLY_HAS_COROUTINES
#define FOLLY_HAS_COROUTINES 0

// STDINCDIRS includes CRTINCDIR, so folly finds string_view, but it doesn't work in droid
// So force FOLLY_HAS_STRING_VIEW back to false by including portability.h ahead of time
// then resetting FOLLY_HAS_STRING_VIEW to 0
#include <folly/portability.h>
#undef FOLLY_HAS_STRING_VIEW
#define FOLLY_HAS_STRING_VIEW 0

#elif MS_TARGET_WINDOWS

// Facebook headers use the __unused clang attribute which MSVC has no
// understanding of. As of this writing, Office pulls in an external definition
// of this to an empty string (where we used to seemingly get something sane).
// Leak an adapter for __unused to anything that includes us. This includes
// many places that include non-Folly React Native headers after us, meaning we
// cannot pop back our previous definition without including more here or
// adding a burden to anyone including headers.
//
// This is particularly gross, but inclusion of raw React Native headers will
// be going away with future efforts, and the potential breakage stemming from
// this is low (something may unintentionally rely on it).
#undef __unused
#define __unused [[maybe_unused]]

#pragma warning(push)
#pragma warning(disable : 4995)
#pragma warning(disable : 4068)
#pragma warning(disable : 4146)
#pragma warning(disable : 4100)
#pragma warning(disable : 4127)
#pragma warning(disable : 4324) // structure was padded due to alignment specifier
#pragma warning(disable : 4365) // signed/unsigned mismatch enabled by switching to -Wall
#pragma warning(disable : 4574)
#pragma warning(disable : 4643) // Forward declaring 'T' in namespace std is not permitted by the C++ Standard.

#define FOLLY_MOBILE 1

#include <folly/portability.h>
#undef FOLLY_HAS_COROUTINES
#define FOLLY_HAS_COROUTINES 0

#else // MS_TARGET_*

#error Unknown target!

#endif // MS_TARGET_*

/**
	Main folly include
*/
#include <folly/dynamic.h>

#if MS_TARGET_APPLE || MS_TARGET_ANDROID
#pragma pop_macro("_WIN32")
#else
#pragma warning(pop)
#endif

#pragma pop_macro("IN")
#pragma pop_macro("OUT")
#pragma pop_macro("min")
#pragma pop_macro("max")
#pragma pop_macro("Check")
#pragma pop_macro("CHECK")
#pragma pop_macro("STRICT")
