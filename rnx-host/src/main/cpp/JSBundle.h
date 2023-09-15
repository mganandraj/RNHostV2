//
// Created by anandrag on 9/15/2023.
//

#ifndef ORNHOST_JSBUNDLE_H
#define ORNHOST_JSBUNDLE_H

#include <string>

struct JSBundleInfo
{
    std::string Id;
    std::string FileName;
    uint64_t Timestamp;
};

struct IJSBundle
{
    virtual std::string_view Content() noexcept = 0;
    virtual JSBundleInfo Info() noexcept = 0;
};

#endif //ORNHOST_JSBUNDLE_H
