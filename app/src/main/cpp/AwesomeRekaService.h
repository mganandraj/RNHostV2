//
// Created by anandrag on 9/21/2023.
//

#ifndef ORNHOST_AWESOMEREKASERVICE_H
#define ORNHOST_AWESOMEREKASERVICE_H

#include "precomp.h"
#include "reka/AwesomeRekaDefs.g.h"

struct AwesomeRekaService final : Mso::JSHost::DataService<reacthostapp::NativeService> {
    AwesomeRekaService(Mso::JSHost::IRekaContext &rekaContext) noexcept
            : DataServiceBase(rekaContext) {}
    /*virtual*/ void tryCall(std::string&& input1, int32_t&& input2) noexcept override;
};

#endif //ORNHOST_AWESOMEREKASERVICE_H
