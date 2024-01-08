/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <fbjni/fbjni.h>
#include "JJsiRuntimeRef.h"

namespace facebook {
namespace react {

struct JRuntimeInstaller : public facebook::jni::JavaClass<JRuntimeInstaller> {
 public:
  static auto constexpr kJavaDescriptor =
      "Lcom/microsoft/office/reactnative/host/RuntimeInstaller;";
  void DoInstall(facebook::jni::local_ref<JJsiRuntimeRef::JavaPart> jsiRuntimeRef);
};

} // namespace react
} // namespace facebook
