#pragma once

#include "IReactInstanceInternal.h"
#include <object/RefCounted.h>
#include <ReactNativeHost/React.h>

#include <vector>

namespace Mso { namespace React {

//constexpr DWORD OnInstanceLoadedErrorTag = MsoReserveTag(0x1e5934e3 /* tag_4wtt9 */);
//constexpr DWORD OnJSExceptionTag = MsoReserveTag(0x1e5926d1 /* tag_4ws1r */);
//constexpr DWORD OnErrorTag = MsoReserveTag(0x1e5926d0 /* tag_4ws1q */);

// This is a factory method and needs to be implemented per platform.
// It is going to create a platform specific react instance and will return that.
Mso::CntPtr<IReactInstance> CreateReactInstance(SDXDescriptor&& descriptor) noexcept;

// Returns a cloned list of currently active RN Instances.
std::vector<Mso::WeakPtr<IReactInstance>> CloneReactInstances() noexcept;

// Each platform has to implement this API to define behavior about whether to create a new RN instance or not.
// Default behavior is to reuse RN instances whenever possible, since this gives us the best perf. But for testing
// and for perf comparison, we still want platforms to be able to have control on creation of a new RN instance,
// even when there is already an existing RN instance serving the same SDXIdentity.
bool ShouldReuseReactInstancesWhenPossible() noexcept;

// To cleanup all leftover WeakPtrs.
void CleanupExpiredInstances() noexcept;

// This is a factory method to be implemented per platform.
// It creates a platform specific react instance.
Mso::CntPtr<IReactInstanceInternal> MakeReactInstance(
	IReactHost& reactHost,
	ReactOptions&& options,
	// Mso::JSHost::IRekaContextProxy& rekaContextProxy,
	Mso::Promise<void>&& whenLoaded) noexcept;

}} // namespace Mso::React
