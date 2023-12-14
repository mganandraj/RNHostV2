/*------------------------------------------------------------------------------
	Json reader
------------------------------------------------------------------------------*/
#pragma once
#include <json/IJsonReader.h>
#include <json/IJsonWriter.h>
#include "jsonInput.h"
#include <object/refCountedObject.h>

namespace Mso { namespace Json {

/*----------------------------------------------------------------------------*/
template<typename TChar>
class JsonReader: public Mso::RefCountedObject<TIReader<TChar>>
{
public:
	JsonReader(std::unique_ptr<TIInput<TChar>>&& input) : m_input(std::move(input))
	{
		m_isAllowed.all = 0;
		OnValueExpected();
	}

	ParseState::Enum ReadNext() noexcept override;
	std::optional<Mso::basic_string_view<TChar>> TryGetString() const noexcept override;
	std::optional<bool> TryGetBool() const noexcept override;
	std::optional<int32_t> TryGetInt32() const noexcept override;
	std::optional<int64_t> TryGetInt64() const noexcept override;
	std::optional<uint64_t> TryGetUInt64() const noexcept override;
	std::optional<double> TryGetDouble() const noexcept override;
	bool IsNull() const noexcept override;
	std::optional<std::pair<Mso::basic_string_view<TChar>, bool>> TryGetRawValue() const noexcept override;

private:
	void ResetGroupState() noexcept;
	void OnValueExpected() noexcept;

	ParseState::Enum HandleStartGroup(ParseState::Enum state) noexcept;
	ParseState::Enum HandleEndGroup(ParseState::Enum oldState, ParseState::Enum newState) noexcept;
	
	bool HandleBeginString() noexcept;
	bool HandleNonStringChar(TChar wch) noexcept;
	bool HandleEscapeChar(TChar wch) noexcept;
	bool HandleEscapeCharHex(TChar wch) noexcept;
	bool HandleEndGroupOrValue(ParseState::Enum state) noexcept;
	bool HandleEndName() noexcept;

	ParseState::Enum HandleInvalidData() noexcept
	{
		m_isAllowed.all = 0;
		return ParseState::ErrorInvalidData;
	}

private:
	std::unique_ptr<TIInput<TChar>> m_input;

	// Tracks the next allowed state(s)
	union
	{
		struct
		{
			// [ or {
			uint32_t fStartGroup : 1;

			// "
			uint32_t fBeginName : 1;

			// :
			uint32_t fNameDelim : 1;

			// " or value char
			uint32_t fBeginValue : 1;

			// valid name or value char
			uint32_t fNonStringChar : 1;

			// valid string character
			uint32_t fStringChar : 1;

			// valid escape char
			uint32_t fEscapeChar : 1;

			// 4 hex digits
			uint32_t fEscapeCharHex : 1;

			// ] or }
			uint32_t fEndGroup : 1;

			// ,
			uint32_t fEndComma : 1;
		};

		uint32_t all;

	} m_isAllowed;

	// Stores the current name / value text
	Mso::basic_string<TChar> m_textBuffer;

	// Stack of parse states (Array, Object, Name, Value)
	std::stack<ParseState::Enum, std::vector<ParseState::Enum>> m_states;

	// Used when decoding \uHHHH values
	size_t m_hexStartIndex {};

	// True when m_textBuffer contains a name or value string
	bool m_isStringContent = false;

	DECLARE_COPYCONSTR_AND_ASSIGNMENT(JsonReader);
};

}} // Mso::Json
