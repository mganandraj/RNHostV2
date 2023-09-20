/**
	Legacy macros for set testing
*/
#pragma once

#define MaskSet1(a)					(1UL << (a))
#define MaskSet2(a,b)				(MaskSet1(a) | MaskSet1(b))
#define MaskSet3(a,b,c)				(MaskSet1(a) | MaskSet2(b,c))
#define MaskSet4(a,b,c,d)				(MaskSet1(a) | MaskSet3(b,c,d))
#define MaskSet5(a,b,c,d,e)			(MaskSet1(a) | MaskSet4(b,c,d,e))
#define MaskSet6(a,b,c,d,e,f)			(MaskSet1(a) | MaskSet5(b,c,d,e,f))
#define MaskSet7(a,b,c,d,e,f,g)		(MaskSet1(a) | MaskSet6(b,c,d,e,f,g))
#define MaskSet8(a,b,c,d,e,f,g,h)		(MaskSet1(a) | MaskSet7(b,c,d,e,f,g,h))
#define MaskSet9(a,b,c,d,e,f,g,h,i)		(MaskSet1(a) | MaskSet8(b,c,d,e,f,g,h,i))

// for set {a,b[,c...]} in range 0..31
#define InSetOf(v, mask)	(!((v)&(~0x1fL)) && ((1<<(v)) & (mask)))
#define InSetOf2(v, a,b)				InSetOf(v, MaskSet2(a,b))
#define InSetOf3(v, a,b,c)				InSetOf(v, MaskSet3(a,b,c))
#define InSetOf4(v, a,b,c,d)			InSetOf(v, MaskSet4(a,b,c,d))
#define InSetOf5(v, a,b,c,d,e)			InSetOf(v, MaskSet5(a,b,c,d,e))
#define InSetOf6(v, a,b,c,d,e,f)		InSetOf(v, MaskSet6(a,b,c,d,e,f))
#define InSetOf7(v, a,b,c,d,e,f,g)		InSetOf(v, MaskSet7(a,b,c,d,e,f,g))
#define InSetOf8(v, a,b,c,d,e,f,g,h)	InSetOf(v, MaskSet8(a,b,c,d,e,f,g,h))
#define InSetOf9(v, a,b,c,d,e,f,g,h,i)	InSetOf(v, MaskSet9(a,b,c,d,e,f,g,h,i))

// for set {a,b[,c...]} within 31 of each other, but not 0..31.  'a' must be smallest
#define InBiasSetOf2(v, a,b)				InSetOf2(v-(a), 0,b-(a))
#define InBiasSetOf3(v, a,b,c)				InSetOf3(v-(a), 0,b-(a),c-(a))
#define InBiasSetOf4(v, a,b,c,d)			InSetOf4(v-(a), 0,b-(a),c-(a),d-(a))
#define InBiasSetOf5(v, a,b,c,d,e)			InSetOf5(v-(a), 0,b-(a),c-(a),d-(a),e-(a))
#define InBiasSetOf6(v, a,b,c,d,e,f)		InSetOf6(v-(a), 0,b-(a),c-(a),d-(a),e-(a),f-(a))
#define InBiasSetOf7(v, a,b,c,d,e,f,g)		InSetOf7(v-(a), 0,b-(a),c-(a),d-(a),e-(a),f-(a),g-(a))
#define InBiasSetOf8(v, a,b,c,d,e,f,g,h)	InSetOf8(v-(a), 0,b-(a),c-(a),d-(a),e-(a),f-(a),g-(a),h-(a))
#define InBiasSetOf9(v, a,b,c,d,e,f,g,h,i)	InSetOf9(v-(a), 0,b-(a),c-(a),d-(a),e-(a),f-(a),g-(a),h-(a),i-(a))
