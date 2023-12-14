/*-----------------------------------------------------------------------------
	Private APIs for Unicode Traits
------------------------------------------------------------------------------*/
#pragma once
// #include <mLang.h>
#include <stringcore/unicodesubranges.h>

#define ASCII           0x007f

/*----------------------------------------------------------------------------
	Shared data structures
----------------------------------------------------------------------------*/
#define nMaxUsrForUsb	8	// maximal number of USRs corresponding to a single USB
LIBLET_PUBLICAPI BYTE UsbToUsr(int usb, int usrmax) noexcept;
LIBLET_PUBLICAPI BYTE UsrToUsb(int usr) noexcept;

/*----------------------------------------------------------------------------
	%%Owner: LevL
	%%Id: 6245d18d-686b-422e-a12c-bb5c28ef651f
----------------------------------------------------------------------------*/
struct usrspan
{
	DWORD ucs4First;
	DWORD ucs4Lim;
};

LIBLET_PUBLICAPI struct usrspan GetUsrSpan(int usr) noexcept;

/*----------------------------------------------------------------------------
	%%Owner: LevL
	%%Id: 80c8a191-6bf1-450f-8b71-61fa37b8c817
----------------------------------------------------------------------------*/
struct UNIPROP
{
	BYTE fNeedsGlyphing     : 1;    // partition needs glyphing
	BYTE fCombiningMark     : 1;    // partition consists of combining marks
	BYTE fZeroWidth         : 1;    // characters in partition have zero width
	BYTE fWhiteBetweenWords : 1;    // white space between words not required
	BYTE fMoveByCluster     : 1;    // The caret cannot be positioned inside of a cluster
	BYTE fUnused            : 3;    // unused bits
};


typedef BYTE CHAR_CLASS;
const DWORD CHAR_CLASS_MAX = 249;

LIBLET_PUBLICAPI UNIPROP GetUniProp(CHAR_CLASS) noexcept;

/*-----------------------------------------------------------------------------
	Given a character return a Unicode character class.  This
	character class implies other properties, such as script id,
	breaking class, etc.

	Note: pccUnicodeClass is a hack table.  For every Unicode page for
	which every codepoint is the same value, the table entry is
	the charclass itself.  Otherwise we have a pointer to a table
	of charclass.
-----------------------------------------------------------------------------*/
LIBLET_PUBLICAPI CHAR_CLASS CharClassFromWch( WCHAR wch ) noexcept;

/*----------------------------------------------------------------------------
	Given a character class, this function returns the proper script id.
----------------------------------------------------------------------------*/
LIBLET_PUBLICAPI SCRIPT_ID SidFromCharClass( CHAR_CLASS cc ) noexcept;

LIBLET_PUBLICAPI SCRIPT_ID SidFromWch(WCHAR wch) noexcept;

