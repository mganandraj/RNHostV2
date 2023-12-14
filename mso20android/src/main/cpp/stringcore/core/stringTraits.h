/**
	stringTraits.h

	Copyright (c) 1994 Microsoft Corporation

	Functions related to string and character characteristics.
*/
#pragma once
#include "msoStringTraits.h"

#include <cstddef>

WORD _WGetCType1Wch(WCHAR wch) noexcept;
WORD _WGetCType3Wch(WCHAR wch) noexcept;
LIBLET_PUBLICAPI bool FChIsTerminator(WCHAR ch, bool fBreakAtPunc) noexcept;
LIBLET_PUBLICAPI bool MsoFIsThaiChar(WCHAR wch) noexcept;

#ifdef DEBUG
bool FAllSimpleRgch(const CHAR *sz, int cch) noexcept;
bool FAllSimpleRgwch(const WCHAR *wz, int cch) noexcept;

__inline bool FAllSimpleSz(const CHAR *sz) noexcept { return FAllSimpleRgch(sz, sz ? (int) strlen(sz) : 0); }
__inline bool FAllSimpleWz(const WCHAR *wz) noexcept { return FAllSimpleRgwch(wz, wz ? (int) wcslen(wz) : 0); }
#endif // DEBUG

#ifdef __cplusplus
namespace Mso { namespace StringTraits {

LIBLET_PUBLICAPI bool MsoIsSurrogateVariationSelector( const WCHAR* pwz, const int & cwch ) noexcept;

namespace Details {
	inline bool	FBetween(ptrdiff_t a, ptrdiff_t b, ptrdiff_t c) noexcept  {return ((b <= a) && (a <= c)); }
}
//Variation Selectors in the Basic Multilingual Plane
inline bool MsoIsBmpVariationSelector( WCHAR wch ) noexcept
{
	return Details::FBetween( wch, 0xFE00, 0xFE0F ) ||
		Details::FBetween( wch, 0x180B, 0x180D );
}

inline bool MsoIsEALigature( WCHAR wch ) noexcept
{
	return Details::FBetween( wch, 0x3099, 0x309A );
}

// East Asian characters requiring rotation in vertical layout
// cf. https://www.unicode.org/Public/vertical/revision-17/VerticalOrientation-17.html
inline bool MsoIsEARotateCharacter(WCHAR wch) noexcept
{
	return Details::FBetween(wch, 0xFF61, 0xFFDF);
}

inline bool MsoIsJoinChar( WCHAR wch ) noexcept
{
	return MsoIsEALigature( wch ) || MsoIsBmpVariationSelector( wch );
}

inline bool MsoIsCombiningEnclosingCap( WCHAR wch ) noexcept
{
	return wch == 0x20E3;
}

inline bool MsoIsZeroWidthJoiningController(WCHAR wch) noexcept
{
	return Details::FBetween(wch, 0x200C, 0x200D);
}

} } // Mso::StringTraits
#endif //__cplusplus
