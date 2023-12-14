#pragma once
// #include <debugassertapi/debugassertapi.h>

/****************************************************************************
	Unicode Surrogate Pair support functions
*****************************************************************************/

/****************************************************************************
	Surrogate Pair Definitions
	There are two types of surrogates
	1) High Surrogate Normal
	2) Low Surrogate Normal

	Within these two types we have several subsets
	1) EmojiModifierHighSurrogate
	2) EmojiModifierLowSurrogate
	3) RegionalHighSurrogate
	4) RegionalLowSurrogate
 *****************************************************************************/

#define wchHighSurrogateMin                 0xd800
#define wchHighSurrogateMax                 0xdbff
#define wchLowSurrogateMin                  0xdc00
#define wchLowSurrogateMax                  0xdfff
#define wchEmojiModifierMinLowSurrogate     0xdffb
#define wchEmojiModifierMaxLowSurrogate     0xffff
#define wchEmojiModifierHighSurrogate       0xd83c
#define wchRegionalIndicatorMinLowSurrogate 0xdde6
#define wchRegionalIndicatorMaxLowSurrogate 0xddff
#define wchRegionalIndicatorHighSurrogate   0xd83c

/****************************************************************************
	Variant Form Definitions
	There are three types of variant forms:
	1) Variation Selectors
	2) Variation Selector Supplements
	3) Mongolian Variation Selectors
 *****************************************************************************/
#define wchVariationSelectorMin                0xfe00
#define wchVariationSelectorMax                0xfe0f
#define wchVariationSelectorEmoji              0xfe0f
#define wchHighVariationSelectorSupplement     0xdb40
#define wchLowVariationSelectorSupplementMin   0xdd00
#define wchLowVariationSelectorSupplementMax   0xddef
#define wchMongolianMin                        0x180b
#define wchMongolianMax                        0x180d

/****************************************************************************
	Other Definitions
	1) Surrogate Joiners - These characters build ZWJ Sequences
	2) Combining Enlosing Caps - These characters build Combining Sequences
 *****************************************************************************/
#define wchCombiningEnclosingCap            0x20e3
#define wchSurrogateJoiner                  0x200d

__inline  bool MsoFWchEmojiFlagSequenceBetween(const WCHAR* pwch, int cp, int cwch) noexcept {
	if( cp < 2 || cp > cwch - 2 )
	{
		return false;
	}
	return
	( pwch[cp - 2] == wchRegionalIndicatorHighSurrogate ) &&
	( pwch[cp - 1] <= wchRegionalIndicatorMaxLowSurrogate && pwch[cp - 1] >= wchRegionalIndicatorMinLowSurrogate ) &&
	( pwch[cp] == wchRegionalIndicatorHighSurrogate ) &&
	( pwch[cp + 1] <= wchRegionalIndicatorMaxLowSurrogate && pwch[cp + 1] >= wchRegionalIndicatorMinLowSurrogate );
}

__inline bool MsoFWchEmojiModifierHighSurrogate(WCHAR wch) noexcept {
	return (wch == wchEmojiModifierHighSurrogate);
	}

__inline bool MsoFWchRegionalIndicatorHighSurrogate(WCHAR wch) noexcept {
	return (wch == wchRegionalIndicatorHighSurrogate);
	}

__inline bool MsoFWchEmojiModifierLowSurrogate(WCHAR wch) noexcept {
	return ((wch >= wchEmojiModifierMinLowSurrogate) && (wch <= wchEmojiModifierMaxLowSurrogate));
	}

__inline bool MsoFWchRegionalIndicatorLowSurrogate(WCHAR wch) noexcept {
	return ((wch >= wchRegionalIndicatorMinLowSurrogate) && (wch <= wchRegionalIndicatorMaxLowSurrogate));
	}

__inline bool MsoFWchStringVariant(WCHAR wch) noexcept {
	return ((wch >= wchVariationSelectorMin) && (wch <= wchVariationSelectorMax));
	}

_Ret_range_(== , (wch >= wchHighSurrogateMin) && (wch <= wchHighSurrogateMax))
__inline bool MsoFWchHighSurrogate(WCHAR wch) noexcept {
	return ((wch >= wchHighSurrogateMin) && (wch <= wchHighSurrogateMax));
	}

_Ret_range_(==, (wch >= wchLowSurrogateMin) && (wch <= wchLowSurrogateMax))
__inline bool MsoFWchLowSurrogate(WCHAR wch) noexcept {
	return ((wch >= wchLowSurrogateMin) && (wch <= wchLowSurrogateMax));
	}

__inline bool MsoFPwchHighSurrogate(_In_count_c_(2) const WCHAR *pwch) noexcept {
	return MsoFWchHighSurrogate(pwch[0]) && MsoFWchLowSurrogate(pwch[1]);
	}

__inline bool MsoFPwchLowSurrogate(const WCHAR *pwch) noexcept {
#ifdef DEBUG
	// verify valid surrogate pair, assert if malformed string
	if (MsoFWchLowSurrogate(*pwch))
		{
      AssertSzTag(MsoFWchHighSurrogate(*(pwch - 1)), NULL, 0x1f545689 /* tag_5vf0j */);
		}
#endif
	return MsoFWchHighSurrogate(pwch[-1]) && MsoFWchLowSurrogate(pwch[0]);
	}

__inline WCHAR *MsoPwchNext(const WCHAR *pwch) noexcept {
	if (MsoFPwchHighSurrogate(pwch))
		pwch++;
	pwch++;
	return (WCHAR *)pwch;
	}

__inline WCHAR *MsoPwchPrev(const WCHAR *pwch) noexcept {
	pwch--;
	if (MsoFPwchLowSurrogate(pwch))
		pwch--;
	return (WCHAR *)pwch;
}
