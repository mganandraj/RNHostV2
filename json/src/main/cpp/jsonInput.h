#pragma once
#include <json/ijsonreader.h>

namespace Mso::Json {

template<typename TChar>
struct TIInputString final : TIInput<TChar>
{
	TIInputString(const Mso::basic_string_view<TChar>& json) noexcept
	{
		TIInput<TChar>::m_json = json;
	}
};

// Reka uses the string version
//
//template<typename TChar>
//struct TIInputStream final : TIInput<TChar>
//{
//public:
//	static std::unique_ptr<TIInput<TChar>> Make(ISequentialStream& stream) noexcept
//	{
//		return std::make_unique<TIInputStream<TChar>>(stream);
//	}
//
//	TIInputStream(ISequentialStream& stream) noexcept : m_stream(&stream)
//	{
//		ReadBuffer();
//	}
//
//	virtual void FillBuffer() noexcept override
//	{
//		if (TIInput<TChar>::m_json.size())
//		{
//			return;
//		}
//
//		ReadBuffer();
//	}
//
//private:
//	_Memberinitializer_ void ReadBuffer() noexcept
//	{
//		ULONG bytesRead = 0;
//		if (FAILED(m_stream->Read(m_buffer, sizeof(m_buffer), &bytesRead)))
//		{
//			return;
//		}
//
//		if (bytesRead % sizeof(TChar))
//		{
//			return;
//		}
//
//		TIInput<TChar>::m_json = Mso::basic_string_view<TChar>(m_buffer, static_cast<size_t>(bytesRead / sizeof(TChar)));
//	}
//
//	TChar m_buffer[4096 / sizeof(TChar)];
//	Mso::TCntPtr<ISequentialStream> m_stream;
//};

} // namespace
