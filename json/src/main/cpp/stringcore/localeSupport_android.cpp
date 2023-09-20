#include "precomp.h"

namespace Mso { namespace Platform {

#define CP_WESTEUROPE    1252

UINT MsoGetOEMCP() noexcept
{
	return CP_WESTEUROPE;
}

UINT MsoGetACP() noexcept
{
	return CP_WESTEUROPE;
}

}} // Mso::Platform

