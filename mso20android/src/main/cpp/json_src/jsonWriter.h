#pragma once
#include <json/IJsonReader.h>
#include <json/IJsonWriter.h>

#include <stack>

#define DECLARE_COPYCONSTR_AND_ASSIGNMENT( C )  explicit C( const C& ); void operator=( const C& )

#include <object/refCountedObject.h>

namespace Mso::Json {

/**
	\class JsonWriter
*/
class JsonWriter final : public Mso::RefCountedObject<IJsonWriter>
{
public:
	JsonWriter(bool shouldPrettyPrint) noexcept : m_shouldPrettyPrint(shouldPrettyPrint)
	{
	}

	bool StartObject() noexcept override;
	bool EndObject() noexcept override;
	bool StartArray() noexcept override;
	bool EndArray() noexcept override;
	bool WriteName(_In_z_ const wchar_t* wzName) noexcept override;
	bool WriteString(_In_z_ const wchar_t* wzValue) noexcept override;
	bool WriteBool(bool fValue) noexcept override;
	bool WriteInt(int64_t iValue) noexcept override;
	bool WriteUInt64(uint64_t ui64Value) noexcept override;
	bool WriteDouble(double dValue) noexcept override;
	bool WriteFloat(float fpValue) noexcept override;
	bool WriteNull() noexcept override;
	bool WriteRawChars(_In_z_ const wchar_t* wzRawChars) noexcept override;
	bool GetResultData(_Out_ std::wstring& value) const noexcept override;
	std::optional<std::wstring> TryExtractOutput() noexcept override;
	bool IsInvalid() const noexcept override { return m_isInvalid; }

private:
	virtual ~JsonWriter() = default;

	bool StartObjectAllowed() const noexcept;
	bool EndObjectAllowed() const noexcept;
	bool StartArrayAllowed() const noexcept;
	bool EndArrayAllowed() const noexcept;
	bool NameAllowed() const noexcept;
	bool ValueAllowed() const noexcept;
	bool NullAllowed() const noexcept;
	void WriteEscapedString(_In_z_ LPCWSTR wz);
	void WriteEscapedControlChar(wchar_t wch);

	std::wstring resultString;
	const bool m_shouldPrettyPrint;
	// Stack of array/object
	std::stack<ParseState::Enum> stack;
	ParseState::Enum lastWritten {ParseState::ErrorEndOfData};
	bool fNameIsSet {};
	bool fValueIsSet {};
	bool m_isInvalid {};
	DECLARE_COPYCONSTR_AND_ASSIGNMENT(JsonWriter);
};

} // namespace Mso::Json
