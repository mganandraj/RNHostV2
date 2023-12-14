#include "precomp.h"
#include "jsonStorage.h"

namespace Mso::Json {

/**
	Returns the events that make up the current json fragment
*/
template<typename TChar>
std::optional<std::vector<ParseEvent<TChar>>> TryGetEvents(TIReader<TChar>& reader) noexcept
{
	std::vector<ParseEvent<TChar>> events;
	uint32_t depth = 0;
	while (1)
	{
		auto result = reader.ReadNext();
		switch (result)
		{
			case ParseState::StartObject:
			case ParseState::StartArray:
				events.emplace_back(result);
				depth++;
				break;

			case ParseState::EndObject:
			case ParseState::EndArray:
				events.emplace_back(result);
				depth--;
				break;

			case ParseState::Name:
			case ParseState::Value:
			{
				auto optValue = reader.TryGetRawValue();
				if (!optValue)
					return {};

				events.emplace_back(result, optValue->first, optValue->second);
				break;
			}

			case ParseState::ErrorInvalidData:
			case ParseState::ErrorEndOfData:
				return {};
		}

		if (depth == 0)
			break;
	}

	return events;
}

/**
	Replay the stored events to the writer
*/
template<typename TChar>
bool Storage<TChar>::TryWrite(IWriter<TChar>& writer) const noexcept
{
	for (const auto& event : m_events)
	{
		switch (event.state)
		{
			case ParseState::StartObject:
				writer.StartObject();
				break;

			case ParseState::StartArray:
				writer.StartArray();
				break;

			case ParseState::EndObject:
				writer.EndObject();
				break;

			case ParseState::EndArray:
				writer.EndArray();
				break;

			case ParseState::Name:
				writer.WriteName(event.value);
				break;

			case ParseState::Value:
				if (event.isQuoted)
					writer.WriteValue(event.value);
				else
					writer.WriteValue(event.value, rawValue);
				break;

			case ParseState::ErrorInvalidData:
			case ParseState::ErrorEndOfData:
				VerifyElseCrashSzTag(false, "Storage object should never have invalid data", 0x1e50d39e /* tag_4uno4 */);
		}
	}
	
	return writer.IsValid();
}

template<typename TChar>
std::unique_ptr<TIStorage<TChar>> TryStoreFragment(TIReader<TChar>& reader) noexcept
{
	auto optEvents = TryGetEvents(reader);
	if (optEvents)
		return std::make_unique<Storage<TChar>>(std::move(*optEvents));
	return {};
}


} // namespace Mso::Json

namespace Mso::Json::Utf8 {

std::unique_ptr<IStorage> TryStoreFragment(IReader& reader) noexcept
{
	return Mso::Json::TryStoreFragment<char>(reader);
}

} // namespace Mso::Json::Utf8

namespace Mso::Json::Unicode {

std::unique_ptr<IStorage> TryStoreFragment(IReader& reader) noexcept
{
	return Mso::Json::TryStoreFragment<wchar_t>(reader);
}

} // namespace Mso::Json::Unicode

