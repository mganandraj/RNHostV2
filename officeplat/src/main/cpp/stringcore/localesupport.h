#pragma once

#include <compileradapters/functiondecorations.h>
#include <platformadapters/windowsfirst.h>

namespace Mso { namespace Platform {

LIBLET_PUBLICAPI UINT MsoGetOEMCP() noexcept;
LIBLET_PUBLICAPI UINT MsoGetACP() noexcept;

}} // Mso::Platform

