#pragma once
#include <json/ijsonreader.h>
#include <json/ijsonStorage.h>
#include <json/ijsonwriter.h>
#include <optional>
#include <variant>

namespace Mso::Json {

template<typename TChar>
struct ParseEvent
{
	ParseEvent(ParseState::Enum stateIn) noexcept :
		state(stateIn)
	{
	}

	ParseEvent(ParseState::Enum stateIn, Mso::basic_string_view<TChar> valueIn, bool isQuotedIn) :
		state(stateIn), value(valueIn), isQuoted(isQuotedIn)
	{
	}

	ParseState::Enum state {};
	Mso::basic_string<TChar> value;
	bool isQuoted {};
};

template<typename TChar>
class Storage : public TIStorage<TChar>
{
public:
	Storage(std::vector<ParseEvent<TChar>>&& events) noexcept : m_events(std::move(events))
	{
	}

	virtual ~Storage() = default;
	virtual bool TryWrite(IWriter<TChar>& writer) const noexcept override;

private:
	const std::vector<ParseEvent<TChar>> m_events;
};

} // namespace Mso::Json
