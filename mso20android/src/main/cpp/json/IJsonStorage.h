/**
	API to store the current JSON fragment
	Useful to round-trip unknown JSON content
*/
#pragma once
#include <json/ijsonreader.h>
#include <json/ijsonwriter.h>

namespace Mso::Json {

template<typename TChar>
struct TIStorage
{
	virtual ~TIStorage() = default;

	// Write the stored events back to the writer
	// Only returns false if IWriter returns false
	virtual bool TryWrite(IWriter<TChar>& writer) const noexcept = 0;
};

} // namespace Mso::Json

namespace Mso::Json::Utf8 {

using IStorage = TIStorage<char>;

// Returns a new storage object which contains the current JSON fragment
// Returns false if the reader fails to parse
LIBLET_PUBLICAPI std::unique_ptr<IStorage> TryStoreFragment(IReader& reader) noexcept;

} // namespace Mso::Json::Utf8

namespace Mso::Json::Unicode {

using IStorage = TIStorage<wchar_t>;

// Returns a new storage object which contains the current JSON fragment
// Returns false if the reader fails to parse
LIBLET_PUBLICAPI std::unique_ptr<IStorage> TryStoreFragment(IReader& reader) noexcept;

} // namespace Mso::Json::Unicode
