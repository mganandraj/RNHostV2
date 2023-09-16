//
// Created by anandrag on 9/15/2023.
//

#include "JJSBundle.h"

using namespace facebook::jni;
using namespace Mso::React;

struct ByteBufferJSBundle : public Mso::UnknownObject<IJSBundle>
{
public:
    std::string_view Content() noexcept override;
    JSBundleInfo Info() noexcept override;

private:
    friend MakePolicy; // Allows Mso::Make to access private constructor
    ByteBufferJSBundle(JSBundleInfo&& info, local_ref<JByteBuffer>) noexcept;
    uint64_t Timestamp() noexcept;

private:
    JSBundleInfo m_info;
    global_ref<JByteBuffer> m_byteBuffer; // Owns the java ByteBuffer

};

ByteBufferJSBundle::ByteBufferJSBundle(
        JSBundleInfo&& info,
        local_ref<JByteBuffer> content) noexcept
        : m_info(std::move(info))
        , m_byteBuffer(make_global(content))
{}

std::string_view ByteBufferJSBundle::Content() noexcept  {
    size_t size = m_byteBuffer->getDirectSize();
    const char* bytes = reinterpret_cast<const char*>(m_byteBuffer->getDirectBytes());
    return std::string_view(bytes, size);
}

JSBundleInfo ByteBufferJSBundle::Info() noexcept  {
    return m_info;
}

/*static */Mso::CntPtr<Mso::React::IJSBundle> JJSBundle::get(alias_ref<JJSBundle> thizz) {
    const auto cls = javaClassStatic();
    const auto iFieldContent = cls->getField<JByteBuffer>("Content");
    local_ref<JByteBuffer> content = make_local(thizz->getFieldValue(iFieldContent));
    const auto iFieldInfo = cls->getField<JJSBundleInfo>("Info");
    local_ref<JJSBundleInfo> info = make_local(thizz->getFieldValue(iFieldInfo));

    Mso::CntPtr<Mso::React::IJSBundle>jsBundle;
    if(content) {
        jsBundle = Mso::Make<ByteBufferJSBundle>( std::move(JJSBundleInfo::get(info)), content);
    }
    return jsBundle;
}

/*static */Mso::React::JSBundleInfo JJSBundleInfo::get(facebook::jni::alias_ref<JJSBundleInfo> thizz) {
    Mso::React::JSBundleInfo info;
    const auto cls = javaClassStatic();
    const auto iFieldId = cls->getField<JString>("Id");
    auto idValue = thizz->getFieldValue(iFieldId);
    if(idValue) {
        info.Id = idValue->toStdString();
    }

    const auto iFieldFileName = cls->getField<JString>("FileName");
    auto fileNameValue = thizz->getFieldValue(iFieldFileName);
    if(fileNameValue) {
        info.FileName = fileNameValue->toStdString();
    }

    const auto iFieldTimestamp = cls->getField<JLong>("Timestamp");
    auto timestampValue = thizz->getFieldValue(iFieldTimestamp);
    if(timestampValue) {
        info.Timestamp = timestampValue->value();
    }

    return info;
}

/*static */facebook::jni::local_ref<JJSBundleInfo> JJSBundleInfo::create(Mso::React::JSBundleInfo info) {
    return newInstance(make_jstring(info.Id), make_jstring(info.FileName), JLong::valueOf(info.Timestamp));
}

/*static */facebook::jni::local_ref<JJSBundle> JJSBundle::create(Mso::React::IJSBundle& bundle) {
    local_ref<facebook::jni::JByteBuffer> content;
    if(!bundle.Content().empty()) {
        content = facebook::jni::JByteBuffer::wrapBytes(const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(bundle.Content().data())), bundle.Content().size());
    }

    return newInstance(content, JJSBundleInfo::create(bundle.Info()));
}