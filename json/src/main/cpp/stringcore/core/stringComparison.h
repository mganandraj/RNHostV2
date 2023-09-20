/**
	stringComparison.h

	Copyright (c) 1994 Microsoft Corporation

	Private functions related to comparing strings
*/
#pragma once
#include <stringcore/comparison.h>

BOOL FCmpTextEq(const WCHAR *rgwch1, int cch1, const WCHAR *rgwch2, int cch2, int cs) noexcept;
