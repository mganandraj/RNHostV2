/**
	APIs to write Json data (see http://Json.org)

	IWriter interface for writing JSON
	- VerifyElseCrash is used to prevent writing invalid JSON

	IOutput interface to receive chunks of JSON output
	- A failure returned from IOutput::TryWrite will invalidate the writer
		which makes all subsequent calls into a no-op.

	MakeWriter to construct the JSON writer
	- Since the caller owns the output, the caller must check IsValid()
		to verify that the JSON was written correctly. This can be done at any
		time but given the rarity of OOM, once at the end is ideal.

	Example usage:

	std::wstring strJson;
	auto writer = Mso::Json::Unicode::MakeWriter(strJson, {});
	...
	if (writer->IsValid())
	{
		Use strJson...
	}

	See *Output.h files for more output implementations.
*/
#pragma once

#include <compileradapters/functiondecorations.h>
#include <memory>
#include <object/refcounted.h>
#include <optional>
#include <stlextensions/string_adapter.h>
#include <string>

namespace Mso::Json {

// Tag to write raw characters as a value to the json output
constexpr struct rawValue_t {} rawValue;

template<typename TChar>
struct TIWriter
{
	virtual ~TIWriter() = default;

	virtual void StartObject() noexcept = 0;
	virtual void EndObject() noexcept = 0;
	virtual void StartArray() noexcept = 0;
	virtual void EndArray() noexcept = 0;
	virtual void WriteName(const Mso::basic_string_view<TChar>& name) noexcept = 0;
	virtual void WriteValue(const Mso::basic_string_view<TChar>& value) noexcept = 0;
	virtual void WriteValue(const Mso::basic_string_view<TChar>& value, const rawValue_t&) noexcept = 0;
	virtual void WriteValue(bool value) noexcept = 0;
	virtual void WriteValue(int64_t value) noexcept = 0;
	virtual void WriteValue(uint64_t value) noexcept = 0;
	virtual void WriteValue(double value) noexcept = 0;
	virtual void WriteValue(float value) noexcept = 0;
	virtual void WriteValue(std::nullptr_t) noexcept = 0;

	// Returns false if runtime error occured in above methods
	virtual bool IsValid() const noexcept = 0;

	// This overload is needed to prevent const TChar* from casting to bool
	inline void WriteValue(_In_z_ const TChar* value) noexcept
	{
		WriteValue(Mso::basic_string_view<TChar>(value));
	}
};

/**
	Output abstraction for writing JSON.
*/
template<typename TChar>
struct TIOutput : Mso::IRefCounted
{
	// Called with chunks of json output by TIWriter
	virtual bool TryWrite(const Mso::basic_string_view<TChar>& view) noexcept = 0;

protected:
	TIOutput() noexcept = default;
	virtual ~TIOutput() = default;
};

/**
	Simple RAII class to start/end a JSON object.
*/
template<typename T>
class ObjectScope;
template <typename T>
ObjectScope(const T&) -> ObjectScope<T>;

template<typename TChar>
class ObjectScope<TIWriter<TChar>>
{
public:
	DECLARE_COPYCONSTR_AND_ASSIGNMENT(ObjectScope);
	explicit ObjectScope(TIWriter<TChar>& writer) noexcept : m_writer(writer)
	{
		m_writer.StartObject();
	}

	~ObjectScope() noexcept
	{
		m_writer.EndObject();
	}

private:
	TIWriter<TChar>& m_writer;
};

/**
	Simple RAII class to start/end a JSON array.
*/
template<typename T>
class ArrayScope;
template <typename T>
ArrayScope(const T&) -> ArrayScope<T>;

template<typename TChar>
class ArrayScope<TIWriter<TChar>>
{
public:
	DECLARE_COPYCONSTR_AND_ASSIGNMENT(ArrayScope);
	explicit ArrayScope(TIWriter<TChar>& writer) noexcept : m_writer(writer)
	{
		m_writer.StartArray();
	}

	~ArrayScope() noexcept
	{
		m_writer.EndArray();
	}

private:
	TIWriter<TChar>& m_writer;
};

enum class PrettyPrint
{
	None,

	// Add spaces after comma/colon
	Spaces,
};

} // namespace Mso::Json

/**
	Factory APIs for writing ascii/utf8 JSON
*/
namespace Mso::Json::Utf8 {

using IOutput = TIOutput<char>;
using IWriter = TIWriter<char>;

// Returns an object to write json to an arbitrary TIOutput object
LIBLET_PUBLICAPI std::unique_ptr<IWriter> MakeWriter(IOutput& output, PrettyPrint prettyPrint) noexcept;

// Returns an object to write json to an existing wstring
LIBLET_PUBLICAPI std::unique_ptr<IWriter> MakeWriter(std::string& output, PrettyPrint prettyPrint) noexcept;

} // namespace Mso::Json::Utf8

/**
	Factory APIs for writing Unicode JSON
*/
namespace Mso::Json::Unicode {

using IOutput = TIOutput<wchar_t>;
using IWriter = TIWriter<wchar_t>;

// Returns an object to write json to an arbitrary TIOutput object
LIBLET_PUBLICAPI std::unique_ptr<IWriter> MakeWriter(IOutput& output, PrettyPrint prettyPrint) noexcept;

// Returns an object to write json to an existing wstring
LIBLET_PUBLICAPI std::unique_ptr<IWriter> MakeWriter(std::wstring& output, PrettyPrint prettyPrint) noexcept;

} // namespace Mso::Json::Unicode

namespace Mso::Json::Flighting {

LIBLET_PUBLICAPI void EnableNewWriter(bool value) noexcept;

} // namespace Mso::Json::Flighting






/**
	The rest of this file contains deprecated APIs - please use APIs above in new code
*/
namespace Mso::Json {

/*!
	\struct IJsonWriter

	Operations only fail in case of OOM or attempting to write invalid JSON
	Deprecated in favor of IWriter above
*/
struct IJsonWriter : Mso::IRefCounted
{
	//! Write StartObject token
	virtual bool StartObject() noexcept = 0;

	//! Write EndObject token
	virtual bool EndObject() noexcept = 0;

	//! Write StartArray token
	virtual bool StartArray() noexcept = 0;

	//! Write EndArray token
	virtual bool EndArray() noexcept = 0;

	//! Write Name
	virtual bool WriteName(_In_z_ const wchar_t* wzName) noexcept = 0;

	//! Write String
	virtual bool WriteString(_In_z_ const wchar_t* wzName) noexcept = 0;

	//! Write Bool
	virtual bool WriteBool(bool fValue) noexcept = 0;

	//! Write Int
	virtual bool WriteInt(int64_t iValue) noexcept = 0;

	//! Write UInt64
	virtual bool WriteUInt64(uint64_t ui64Value) noexcept = 0;

	//! Write Double
	virtual bool WriteDouble(double dValue) noexcept = 0;

	//! Write Float
	virtual bool WriteFloat(float fpValue) noexcept = 0;

	//! Write null
	virtual bool WriteNull() noexcept = 0;

	//! Write Raw Characters
	virtual bool WriteRawChars(_In_z_ const wchar_t* /*wzRawChars*/) noexcept
	{
		return false;
	}

	// (deprecated) Returns the result data.
	virtual bool GetResultData(_Out_ std::wstring& value) const noexcept = 0;

	// Moves the stored json output to the return value.
	// Fails if the writer is invalid or a complete fragment has not been written
	virtual std::optional<std::wstring> TryExtractOutput() noexcept = 0;

	/**
		This returns true if OOM occured and left the object in an invalid state
	*/
	virtual bool IsInvalid() const noexcept = 0;
};

LIBLET_PUBLICAPI Mso::CntPtr<IJsonWriter> CreateJsonWriter() noexcept;
LIBLET_PUBLICAPI Mso::CntPtr<IJsonWriter> CreatePrettyPrintingJsonWriter() noexcept;

template<>
class ObjectScope<IJsonWriter>
{
public:
	DECLARE_COPYCONSTR_AND_ASSIGNMENT(ObjectScope);
	explicit ObjectScope(IJsonWriter& writer) noexcept : m_writer(writer)
	{
		VerifyElseCrashSzTag(m_writer.StartObject() || m_writer.IsInvalid(), "StartObject is not allowed", 0x1e56105d /* tag_4v7b3 */);
	}

	~ObjectScope() noexcept
	{
		VerifyElseCrashTag(m_writer.EndObject() || m_writer.IsInvalid(), 0x1e56105c /* tag_4v7b2 */);
	}

private:
	IJsonWriter& m_writer;
};

template<>
class ArrayScope<IJsonWriter>
{
public:
	DECLARE_COPYCONSTR_AND_ASSIGNMENT(ArrayScope);
	explicit ArrayScope(IJsonWriter& writer) noexcept : m_writer(writer)
	{
		VerifyElseCrashSzTag(m_writer.StartArray() || m_writer.IsInvalid(), "StartArray is not allowed", 0x1e56105b /* tag_4v7b1 */);
	}

	~ArrayScope() noexcept
	{
		VerifyElseCrashTag(m_writer.EndArray() || m_writer.IsInvalid(), 0x1e56105a /* tag_4v7b0 */);
	}

private:
	IJsonWriter& m_writer;
};

} // namespace Mso::Json

/**
	The rest of this file contains deprecated APIs - please use APIs above in new code
*/
namespace Mso::Json {

template<typename TChar>
using IWriter = TIWriter<TChar>;

template<typename TChar>
using IOutput = TIOutput<TChar>;

} // namespace Mso::Json

/**
	The rest of this file contains deprecated APIs - please use APIs above in new code
*/
namespace Mso::Json::Ascii {

using Writer = IWriter<char>;

// Returns an object to write json to an arbitrary TIOutput object
inline std::unique_ptr<Writer> MakeWriter(IOutput<char>& output, PrettyPrint prettyPrint) noexcept
{
	return Mso::Json::Utf8::MakeWriter(output, prettyPrint);
}

// Returns an object to write json to an existing wstring
inline std::unique_ptr<Writer> MakeWriter(std::string& output, PrettyPrint prettyPrint) noexcept
{
	return Mso::Json::Utf8::MakeWriter(output, prettyPrint);
}

} // namespace Mso::Json::Ascii

/**
	The rest of this file contains deprecated APIs - please use APIs above in new code
*/
namespace Mso::Json::Unicode {

using Writer = TIWriter<wchar_t>;

} // namespace Mso::Json::Unicode
