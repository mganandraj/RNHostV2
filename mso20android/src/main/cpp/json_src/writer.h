#pragma once
#include <json/IJsonReader.h>
#include <json/IJsonWriter.h>
#include "stringrefoutput.h"
#include <stack>

namespace Mso::Json {

template<typename TChar>
class TWriter final : public TIWriter<TChar>
{
	static constexpr size_t c_bufferSize = 4096;

public:
	DECLARE_COPYCONSTR_AND_ASSIGNMENT(TWriter);
	TWriter(TIOutput<TChar>& output, PrettyPrint prettyPrint) noexcept :
		m_output(&output), m_prettyPrint(prettyPrint)
	{
		m_outputBuffer.reserve(c_bufferSize);
	}

	~TWriter() = default;

	void StartObject() noexcept override;
	void EndObject() noexcept override;
	void StartArray() noexcept override;
	void EndArray() noexcept override;
	void WriteName(const Mso::basic_string_view<TChar>& name) noexcept override;
	void WriteValue(const Mso::basic_string_view<TChar>& value) noexcept override;
	void WriteValue(const Mso::basic_string_view<TChar>& value, const rawValue_t&) noexcept override;
	void WriteValue(bool value) noexcept override;
	void WriteValue(int64_t value) noexcept override;
	void WriteValue(uint64_t value) noexcept override;
	void WriteValue(double value) noexcept override;
	void WriteValue(float value) noexcept override;
	void WriteValue(std::nullptr_t) noexcept override;

	inline void WriteValue(_In_z_ const TChar* value) noexcept
	{
		WriteValue(Mso::basic_string_view<TChar>(value));
	}

	bool IsValid() const noexcept override
	{
		return !IsInvalid();
	}

	bool IsInvalid() const noexcept
	{
		return m_optState && *m_optState == ParseState::ErrorInvalidData;
	}

	bool IsDone() const noexcept
	{
		return m_optState && *m_optState == ParseState::ErrorEndOfData;
	}

	bool IsValueAllowed() const noexcept;
	bool IsNameOrEndObjectAllowed() const noexcept;
	bool IsEndArrayAllowed() const noexcept;

private:
	void AddCommaIfAppending() noexcept;
	void WriteEscapedString(const Mso::basic_string_view<TChar>& value) noexcept;
	void WriteRawChars(const Mso::basic_string_view<TChar>& value) noexcept;

	void OnStateChange(ParseState::Enum newState) noexcept;
	void WriteBuffer() noexcept;

	// Stores intermediate output to reduce virtual calls to m_output
	Mso::basic_string<TChar> m_outputBuffer;

	// Destination for json output
	Mso::CntPtr<TIOutput<TChar>> m_output;

	// Track the current stack of array/object
	std::stack<ParseState::Enum> m_stack;

	// Initially empty and then tracks the last thing written
	// Set to ErrorInvalidData if this writer becomes invalid
	// Set to ErrorEndOfData when the writer is done.
	std::optional<ParseState::Enum> m_optState;

	const PrettyPrint m_prettyPrint;
};

} // namespace Mso::Json
