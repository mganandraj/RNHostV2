#include "JExecutorObserver.h"

using namespace facebook;

namespace facebook {
namespace react {

void JExecutorObserver::OnLoaded(const std::string& bundleUrl)
{
  static const auto method = getClass()->getMethod<void(jstring)>("OnBundleLoaded");
  method(self(), jni::make_jstring(bundleUrl).get());
}

void JExecutorObserver::OnInitialized()
{
  static const auto method = getClass()->getMethod<void()>("OnInitialized");
  method(self());
}

}}