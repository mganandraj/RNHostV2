#pragma once

// #include <compileradapters/functiondecorations.h>
// #include <platformadapters/windowsfirst.h>

/****************************************************************************
	Unicode SubRange (USR) definitions
*****************************************************************************/
// Updated according to Unicode 4.1 for Office 12
// Ranges w/o remarks in the last column were defined in Office 11
typedef enum UnicodeSubRanges
{
// BMP
 usrBasicLatin,					//		0		// 0x0020->0x007F
 usrLatin1,					//		1		// 0x00A0->0x00FF
 usrLatinXA,					//		2		// 0x0100->0x017F
 usrLatinXB,					//		3		// 0x0180->0x024F
 usrIPAExtensions,				//		4		// 0x0250->0x02AF
 usrSpacingModLetters,			 	//		5		// 0x02B0->0x02FF
 usrCombDiacritical,				//		6		// 0x0300->0x036F
 usrBasicGreek,					//		7		// 0x0370->0x03CF	unified in Greek
 usrGreekSymbolsCop,				//		8		// 0x03D0->0x03FF	unified in Greek
 usrCyrillic,					//		9		// 0x0400->0x04FF
 usrCyrillicSupp,				//		10		// 0x0500->0x052F	(Unicode 3.2)
 usrArmenian,					//		11		// 0x0530->0x058F
 usrHebrewXA,					//		12		// 0x0590->0x05CF	unified in Hebrew
 usrBasicHebrew,				//		13		// 0x05D0->0x05FF	unified in Hebrew
 usrBasicArabic,				//		14		// 0x0600->0x0670	unified in Arabic
 usrArabicX,					//		15		// 0x0671->0x06FF	unified in Arabic
 usrSyriac,					//		16		// 0x0700->0x074F
 usrArabicSupp,					//		17		// 0x0750->0x077F	(Unicode 4.1)
 usrThaana,					//		18		// 0x0780->0x07BF
 usrDevangari,					//		19		// 0x0900->0x097F
 usrBengali,					//		20		// 0x0980->0x09FF
 usrGurmukhi,					//		21		// 0x0A00->0x0A7F
 usrGujarati,					//		22		// 0x0A80->0x0AFF
 usrOriya,					//		23		// 0x0B00->0x0B7F
 usrTamil,					//		24		// 0x0B80->0x0BFF
 usrTelugu,					//		25		// 0x0C00->0x0C7F
 usrKannada,					//		26		// 0x0C80->0x0CFF
 usrMalayalam,					//		27		// 0x0D00->0x0D7F
 usrSinhala,					//		28		// 0x0D80->0x0DFF
 usrThai,					//		29		// 0x0E00->0x0E7F
 usrLao,					//		30		// 0x0E80->0x0EFF
 usrTibetan,					//		31		// 0x0F00->0x0FFF
 usrMyanmar,					//		32		// 0x1000->0x109F
 usrGeorgianExtended,				//		33		// 0x10A0->0x10CF	unified in Georgian
 usrBasicGeorgian,				//		34		// 0x10D0->0x10FF	unified in Georgian
 usrHangulJamo,					//		35		// 0x1100->0x11FF
 usrEthiopic,					//		36		// 0x1200->0x137F
 usrEthiopicSupp,				//		37		// 0x1380->0x139F	(Unicode 4.1)
 usrCherokee,					//		38		// 0x13A0->0x13FF
 usrCanadianAbor,				//		39		// 0x1400->0x167F
 usrOgham,					//		40		// 0x1680->0x169F
 usrRunic,					//		41		// 0x16A0->0x16FF
 usrTagalog,					//		42		// 0x1700->0x171F	(Unicode 3.2)
 usrHanunoo,					//		43		// 0x1720->0x173F	(Unicode 3.2)
 usrBuhid,					//		44		// 0x1740->0x175F	(Unicode 3.2)
 usrTagbanwa,					//		45		// 0x1760->0x177F	(Unicode 3.2)
 usrKhmer,					//		46		// 0x1780->0x17FF
 usrMongolian,					//		47		// 0x1800->0x18AF
 usrLimbu,					//		48		// 0x1900->0x194F	(Unicode 4.0)
 usrTaiLe,					//		49		// 0x1950->0x197F	(Unicode 4.0)
 usrNewTaiLue,					//		50		// 0x1980->0x19DF	(Unicode 4.1)
 usrKhmerSymbols,				//		51		// 0x19E0->0x19FF	(Unicode 4.0)
 usrBuginese,					//		52		// 0x1A00->0x1A1F	(Unicode 4.1)
 usrPhoneticExtensions,				//		53		// 0x1D00->0x1D7F	(Unicode 4.0)
 usrPhoneticExtensionsSupp,			//		54		// 0x1D80->0x19BF	(Unicode 4.1)
 usrCombDiacriticalMarksSupp,			//		55		// 0x1DC0->0x1DFF	(Unicode 4.1)
 usrLatinExtendedAdd,				//		56		// 0x1E00->0x1EFF
 usrGreekExtended,				//		57		// 0x1F00->0x1FFF
 usrGeneralPunct,				//		58		// 0x2000->0x206F
 usrSuperAndSubscript,				//		59		// 0x2070->0x209F
 usrCurrencySymbols,				//		60		// 0x20A0->0x20CF
 usrCombDiacriticsS,				//		61		// 0x20D0->0x20FF
 usrLetterlikeSymbols,				//		62		// 0x2100->0x214F
 usrNumberForms,				//		63		// 0x2150->0x218F
 usrArrows,					//		64		// 0x2190->0x21FF
 usrMathematicalOps,				//		65		// 0x2200->0x22FF
 usrMiscTechnical,				//		66		// 0x2300->0x23FF
 usrControlPictures,				//		67		// 0x2400->0x243F
 usrOpticalCharRecog,				//		68		// 0x2440->0x245F
 usrEnclosedAlphanum,				//		69		// 0x2460->0x24FF
 usrBoxDrawing,					//		70		// 0x2500->0x257F
 usrBlockElements,				//		71		// 0x2580->0x259F
 usrGeometricShapes,				//		72		// 0x25A0->0x25FF
 usrMiscDingbats,				//		73		// 0x2600->0x26FF	correct: Misc Symbols
 usrDingbats,					//		74		// 0x2700->0x27BF
 usrMiscMathSymbolsA,				//		75		// 0x27C0->0x27EF	(Unicode 3.2)
 usrSuppArrowsA,				//		76		// 0x27F0->0x27FF	(Unicode 3.2)
 usrBraillePatterns,				//		77		// 0x2800->0x28FF
 usrSuppArrowsB,				//		78		// 0x2900->0x297F	(Unicode 3.2)
 usrMiscMathSymbolsB,				//		79		// 0x2980->0x29FF	(Unicode 3.2)
 usrMiscMathOperators,				//		80		// 0x2A00->0x2AFF	(Unicode 3.2)
 usrMiscSymbolsAndArrows,			//		81		// 0x2B00->0x2BFF	(Unicode 4.0)
 usrGlagolitic,					//		82		// 0x2C00->0x2C5F	(Uniocde 4.1)
 usrCoptic,					//		83		// 0x2C80->0x2CFF	(Unicode 4.1)
 usrGeorgianSupp,				//		84		// 0x2D00->0x2D2F	(Unicode 4.1)
 usrTifinagh,					//		85		// 0x2D30->0x2D7F	(Unicode 4.1)
 usrEthiopicExtended,				//		86		// 0x2D80->0x2DDF	(Unicode 4.1)
 usrSuppPunctuation,				//		87		// 0x2E00->0x2E7F	(Unicode 4.1)
 usrCJKRadicalsSupp,				//		88		// 0x2E80->0x2EFF
 usrKangxiRadicals,				//		89		// 0x2F00->0x2FDF
 usrIdeographicDesc,				//		90		// 0x2FF0->0x2FFF
 usrCJKSymAndPunct,				//		91		// 0x3000->0x303F
 usrHiragana,					//		92		// 0x3040->0x309F
 usrKatakana,					//		93		// 0x30A0->0x30FF
 usrBopomofo,					//		94		// 0x3100->0x312F
 usrHangulCompatJamo,				//		95		// 0x3130->0x318F
 usrCJKMisc, 					//		96		// 0x3190->0x319F	correct: Kanbun
 usrExtendedBopomofo,				//		97		// 0x31A0->0x31BF	(Unicode 3.2)
 usrCJKStrokes,					//		98		// 0x31C0->0x31EF	(Unicode 4.1)
 usrKatakanaPhoneticExt,			//		99		// 0x31F0->0x31FF	(Unicode 3.2)
 usrEnclosedCJKLtMnth,				//		100		// 0x3200->0x32FF
 usrCJKCompatibility,				//		101		// 0x3300->0x33FF
 usrCJKUnifiedIdeoExtA,				//		102		// 0x3400->0x4DBF
 usrYijingHexagramSymbols,			//		103		// 0x4DC0->0x4DFF	(Unicode 4.0)
 usrCJKUnifiedIdeo,				//		104		// 0x4E00->0x9FFF
 usrYi,						//		105		// 0xA000->0xA48F
 usrYiRadicals,					//		106		// 0xA490->0xA4CF
 usrModifierToneLetters,			//		107		// 0xA700->0xA71F	(Unicode 4.1)
 usrSylotiNagri,				//		108		// 0xA800->0xA82F	(Unicode 4.1)
 usrHangulJamoExtendedA,			//		109		// 0xA960->0xA97F
 usrHangul,					//		110		// 0xAC00->0xD7AF
 usrHangulJamoExtendedB,			//		111		// 0xD7B0->0xD7FF
 usrHighSurrogate,				//		112		// 0xD800->0xDB7F
 usrHighPrivateUseSurrogate,			//		113		// 0xDB80->0xDBFF	split from High Surrogates
 usrLowSurrogate,				//		114		// 0xDC00->0xDFFF
 usrPrivateUseArea,				//		115		// 0xE000->0xF8FF
 usrCJKCompatibilityIdeographs,			//		116		// 0xF900->0xFA5F
 usrAlphaPresentationForms,			//		117		// 0xFB00->0xFB4F
 usrArabicPresentationFormsA,			//		118		// 0xFB50->0xFDFF
 usrVariationSelectors,				//		119		// 0xFE00->0xFE0F	(Unicode 3.2)
 usrVerticalForms,				//		120		// 0xFE10->0xFE1F	(Unicode 4.1)
 usrCombiningHalfMarks,				//		121		// 0xFE20->0xFE2F
 usrCJKCompatForms,				//		122		// 0xFE30->0xFE4F
 usrSmallFormVariants,				//		123		// 0xFE50->0xFE6F
 usrArabicPresentationFormsB,			//		124		// 0xFE70->0xFEFE
 usrHFWidthForms,				//		125		// 0xFF00->0xFFEF
 usrSpecials,					//		126		// 0xFFF0->0xFFFD
// Plane 1
 usrLinearBSyllabary,				//		127		// 0x10000->0x1007F	(Unicode 4.0)
 usrLinearBIdeograms,				//		128		// 0x10080->0x100FF	(Unicode 4.0)
 usrAegeanNumbers,				//		129		// 0x10100->0x1013F	(Unicode 4.0)
 usrAncientGreekNumbers,			//		130		// 0x10140->0x1018F	(Unicode 4.1)
 usrOldItalic,					//		131		// 0x10300->0x1032F	(Unicode 3.1)
 usrGothic,					//		132		// 0x10330->0x1034F	(Unicode 3.1)
 usrUgaritic,					//		133		// 0x10380->0x1039F	(Unicode 4.0)
 usrOldPersian,					//		134		// 0x103A0->0x103DF	(Unicode 4.1)
 usrDeseret,					//		135		// 0x10400->0x1044F	(Unicode 3.1)
 usrShavian,					//		136		// 0x10450->0x1047F	(Unicode 4.0)
 usrOsmania,					//		137		// 0x10480->0x104AF	(Unicode 4.0)
 usrCypriotSyllabary,				//		138		// 0x10800->0x1083F	(Unicode 4.0)
 usrKharoshthi,					//		139		// 0x10A00->0x10A5F	(Unicode 4.1)
 usrByzantineMusicalSymbols,			//		140		// 0x1D000->0x1D0FF	(Unicode 3.1)
 usrMusicalSymbols,				//		141		// 0x1D100->0x1D1FF	(Unicode 3.1)
 usrAncientGreekMusicalNotation, 		//		142		// 0x1D200->0x1D24F	(Unicode 4.1)
 usrTaiXuanJingSymbols,				//		143		// 0x1D300->0x1D35F	(Unicode 4.0)
 usrMathAlphanumSymbols,			//		144		// 0x1D400->0x1D7FF	(Unicode 3.1)
// Plane 2
 usrCJKUnifiedIdeoExtB,				//		145		// 0x20000->0x2A6DF	(Unicode 4.0)
// TODO: Add more ranges (CJK Unified Idoegraphs Ext C...F
 usrCJKCompatibilityIdeoSupp,			//		146		// 0x2F800->0x2FA1F	(Unicode 4.0)
// TODO: Add more ranges (CJK Unified Idoegraphs Ext G, H
// Plane 14
 usrTags,					//		147		// 0xE0000->0xE007F	(Unicode 3.1)
 usrVariationSelectorsSupp,			//		148		// 0xE0100->0xE01EF	(Unicode 4.0)
// Plane 15
 usrSuppPrivateUseAreaA,			//		149		// 0xF0000->0xFFFFF	(Unicode 3.1)
// Plane 16
 usrSuppPrivateUseAreaB,			//		150		// 0x100000->10FFFF	(Unicode 3.1)
// sentinel
 usrMax						//		151		// N/A			enum sentinel
} UnicodeSubRanges;

LIBLET_PUBLICAPI int MsoUsrFromWch(WCHAR) noexcept;
LIBLET_PUBLICAPI bool MsoFWchInUsr(WCHAR wch, int usr) noexcept;
