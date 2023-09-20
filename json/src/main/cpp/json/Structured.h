/**
	Declarative parsers for working with fixed structure Json.

	Allows specifying the expected structure of Json data, and
	having a parser for the Json automatically constructed.

	For example, parsing a Json array of people.
	First declare a "Name" attribute we expect on a person object and its type (string):

		struct NameAttr : Structured::Attribute<Structured::String> { static constexpr WCHAR* name = L"Name"; };
		
	A single person object:

		using Person = Structured::Object<NameAttr>;

	An array of person objects:

		using People = Structured::Array<Person>;

	Now we can use our parsers. The behavior of `Object` and `Array` is to
	skip unmatched elements and values:

		auto reader = createReader(LR"[
			{"Name": "Jack", "Age": 4},
			null,
			{"Occupation": "child", "Age": 5, "Name": "Jill"},
			false]");

		auto result = Structured::Parse<People>(reader);

	And extract the results:
		
		if (result.IsSuccess())
		{
			auto people = result.value; // in parsed order
			people.size() == 2;

			for (auto person : people)
			{
				// Extract name attribute 
				auto name = std::get<NameAttr>(person).value;
				std::cout << name << std::endl;
			}
		}

	Prints:

		Jack
		Jill
*/
#pragma once

#include <collections/Either.h>
#include <json/IJsonReader.h>
#include <functional/Predicate.h>
#include <bitset>

namespace Mso { namespace Json { namespace Structured {

/**
	Parsing error state.
*/
enum class ParseError
{
	Ok,
	Error
};

/**
	Result of a running a parser.
*/
template <typename T>
struct ParseResult
{
	/**
		Create a result from a state.
	*/
	static ParseResult Make(ParseState::Enum resultState, T&& resultValue)
	{
		if (IsParseError(resultState))
			return MakeError(resultState);
		else
			return ParseResult(ParseError::Ok, resultState, std::move(resultValue));
	}

	/**
		Create an error result with an empty value.
	*/
	static ParseResult MakeError(Mso::Json::ParseState::Enum state)
	{
		return ParseResult(ParseError::Error, state, T());
	}

	ParseResult(ParseError resultError, ParseState::Enum resultState, const T& resultValue) :
		error(resultError),
		state(resultState),
		value(resultValue)
	{ }

	ParseResult(ParseError resultError, ParseState::Enum resultState, T&& resultValue) :
		error(resultError),
		state(resultState),
		value(std::move(resultValue))
	{ }

	ParseResult(ParseResult&& other) :
		error(other.error),
		state(other.state),
		value(std::move(other.value))
	{
		other.error = ParseError::Error;
		other.state = ParseState::ErrorEndOfData;
	}

	ParseResult& operator=(ParseResult&& other)
	{
		this->error = other.error;
		other.error = ParseError::Error;
		this->state = other.state;
		other.state = ParseState::ErrorEndOfData;
		this->value = std::move(other.value);
	}

	ParseError error;
	ParseState::Enum state;
	T value;

	template <typename R>
	ParseResult<R> UpdateValue(R&& newValue) const
	{
		return ParseResult<R>(error, state, std::forward<R>(newValue));
	}

	bool IsSuccess() const noexcept { return (error == ParseError::Ok); }
	
	operator bool() const noexcept { return IsSuccess(); }
};

namespace Details
{

/**
	Nothing type.

	Used to indicate when a parser succeeds but produces no value.
*/
struct nothing_t { };

__declspec(selectany) extern const nothing_t nothing { };

/**
	Collection of Json parsing states.

	Used to store the first accepting state for a parser.
*/
template <ParseState::Enum... tokens>
struct TokenSet { };

/**
	Joins two TokenSets together.
*/
template <typename A, typename B>
struct JoinTokenSets;

template <ParseState::Enum... aTokens, ParseState::Enum... bTokens>
struct JoinTokenSets<TokenSet<aTokens...>, TokenSet<bTokens...>>
{
	using type = TokenSet<aTokens..., bTokens...>;
};

/**
	Check if a state is a member of a token set.
*/
template <ParseState::Enum first, ParseState::Enum... rest>
bool AcceptsToken(TokenSet<first, rest...>, ParseState::Enum s)
{
	return (s == first || AcceptsToken(TokenSet<rest...> { }, s));
}

inline bool AcceptsToken(TokenSet<>, ParseState::Enum /*s*/)
{
	return false;
}

/**
	Is a given parser valid for some initial state?
*/
template <typename Parser>
bool Accepts(ParseState::Enum s)
{
	return AcceptsToken(typename Parser::first_token(), s);
}

/**
	Extract a value from the parsing input.

	@tparam T Type extracted.
	@tparam Getter Method on `IJsonReader` that gets the value from the input.
*/
template <
	typename T,
	bool(IJsonReader::*Getter)(T*) const noexcept>
struct SimpleExtractor
{
	using result_type = T;

	static bool ExtractValue(
		Mso::Json::IJsonReader& input,
		_Out_ T& value) noexcept
	{
		value = T();
		return (input.*Getter)(&value);
	}
};

using BoolExtractor = SimpleExtractor<bool, &IJsonReader::GetBool>;
using IntExtractor = SimpleExtractor<int, &IJsonReader::GetInt>;
using Uint64Extractor = SimpleExtractor<uint64_t, &IJsonReader::GetUInt64>;
using DoubleExtractor = SimpleExtractor<double, &IJsonReader::GetDouble>;

/**
	Extracts a null value from the state.
*/
struct NullExtractor
{
	using result_type = nothing_t;

	static bool ExtractValue(
		const Mso::Json::IJsonReader& input,
		_Out_ nothing_t& value)
	{
		value = { };
		return input.IsNull();
	}
};

/**
	Extracts a string value from the state.
*/
struct StringExtractor
{
	using result_type = std::wstring;

	static bool ExtractValue(
		Mso::Json::IJsonReader& input,
		_Out_ std::wstring& value)
	{
		const WCHAR* rgwch = nullptr;
		size_t cch = 0;
		if (!input.GetString(&rgwch, &cch) || !rgwch)
		{
			value = std::wstring();
			return false;
		}
		else
		{
			value = std::wstring(rgwch, cch);
			return true;
		}
	}
};

template <typename T>
std::wstring ToJsonString(const T& x) { return std::to_wstring(x); }

inline std::wstring ToJsonString(const std::wstring& val) { return val; }

inline std::wstring ToJsonString(nothing_t) { return std::wstring(L""); }

inline std::wstring ToJsonString(bool val) { return std::wstring(val ? L"true" : L"false"); }


/**
	Attempts to extracts a value as a string.
*/
struct ExtractAsString
{
	using result_type = std::wstring;

	template <typename Extractor>
	static bool TryExtractAsString(
		Mso::Json::IJsonReader& input,
		_Out_ std::wstring& value)
	{
		typename Extractor::result_type result;
		if (Extractor::ExtractValue(input, result))
		{
			value = ToJsonString(result);
			return true;
		}
		return false;
	}

	using pFunc_t = bool (*)(Mso::Json::IJsonReader&, std::wstring&);

	static bool ExtractValue(
		Mso::Json::IJsonReader& input,
		_Out_ std::wstring& value)
	{
		return Mso::Functional::anyOf(
			static_cast<pFunc_t>(&TryExtractAsString<StringExtractor>),
			static_cast<pFunc_t>(&TryExtractAsString<NullExtractor>),
			static_cast<pFunc_t>(&TryExtractAsString<BoolExtractor>),
			static_cast<pFunc_t>(&TryExtractAsString<IntExtractor>),
			static_cast<pFunc_t>(&TryExtractAsString<DoubleExtractor>))(input, value);
	}
};

/**
	Matches an expected state.
*/
template <ParseState::Enum expected>
struct Token
{
	using result_type = nothing_t;
	using first_token = TokenSet<expected>;

	static ParseResult<result_type> Parse(ParseState::Enum s, Mso::Json::IJsonReader& /*input*/)
	{
		if (AcceptsToken(first_token(), s))
			return { ParseError::Ok, s, nothing };
		else
			return ParseResult<result_type>::MakeError(s);
	}
};

/**
	Run `Parser` and advance the input by one if it succeeded.
*/
template <typename Parser>
struct Consume
{
	using result_type = typename Parser::result_type;
	using first_token = typename Parser::first_token;
	
	static ParseResult<result_type> Parse(ParseState::Enum s, Mso::Json::IJsonReader& input)
	{
		auto result = Parser::Parse(s, input);
		if (result)
		{
			return {
				ParseError::Ok, // Always return Ok here even if ReadNext fails to correctly handle end of input.
				input.ReadNext(),
				std::move(result.value) };
		}

		return result;
	}
};

/**
	Run `First` then `Second`, combining the two results with functor `Combiner`.
*/
template <
	typename First,
	typename Second,
	typename Combiner>
struct Binary
{
	using result_type =
		typename std::remove_reference<
			typename std::invoke_result_t<Combiner,
				typename First::result_type&&,
				typename Second::result_type&&>>::type;
	
	using first_token = typename First::first_token;

	static ParseResult<result_type> Parse(ParseState::Enum s, Mso::Json::IJsonReader& input)
	{
		auto firstResult = First::Parse(s, input);
		s = firstResult.state;
		if (firstResult)
		{
			auto secondResult = Second::Parse(s, input);
			s = secondResult.state;
			if (secondResult)
				return secondResult.UpdateValue(Combiner()(std::move(firstResult.value), std::move(secondResult.value)));
		}
	
		return ParseResult<result_type>::MakeError(s);
	}
};

/**
	Run `First` or if it fails, run `Second`.

	Note that no backtracking can be performed if `First` fails.
*/
template <
	typename First,
	typename Second>
struct Either
{
	using result_type = Collections::Either<
		typename First::result_type,
		typename Second::result_type>;
	
	using first_token = typename JoinTokenSets<
		typename First::first_token,
		typename Second::first_token>::type;

	static ParseResult<result_type> Parse(ParseState::Enum s, Mso::Json::IJsonReader& input)
	{
		if (Accepts<First>(s))
		{
			auto firstResult = First::Parse(s, input);
			s = firstResult.state;
			if (firstResult)
				return firstResult.UpdateValue(result_type(Collections::either_left, std::move(firstResult.value)));
		}

		if (Accepts<Second>(s))
		{
			auto secondResult = Second::Parse(s, input);
			s = secondResult.state;
			if (secondResult)
				return secondResult.UpdateValue(result_type(Collections::either_right, std::move(secondResult.value)));
		}
		return ParseResult<result_type>::MakeError(s);
	}
};

/**
	Parses `open`, then `Content`, then `Close`, returning result from `Content`.
*/
template <
	typename Open,
	typename Close,
	typename Content>
struct Between
{
	using result_type = typename Content::result_type;
	using first_token = typename Open::first_token;

	static ParseResult<result_type> Parse(ParseState::Enum s, Mso::Json::IJsonReader& input)
	{
		auto openResult = Open::Parse(s, input);
		s = openResult.state;
		if (openResult)
		{
			auto contentResult = Content::Parse(s, input);
			s = contentResult.state;
			if (contentResult)
			{
				auto closeResult = Close::Parse(s, input);
				s = closeResult.state;
				if (closeResult)
				{
					contentResult.state = s;
					return contentResult;
				}
			}
		}

		return ParseResult<result_type>::MakeError(s);
	}
};

/**
	Run `ElementParser` many times until it fails, building results into container.

	TODO: generalize
*/
template <
	typename T,
	typename ElementParser,
	typename Adder>
struct Accumulate
{	
	using result_type = T;
	using first_token = typename ElementParser::first_token;

	static ParseResult<result_type> Parse(ParseState::Enum s, Mso::Json::IJsonReader& input)
	{
		result_type results;
		while (Accepts<ElementParser>(s))
		{
			auto result = ElementParser::Parse(s, input);
			s = result.state;
			if (!result)
				break;

			Adder::Add(results, std::move(result.value));
		}
		
		return ParseResult<result_type>::Make(s, std::move(results));
	}
};

struct VectorAdder
{
	template <typename T>
	static void Add(std::vector<T>& results, T&& value)
	{
		results.push_back(std::forward<T>(value));
	}

	template <typename T>
	static void Add(std::vector<T>&, nothing_t&&) { /*noop*/ }

	template <typename T, typename L, typename R>
	static void Add(std::vector<T>& results, Collections::Either<L, R>&& value)
	{
		if (value.IsLeft())
			Add(results, std::move(value.Left()));
		else if (value.IsRight())
			Add(results, std::move(value.Right()));
	}
};

struct MapAdder
{
	template <typename T>
	static void Add(std::unordered_map<std::wstring, T>& results, std::tuple<std::wstring, T>&& value)
	{
		results.emplace(
			std::move(std::get<0>(value)),
			std::move(std::get<1>(value)));
	}

	template <typename T>
	static void Add(std::unordered_map<std::wstring, T>&, std::tuple<std::wstring, nothing_t>&&) { /*noop*/ }

	template <typename T, typename L, typename R>
	static void Add(
		std::unordered_map<std::wstring, T>& results,
		std::tuple<std::wstring, Collections::Either<L, R>>&& value)
	{
		auto name = std::get<0>(std::move(value));
		auto store = std::get<1>(std::move(value));
		if (store.IsLeft())
			Add(results, std::make_tuple(name, store.Left()));
		else if (store.IsRight())
			Add(results, std::make_tuple(name, store.Right()));
	}
};

/**
	Applies a sequence of parsers in order, building a tuple of results.
*/
template <typename... Elements>
struct Sequence;

template <>
struct Sequence<>
{
	using result_type = std::tuple<>;
	using first_token = TokenSet<ParseState::ErrorEndOfData>; // todo: accepts empty flag?

	static ParseResult<result_type> Parse(ParseState::Enum s, Mso::Json::IJsonReader& /*input*/)
	{
		return { ParseError::Ok, s, std::tuple<>() };
	}
};

struct TupleCons
{
	template <typename X, typename...  XS>
	std::tuple<X, XS...> operator()(X&& f, std::tuple<XS...>&& s) const
	{
		return std::tuple_cat(std::make_tuple(std::forward<X>(f)), std::forward<decltype(s)>(s));
	}
};

template <typename X, typename... XS>
struct Sequence<X, XS...>
{
	using Super = Binary<X, Sequence<XS...>, TupleCons>;
	using result_type = typename Super::result_type;
	using first_token = typename Super::first_token;

	static ParseResult<result_type> Parse(ParseState::Enum s, Mso::Json::IJsonReader& input)
	{
		return Super::Parse(s, input);
	}
};

/**
	Base parser for a value.

	@tparam T Type of value produced.
	@tparam ExpectedState State we expect so we can consume a value.
	@tparam ValueExtractor Attribute that extracts a value from the input.
*/
template <
	typename T,
	ParseState::Enum ExpectedState,
	typename ValueExtractor>
struct ValueCore
{
	using result_type = T;
	using first_token = TokenSet<ExpectedState>;
	
	static ParseResult<result_type> Parse(ParseState::Enum s, Mso::Json::IJsonReader& input)
	{
		T value;
		if (ValueExtractor::ExtractValue(input, value))
			return { ParseError::Ok, s, value };
		else
			return ParseResult<result_type>::MakeError(s);
	}
};

template <
	typename T,
	ParseState::Enum ExpectedState,
	typename ValueExtractor>
using Value = Consume<ValueCore<T, ExpectedState, ValueExtractor>>;

/**
	Parser that fully consumes, but does not store, any value type.
*/
struct AnyValue
{
	using result_type = nothing_t;
	using first_token = TokenSet<ParseState::Value, ParseState::StartArray, ParseState::StartObject>;

	template <ParseState::Enum Target>
	static ParseState::Enum runTo(Mso::Json::IJsonReader& input)
	{
		for (;;)
		{
			auto result = input.ReadNext();
			switch (result)
			{
			case Target:
				return Target;
			case ParseState::StartArray:
				runTo<ParseState::EndArray>(input);
				break;
			case ParseState::StartObject:
				runTo<ParseState::EndObject>(input);
				break;
			default:
				if (IsParseError(result))
					return result;
				continue;
			}
		}
	}

	static ParseResult<result_type> Parse(ParseState::Enum s, Mso::Json::IJsonReader& input)
	{
		switch (s)
		{
		case ParseState::StartArray:
			s = runTo<ParseState::EndArray>(input);
			break;
		case ParseState::StartObject:
			s = runTo<ParseState::EndObject>(input);
			break;
		default: break;
		}

		if (IsParseError(s))
			return ParseResult<result_type>::MakeError(s);

		s = input.ReadNext();
		return { ParseError::Ok, s, nothing };
	}
};

/**
	Attempts to match an attribute value for parsing.
*/
template <typename... Attributes>
struct AttrMatcher;

/**
	No matching attribute found, consume the entire value but don't store the result.
*/
template<>
struct AttrMatcher<>
{
	template <typename... types>
	static ParseState::Enum Match(
		ParseState::Enum s,
		Mso::Json::IJsonReader& input,
		const std::wstring& /*name*/,
		std::tuple<types...>& /*result*/)
	{
		return AnyValue::Parse(s, input).state;
	}
};

template <typename X, typename... XS>
struct AttrMatcher<X, XS...>
{
	template <typename... types>
	static ParseState::Enum Match(
		ParseState::Enum s,
		Mso::Json::IJsonReader& input,
		const std::wstring& name,
		std::tuple<types...>& results)
	{
		if (X::name == name) // Match found
		{
			auto result = X::ValueParser::Parse(s, input);
			if (result)
				std::get<X>(results).value = result.value;
			return result.state;
		}
		return AttrMatcher<XS...>::Match(s, input, name, results);
	}
};

/**
	Json key parser.
*/
using Name = Value<std::wstring, Mso::Json::ParseState::Name, StringExtractor>;

/**
	Parser for the body of an JSON object with a set of strongly typed attributes.
*/
template <
	typename... Attributes>
struct ObjectBody
{
	using result_type = std::tuple<Attributes...>;
	using first_token = TokenSet<ParseState::Name>;

	static ParseResult<result_type> Parse(ParseState::Enum s, Mso::Json::IJsonReader& input)
	{
		result_type results;

		while (Accepts<Name>(s))
		{
			auto nameResult = Name::Parse(s, input);
			s = nameResult.state;
			if (!nameResult)
				break;
			s = Details::AttrMatcher<Attributes...>::Match(s, input, nameResult.value, results);
		}

		return ParseResult<result_type>::Make(s, std::move(results));
	}
};

} // Details

/**
	Json string value parser.
*/
using String = Details::Value<std::wstring, Mso::Json::ParseState::Value, Details::StringExtractor>;

/**
	Json key parser.
*/
using Name = Details::Name;

/**
	Json boolean value parser.
*/
using Bool = Details::Value<bool, Mso::Json::ParseState::Value, Details::BoolExtractor>;

/**
	Json integer value parser.
*/
using Int = Details::Value<int, Mso::Json::ParseState::Value, Details::IntExtractor>;

/**
	Json unsigned 64 bit integer value parser.
*/
using Uint64 = Details::Value<uint64_t, Mso::Json::ParseState::Value, Details::Uint64Extractor>;

/**
	Json double integer value parser.
*/
using Double = Details::Value<double, Mso::Json::ParseState::Value, Details::DoubleExtractor>;

/**
	Parses any value type and converts it to a string.
*/
using ValueAsString = Details::Value<std::wstring, Mso::Json::ParseState::Value, Details::ExtractAsString>;

/**
	Attempts to parse `ValueParser`, but if it fails returns an empty result.

	This is only safe when `ValueParser` fails without consuming any input.

	Produces an `Either(ValueParser::result_type, nothing_t)`
*/
template <
	typename ValueParser>
using OptionalValue = Details::Either<ValueParser, Details::AnyValue>;

/**
	Parsers a JSON array of a single type.

	Produces an in-order vector of the results. Attempts to skip
	any values that `ElementParser` does not match.

	@tparam ElementParser Type of parser to run on each element.
*/
template <
	typename ElementParser>
using Array =
	Details::Between<
		Details::Consume<Details::Token<Mso::Json::ParseState::StartArray>>,
		Details::Consume<Details::Token<Mso::Json::ParseState::EndArray>>,
		Details::Accumulate<
			std::vector<typename ElementParser::result_type>,
			OptionalValue<ElementParser>,
			Details::VectorAdder>>;

/**
	Parses a JSON array of multiple, fixed types.

	Produces a tuple of results.

	@tparam Elements Parsers for each element in the array. Run in-order.
*/
template <
	typename... Elements>
using List =
	Details::Between<
		Details::Consume<Details::Token<Mso::Json::ParseState::StartArray>>,
		Details::Consume<Details::Token<Mso::Json::ParseState::EndArray>>,
		Details::Sequence<Elements...>>;

/**
	Parses a single JSON key, value pair.

	@tparam ValueParser Parser for the value.
*/
template <
	typename ValueParser>
using AttributeParser = Details::Sequence<Name, ValueParser>;

/**
	Parser for an object with a single type of value.

	Attempts to skip any values that `ElementParser` does not match.

	Produces an map of keys to result values.
*/
template <
	typename ElementParser>
using Map =
	Details::Between<
		Details::Consume<Details::Token<Mso::Json::ParseState::StartObject>>,
		Details::Consume<Details::Token<Mso::Json::ParseState::EndObject>>,
		Details::Accumulate<
			std::unordered_map<std::wstring, typename ElementParser::result_type>,
			AttributeParser<
				OptionalValue<ElementParser>>,
			Details::MapAdder>>;

/**
	Container that holds an attribute.

	Should be sub classed for use with `Object`.

	@tparam Value Parsers for value held in the attribute.
*/
template <
	typename Value>
struct Attribute
{
	using result_type = typename Value::result_type;
	using ValueParser = Value;

	result_type value;
};

/**
	Parser for an object with multiple attribute types

	Produces an map tuple of attributes.

	Skips non included attributes, but any attribute that does match must be
	parsed properly.
*/
template <
	typename... Attributes>
using Object =
	Details::Between<
		Details::Consume<Details::Token<Mso::Json::ParseState::StartObject>>,
		Details::Consume<Details::Token<Mso::Json::ParseState::EndObject>>,
		Details::ObjectBody<Attributes...>>;


/**
	Mapping from name to parsing function. Used by Structured::Class
*/
struct ClassMember
{
	using ParseMemberPtr = ParseResult<Details::nothing_t> (*)(ParseState::Enum s, Mso::Json::IJsonReader &input, void *obj);

	const wchar_t *Name = nullptr;
	ParseMemberPtr ParseMember = nullptr;
	bool IsOptional = false;
};

namespace Details
{
/**
	Parses a JSON object into an existing class, rather than a tuple.
*/
template <typename T, typename JsonMembersHolder>
struct ClassBody
{
	using first_token = Details::TokenSet<ParseState::Name>;
	using result_type = T;

private:
#pragma push_macro("max")
#undef max
	static constexpr size_t npos = std::numeric_limits<size_t>::max();
#pragma pop_macro("max")

	static size_t FindMemberByName(const Mso::basic_string_view<wchar_t> &name, size_t iGuess) noexcept
	{
		static constexpr size_t memberCount = std::size(JsonMembersHolder::json_members);
		if (name == JsonMembersHolder::json_members[iGuess].Name)
			return iGuess;
		for (size_t i = 0; i < memberCount; ++i)
			if (name == JsonMembersHolder::json_members[i].Name)
				return i;

		return npos;
	}

public:
	static ParseResult<result_type> Parse(ParseState::Enum s, Mso::Json::IJsonReader& input)
	{
		result_type result{};
		static constexpr size_t memberCount = std::size(JsonMembersHolder::json_members);

		if constexpr (memberCount > 0)
		{
			std::bitset<memberCount> memberParsed;
			size_t iGuess = 0;

			static const std::bitset<memberCount> memberIsOptional = []() noexcept
			{
				std::bitset<memberCount> isOptional;
				for (size_t i = 0; i < memberCount; ++i)
				{
					if (JsonMembersHolder::json_members[i].IsOptional)
						isOptional.set(i);
				}
				return isOptional;
			}();

			while (s == ParseState::Name)
			{
				Mso::basic_string_view<wchar_t> nameView;
				if (!input.GetString(nameView))
					return ParseResult<result_type>::MakeError(s);

				size_t i = FindMemberByName(nameView, iGuess);
				if (i == npos)
				{
					// unknown key, ignore this value
					s = input.ReadNext();
					s = AnyValue::Parse(s, input).state;
				}
				else
				{
					if (memberParsed[i])
						return ParseResult<result_type>::MakeError(s);

					const ClassMember &member = JsonMembersHolder::json_members[i];
					s = input.ReadNext(); // NB invalidates nameView
					auto memberResult = member.ParseMember(s, input, &result);
					if (!memberResult.IsSuccess())
						return ParseResult<result_type>::MakeError(s);
					s = memberResult.state;

					memberParsed.set(i);
					iGuess = i + 1;
					if (iGuess >= memberCount)
						iGuess = 0;
				}
			}

			// check if any members were not parsed
			if ((~(memberParsed | memberIsOptional)).any())
				return ParseResult<result_type>::MakeError(s);
		}

		return ParseResult<result_type>::Make(s, std::move(result));
	}
};

/**
	Helper for getting the containing class of a member pointer
*/
template <typename T>
struct MemberPointerTraits {};

template <typename TResult, typename TClass>
struct MemberPointerTraits<TResult TClass::*>
{
	using Class = TClass;
	using Result = TResult;
};

/**
	Helper for defining a function pointer that can parse a member
*/
template <typename MemberParser, auto Invocable>
ParseResult<Details::nothing_t> ParseMember(ParseState::Enum s, Mso::Json::IJsonReader &input, void *obj)
{
	using Class = typename Details::MemberPointerTraits<decltype(Invocable)>::Class;

	auto result = MemberParser::Parse(s, input);
	if (result.IsSuccess())
	{
		constexpr bool is_setter = std::is_invocable_v<decltype(Invocable), Class *, decltype(result.value)>;

		if constexpr (is_setter)
		{
			static_assert(std::is_invocable_v<decltype(Invocable), Class *, decltype(result.value)>, "expected a setter that takes an arg matching the MemberParser result type");
			static constexpr bool returns_bool = std::is_same_v<bool, std::invoke_result_t<decltype(Invocable), Class *, decltype(result.value)>>;
			static constexpr bool returns_void = std::is_same_v<void, std::invoke_result_t<decltype(Invocable), Class *, decltype(result.value)>>;

			if constexpr (returns_bool)
			{
				bool fResult = std::invoke(Invocable, static_cast<Class *>(obj), std::move(result.value));
				if (!fResult)
					return ParseResult<Details::nothing_t>::MakeError(result.state);
			}
			else if constexpr (returns_void)
			{
				std::invoke(Invocable, static_cast<Class *>(obj), std::move(result.value));
			}
			else
			{
				static_assert(returns_bool || returns_void, "we only handle setters that return void or bool");
			}
		}
		else
		{
			static_assert(std::is_invocable_v<decltype(Invocable), Class *>, "expected a member data pointer, a member function that returns a reference, or something else invocable on the class");
			std::invoke(Invocable, static_cast<Class *>(obj)) = std::move(result.value);
		}
	}
	return { result.error, result.state, Details::nothing };
}

} // namespace Details

/**
	Define a required class member for Structured::Class.

	Parameters:
		MemberParser: a Structured-compatible parser
		Invocable: Something that can be invoked on the class, usually a member data pointer
		name: The name to look for in the JSON object
*/
template <typename MemberParser, auto Invocable>
constexpr ClassMember Member(const wchar_t *name)
{
	return ClassMember {
		name,
		&Details::ParseMember<MemberParser, Invocable>
	};
}

/**
	Define an optional class member for Structured::Class.

	Parameters:
		MemberParser: a Structured-compatible parser
		Invocable: Something that can be invoked on the class, usually a member data pointer
		name: The name to look for in the JSON object
*/
template <typename MemberParser, auto Invocable>
constexpr ClassMember OptionalMember(const wchar_t *name)
{
	return ClassMember {
		name,
		&Details::ParseMember<MemberParser, Invocable>,
		true /*IsOptional*/
	};
}

/**
	Parse a JSON object directly into your own data structure. Faster, safer, and easier to use than Structured::Object.

	For a type to be usable with Structured::Class, you need to define a json_members array which maps key names to member
	variables, and tells us how to parse them. If you don't control the type you're parsing to, you can also put
	json_members in a separate class, and pass that as the JsonMembersHolder template parameter.
	
	Produces a ParseResult<T>, where T is your own type.

	Unlike Structured::Object, Structured::Class validates that every field is parsed exactly once. If there's a missing
	member variable, or a duplicated one, we'll return an error.

	Example:

		struct MyObject
		{
			int one;
			std::wstring two;

			static constexpr Structured::ClassMember json_members[] = {
				Structured::Member<Structured::Int, &MyObject::one>(L"one"),
				Structured::Member<Structured::String, &MyObject::two>(L"two"),
			};	
		};

		auto jsonReader = MakeJsonReaderSomehow("{"one": 17, "two": "seventeen"}");
		auto result = Structured::Parse<Structured::Class<MyObject>>(jsonReader);
		TestAssert::AreEqual(result.value.two, "seventeen");

*/
template <
	typename T,
	typename JsonMembersHolder = T>
using Class =
	Details::Between<
		Details::Consume<Details::Token<Mso::Json::ParseState::StartObject>>,
		Details::Consume<Details::Token<Mso::Json::ParseState::EndObject>>,
		Details::ClassBody<T, JsonMembersHolder>>;


/**
	Apply a parser to some Json input.
*/
template <typename Parser>
ParseResult<typename Parser::result_type> Parse(Mso::Json::IJsonReader& input)
{
	auto s = input.ReadNext();
	if (Details::Accepts<Parser>(s))
	{
		return Parser::Parse(s, input);
	}
	else
	{
		return ParseResult<typename Parser::result_type>::MakeError(s);
	}
}

}}} // Mso::Json::Structured


