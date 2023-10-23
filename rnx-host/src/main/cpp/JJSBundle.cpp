#include "JJSBundle.h"
#include <fbjni/ByteBuffer.h>

using namespace facebook::jni;

namespace facebook::react {

struct ByteBufferJSBundle : IJSBundle {
public:
    std::string_view Content() noexcept override;
    JSBundleInfo Info() noexcept override;
    ByteBufferJSBundle(JSBundleInfo &&info, local_ref<JByteBuffer>) noexcept;
    uint64_t Timestamp() noexcept;

private:
    JSBundleInfo m_info;
    global_ref<JByteBuffer> m_byteBuffer; // Owns the java ByteBuffer

};

ByteBufferJSBundle::ByteBufferJSBundle(
        JSBundleInfo &&info,
        local_ref<JByteBuffer> content) noexcept
        : m_info(std::move(info)), m_byteBuffer(make_global(content)) {}

struct InfoJSBundle : IJSBundle {
public:
    std::string_view Content() noexcept override;

    JSBundleInfo Info() noexcept override;

    InfoJSBundle(JSBundleInfo &&info) noexcept;

    uint64_t Timestamp() noexcept;

private:
    JSBundleInfo m_info;
};

InfoJSBundle::InfoJSBundle(
        JSBundleInfo &&info) noexcept
        : m_info(std::move(info)) {}

std::string_view InfoJSBundle::Content() noexcept {
    return std::string_view();
}

JSBundleInfo InfoJSBundle::Info() noexcept {
    return m_info;
}

std::string_view ByteBufferJSBundle::Content() noexcept {
    size_t size = m_byteBuffer->getDirectSize();
    const char *bytes = reinterpret_cast<const char *>(m_byteBuffer->getDirectBytes());
    return std::string_view(bytes, size);
}

JSBundleInfo ByteBufferJSBundle::Info() noexcept {
    return m_info;
}

/*static */std::unique_ptr<IJSBundle> JJSBundle::get(alias_ref<JJSBundle> thizz) {
    const auto cls = javaClassStatic();
    const auto iFieldContent = cls->getField<JByteBuffer>("Content");
    local_ref<JByteBuffer> content = make_local(thizz->getFieldValue(iFieldContent));
    const auto iFieldInfo = cls->getField<JJSBundleInfo>("Info");
    local_ref<JJSBundleInfo> info = make_local(thizz->getFieldValue(iFieldInfo));

    std::unique_ptr<IJSBundle> jsBundle;
    if (content) {
        jsBundle = std::make_unique<ByteBufferJSBundle>(JJSBundleInfo::get(info),
                                                        content);
    } else {
        jsBundle = std::make_unique<InfoJSBundle>(JJSBundleInfo::get(info));
    }

    return jsBundle;
}

/*static */JSBundleInfo JJSBundleInfo::get(facebook::jni::alias_ref<JJSBundleInfo> thizz) {
    JSBundleInfo info;
    const auto cls = javaClassStatic();
    const auto iFieldId = cls->getField<JString>("Id");
    info.Id = thizz->getFieldValue(iFieldId)->toStdString();
    const auto iFieldFileName = cls->getField<JString>("FileName");
    info.FileName = thizz->getFieldValue(iFieldFileName)->toStdString();
    const auto iFieldVersion = cls->getField<JLong>("Version");
    const auto fieldValue = thizz->getFieldValue(iFieldVersion);
    if (fieldValue) {
        info.Timestamp = fieldValue->value();
    } else {
        info.Timestamp = 0;
    }
    return info;
}

} // namespace facebook::react