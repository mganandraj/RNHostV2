#include "JReactInstance.h"

namespace Mso::React {

JReactInstance::JReactInstance(IReactHost& reactHost, ReactOptions&& options) noexcept
        : Super{reactHost.NativeQueue()}
        , m_weakReactHost(&reactHost)
        , m_options{std::move(options)}
{
}

JReactInstance::~JReactInstance() noexcept
{
    Destroy();
}

void JReactInstance::Initialize() noexcept
{
    // Create Java instance and keep the hybrid object.
}

//! This method must be called from the native queue.
Mso::Future<void> JReactInstance::Destroy() noexcept
{
    // Destroy the hybrid object
    return Mso::MakeSucceededFuture();
}

const std::string& JReactInstance::JsBundleName() const noexcept
{
    return m_options.Identity;
}

std::string JReactInstance::SDXBasePath() const noexcept
{
    return m_options.SDXBasePath;
}

const ReactOptions& JReactInstance::Options() const noexcept
{
    return m_options;
}

}; // namespace Mso::React
