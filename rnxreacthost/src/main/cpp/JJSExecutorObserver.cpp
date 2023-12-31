#include "JJSExecutorObserver.h"

using namespace facebook;

namespace facebook::react {

void JJSExecutorObserver::OnLoaded(const std::string& bundleUrl)
{
  static const auto method = getClass()->getMethod<void(jstring)>("OnBundleLoaded");
  method(self(), jni::make_jstring(bundleUrl).get());
}

void JJSExecutorObserver::OnInitialized()
{
  static const auto method = getClass()->getMethod<void()>("OnInitialized");
  method(self());
}
} // namespace facebook::react