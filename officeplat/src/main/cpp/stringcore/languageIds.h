#pragma once
/*************************************************************************
	Language ID's (lid's) supported in the previous versions of Office
*************************************************************************/
#define lidUnknown				0xffff
#define lidNone					0x0000
#define lidNoProof				0x0400
// Locale names that are valid but not associated with a given LCID MAY be assigned the LCID Language ID 0x1000
// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-lcid/926e694f-1797-4418-a922-343d1c5e91a6
#define lidOther				0x1000
// listed in order of PRIMARYLANGID, then SUBLANGID
#define lidArabic				0x0401
#define lidIraq					0x0801
#define lidEgyptian				0x0c01
#define lidLibya				0x1001
#define lidAlgerian				0x1401
#define lidMorocco				0x1801
#define lidTunisia				0x1c01
#define lidOman					0x2001
#define lidYemen				0x2401
#define lidSyria				0x2801
#define lidJordan				0x2c01
#define lidLebanon				0x3001
#define lidKuwait				0x3401
#define lidUAE					0x3801
#define lidBahrain				0x3c01
#define lidQatar				0x4001
#define lidBulgarian			0x0402
#define lidCatalan				0x0403
#define lidValencian			0x0803
#define lidTaiwan				0x0404	// Traditional Chinese
#define lidChina				0x0804	// Simplified Chinese
#define lidHongkong				0x0c04	// Mixed Chinese
#define lidSingapore			0x1004
#define lidMacaoSAR				0x1404
#define lidCzech				0x0405
#define lidDanish				0x0406
#define lidGerman				0x0407
#define lidSwissGerman			0x0807
#define lidAustrianGerman		0x0c07
#define lidGermanLuxembourg		0x1007
#define lidGermanLiechtenstein	0x1407
#define lidGreek				0x0408
#define lidAmerican 			0x0409	// English (U.S.)
#define lidBritish				0x0809	// English (U.K.)
#define lidAustralian			0x0c09	// English (Australian)
#define lidCanadianEnglish		0x1009	// English (Canadian)
#define lidEnglishNewZealand	0x1409
#define lidEnglishIreland		0x1809
#define lidEnglishSouthAfrica	0x1c09
#define lidEnglishJamaica		0x2009
#define lidEnglishCaribbean		0x2409
#define lidEnglishBelize		0x2809
#define lidEnglishTrinidad		0x2c09
#define lidEnglishZimbabwe		0x3009
#define lidEnglishPhilippines	0x3409
#define lidEnglishIndonesia		0x3809
#define lidEnglishHongKong		0x3c09
#define lidEnglishIndia			0x4009
#define lidEnglishMalaysia		0x4409
#define lidEnglishSingapore		0x4809
#define lidSpanish				0x040a	// Traditional Spanish
#define lidSpanishCA			0x080a	// Central American Spanish
#define lidMexican				0x080a
#define lidSpanishModern		0x0c0a	// Modern Spanish
#define lidGuatemala			0x100a
#define lidCostaRica			0x140a
#define lidPanama				0x180a
#define lidDominicanRepublic	0x1c0a
#define lidSpanishSA			0x200a	// South American Spanish
#define lidVenezuela			0x200a
#define lidColombia				0x240a
#define lidPeru					0x280a
#define lidArgentina			0x2c0a
#define lidEcuador				0x300a
#define lidChile				0x340a
#define lidUruguay				0x380a
#define lidParguay				0x3c0a
#define lidBolivia				0x400a
#define lidElSalvador			0x440a
#define lidHonduras				0x480a
#define lidNicaragua			0x4c0a
#define lidPuertoRico			0x500a
#define lidSpanishUS			0x540a
#define lidFinnish				0x040b
#define lidFrench				0x040c
#define lidFrenchBelgian		0x080c
#define lidFrenchCanadian		0x0c0c
#define lidFrenchSwiss			0x100c
#define lidFrenchLuxembourg		0x140c
#define lidFrenchMonaco			0x180c
#define lidFrenchWestIndies		0x1c0c
#define lidFrenchReunion		0x200c
#define lidFrenchCongoDRC		0x240c
#define lidFrenchSenegal		0x280c
#define lidFrenchCameroon		0x2c0c
#define lidFrenchCotedIvoire	0x300c
#define lidFrenchMali			0x340c
#define lidFrenchMorocco		0x380c
#define lidFrenchHaiti			0x3c0c
#define lidHebrew				0x040d
#define lidHungarian			0x040e
#define lidIcelandic			0x040f
#define lidItalian				0x0410
#define lidItalianSwiss			0x0810
#define lidJapanese				0x0411
#define lidKorean				0x0412
#define lidKoreanJohab			0x0812
//	*** the only one with SUBLANGID == 0!! ***
#define lidDutchPreferred		0x0013
#define lidDutch				0x0413
#define lidDutchBelgian			0x0813
#define lidNorskBokmal			0x0414
#define lidNorskNynorsk			0x0814
#define lidPolish				0x0415
#define lidPortBrazil			0x0416
#define lidPortIberian			0x0816
#define lidRhaetoRomanic		0x0417
#define lidRomanian				0x0418
#define lidRomanianMoldavia		0x0818
#define lidRussian				0x0419
#define lidRussianMoldavia		0x0819
#define lidCroatian				0x041a
#define lidSerbianLatinSerbMont				0x081a
#define lidSerbianCyrillicSerbMont			0x0c1a
#define lidBosniaHerzegovina				0x101a
#define lidBosnianBosniaHerzegovinaLatin	0x141a
#define lidSerbianBosniaHerzegovinaLatin	0x181a
#define lidSerbianBosniaHerzegovinaCyrillic	0x1c1a
#define lidBosnianBosniaHerzegovinaCyrillic	0x201a
#define lidSerbianLatin						0x241a
#define lidSerbianCyrillic					0x281a
#define lidSerbianMontenegroLatin			0x2c1a
#define lidSerbianMontenegroCyrillic		0x301a
#define lidSlovak				0x041b
#define lidAlbanian				0x041c
#define lidSwedish				0x041d
#define lidSwedishFinland		0x081d
#define lidThai					0x041e	// Thailand, complex script
#define lidTurkish				0x041f
#define lidUrdu					0x0420	// Indic Language; Arabic script
#define lidUrduIndia			0x0820
#define lidIndonesian			0x0421
#define lidUkrainian			0x0422	// Former `USSR
#define lidByelorussian			0x0423	// Former `USSR
#define lidSlovenian			0x0424
#define lidEstonian				0x0425	// Former `USSR
#define lidLatvian				0x0426	// Former `USSR
#define lidLithuanian			0x0427	// Former `USSR
#define lidLithuanianTrad		0x0827
#define lidTajik				0x0428
#define lidFarsi				0x0429	// Arabic script
#define lidVietnamese			0x042a
#define lidArmenian				0x042b	// Former `USSR
#define lidAzeriLatin			0x042c	// Former `USSR
#define lidAzeriCyrillic		0x082c	// Former `USSR
#define lidBasque				0x042d
#define lidSorbian				0x042e
#define lidLowerSorbian			0x082e
#define lidMacedonianFYROM		0x042f
#define lidSutu					0x0430
#define lidTsonga				0x0431
#define lidTswana				0x0432
#define lidVenda				0x0433
#define lidXhosa				0x0434
#define lidZulu					0x0435
#define lidAfrikaans			0x0436
#define lidGeorgian				0x0437	// Former `USSR
#define lidFaeroese				0x0438
#define lidHindi				0x0439	// Indic Language
#define lidMaltese				0x043a
#define lidSamiLappish			0x043b
#define lidNorthSamiSwe			0x083b
#define lidNorthernSamiFi		0x0c3b
#define lidLuleSamiNor			0x103b
#define lidLuleSamiSwe			0x143b
#define lidSouthSamiNor			0x183b
#define lidSouthSamiSwe			0x1c3b
#define lidSkoltSami			0x203b
#define lidInariSami			0x243b
#define lidGaelicIrish			0x083c
#define lidYiddish				0x043d
#define lidMalaysian			0x043e
#define lidMalayBrunei			0x083e
#define lidKazakh				0x043f	// Former `USSR
#define lidKyrgyz				0x0440
#define lidSwahili				0x0441
#define lidTurkmen				0x0442
#define lidUzbekLatin			0x0443	// Former `USSR
#define lidUzbekCyrillic		0x0843	// Former `USSR
#define lidTatar				0x0444	// Former `USSR
#define lidBengali				0x0445	// Indic Language
#define lidBengaliBangladesh	0x0845
#define lidPunjabi				0x0446	// Indic Language
#define lidPunjabiPakistan		0x0846
#define lidGujarati				0x0447	// Indic Language
#define lidOriya				0x0448	// Indic Language
#define lidTamil				0x0449	// Indic Language
#define lidTamilSriLanka		0x0849	// Indic Language
#define lidTelugu				0x044a	// Indic Language
#define lidKannada				0x044b	// Indic Language
#define lidMalayalam			0x044c	// Indic Language
#define lidAssamese				0x044d	// Indic Language
#define lidMarathi				0x044e	// Indic Language
#define lidSanskrit				0x044f	// Indic Language
#define lidMongolian			0x0450	// Mongolian Cylliric
#define lidMongolianMongo		0x0850	// Mongolian Mongolian
#define lidTibetan				0x0451
#define lidBhutanese			0x0851
#define lidWelsh				0x0452
#define lidKhmer				0x0453
#define lidLao					0x0454
#define lidBurmese				0x0455
#define lidGalician				0x0456
#define lidKonkani				0x0457	// Indic Language
#define lidManipuri				0x0458	// Indic Language
#define lidSindhi				0x0459	// Indic Language
#define lidSindhiPakistan		0x0859	// Indic Language - Arabic script
#define lidSyriac				0x045a
#define lidSinhalese			0x045b
#define lidCherokee				0x045c
#define lidInuktitut			0x045d
#define lidInuktitutLatin		0x085d
#define lidAmharic				0x045e
#define lidTamazight			0x045f	// Berber - Arabic script
#define lidTamazightLatin		0x085f	// Berber - Latin script
#define lidKashmiri				0x0460	// Indic Language
#define lidKashmiriIndia		0x0860
#define lidNepali				0x0461	// Indic Language
#define lidNepaliIndia			0x0861
#define lidFrisian				0x0462
#define lidPashto				0x0463
#define lidFilipino				0x0464	// Philippines
#define lidMaldivian			0x0465	// Thaana script
#define lidEdo					0x0466
#define lidFulfulde				0x0467
#define lidHausa				0x0468
#define lidIbibio				0x0469
#define lidYoruba				0x046a
#define lidQuechuaBolivia		0x046b
#define lidQuechuaEcuador		0x086b
#define lidQuechuaPeru			0x0c6b
#define lidSesothoSaLeboa		0x046c
#define lidBashkir				0x046d
#define lidLuxembourgish		0x046e
#define lidGreenlandic			0x046f
#define lidIgbo					0x0470
#define lidKanuri				0x0471
#define lidOromo				0x0472
#define lidTigrignaEthiopic		0x0473
#define lidTigrignaEritrea		0x0873
#define lidGuarani				0x0474
#define lidHawaiian				0x0475
#define lidLatin				0x0476
#define lidSomali				0x0477
#define lidYi					0x0478
#define lidPapiamentu			0x0479
#define lidMapudungun			0x047a
#define lidMohawk				0x047c
#define lidBreton				0x047e
#define lidMaori				0x0481
#define lidOccitan				0x0482
#define lidCorsican				0x0483
#define lidAlsatian				0x0484
#define lidSakha				0x0485
#define lidKiche				0x0486
#define lidKinyarwanda			0x0487
#define lidWolof				0x0488
#define lidGaelicScots			0x0491
// Update sublangs[] in FValidLegacyLid(), if you add new legacy language above!!!
// The defines below added for Office 12 (not reflected in sublangs[])
#define lidUighur				0x0480
#define lidDari					0x048c
#define lidSetswana				0x0832
#define lidFulah				0x0867 // Fulah (Latin, Senegal)
#define lidCustomCurrent		0x0c00	// custom current locale/language (Vista feature)
// The defines below added for Office 15 (not reflected in sublangs[])
#define lidChineseSimplified	0x0004	// "zh-Hans", Chinese, Simplified script.
#define lidChineseUnspecified	0x7804	// "zh", Chinese, script not specified, fall back to Simplified script.
#define lidChineseTraditional	0x7c04	// "zh-Hant", Chinese, Traditional script.

// macro to get LangBase from a (sub)language (the lower 10 bits of lid)
#define ilLangBase(lid)	(0x3ff & (lid))

// highest value for the lower 10 bits of lid
#define lidBaseHi		0x03ff
// highest value for the upper 6 bits of lid
#define lidSubHi		0x003f



