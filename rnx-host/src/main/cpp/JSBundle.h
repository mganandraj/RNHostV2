#pragma once

#include <string>

namespace facebook::react {

struct JSBundleInfo {
    std::string Id;
    std::string FileName;
    uint64_t Timestamp;
};

struct IJSBundle {
    virtual std::string_view Content() noexcept = 0;
    virtual JSBundleInfo Info() noexcept = 0;
    virtual ~IJSBundle() {}
};

} // namespace facebook::react
