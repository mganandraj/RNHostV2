#include "JOfficeExecutorObserver.h"

using namespace facebook;

namespace facebook {
namespace react {

void JOfficeExecutorObserver::OnLoaded(const std::string& bundleUrl)
{
  static const auto method = getClass()->getMethod<void(jstring)>("OnBundleLoaded");
  method(self(), jni::make_jstring(bundleUrl).get());
}

void JOfficeExecutorObserver::OnInitialized()
{
  static const auto method = getClass()->getMethod<void()>("OnInitialized");
  method(self());
}

}}