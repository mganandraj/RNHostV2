#pragma once

#include <tuple>
#include <variant>
#include <Variant/CallbackSignature.h>
#include "VariantUtilities.h"

namespace Mso { namespace Details {


template <size_t...>
struct SequenceContains;

template <size_t N>
struct SequenceContains<N>
{
	static const bool Value = false;
};

template <size_t N, size_t Head, size_t... Tail>
struct SequenceContains<N, Head, Tail...>
{
	static const bool Value = (N == Head ? true : SequenceContains<N, Tail...>::Value);
};


template <class A, class B>
struct ConcatPacks;

template <class... A, class... B>
struct ConcatPacks<ParameterPack<A...>, ParameterPack<B...>>
{
	using Type = ParameterPack<A..., B...>;
};

template <class Head, class... Tail>
struct DecayParameterPack
{
	using Type = typename ConcatPacks<
		ParameterPack<std::decay_t<Head>>,
		typename DecayParameterPack<Tail...>::Type>::Type;
};

template <class T>
struct DecayParameterPack<T>
{
	using Type = ParameterPack<std::decay_t<T>>;
};


template <class ReturnType, class T>
struct FunctionPointerFromParameterPack;

template <class ReturnType, class... Parameters>
struct FunctionPointerFromParameterPack<ReturnType, ParameterPack<Parameters...>>
{
	using Type = ReturnType (*)(Parameters...);
};


template <class T>
struct DecayFunctionSignature;

template <class ReturnType, class... Parameters>
struct DecayFunctionSignature<ReturnType (*)(Parameters...)>
{
private:
	using ParameterPackType = typename DecayParameterPack<Parameters...>::Type;

public:
	using Type = typename FunctionPointerFromParameterPack<ReturnType, ParameterPackType>::Type;
};


template <class A, class B>
struct FunctionPointerIsCompatibleWith;

template <class ReturnType1, class... Parameters1, class ReturnType2, class... Parameters2>
struct FunctionPointerIsCompatibleWith<ReturnType1 (*)(Parameters1...), ReturnType2 (*)(Parameters2...)>
{
	static const bool Value =
		std::is_same<ParameterPack<Parameters1...>, ParameterPack<Parameters2...>>::value &&
		std::is_convertible<ReturnType2, ReturnType1>::value;
};

template <class Signature, class... Callbacks>
struct IndexOfCallbackWithSignature;

template <class Signature, class Head, class... Tail>
struct IndexOfCallbackWithSignature<Signature, Head, Tail...>
{
private:
	using TailIndex = IndexOfCallbackWithSignature<Signature, Tail...>;

	using DecayedExpectedSignature = typename DecayFunctionSignature<Signature>::Type;
	using PossibleSignature = Mso::CallbackSignature<Head>;
	using DecayedPossibleSignature = typename DecayFunctionSignature<PossibleSignature>::Type;

public:
	static const IndexType Value =
		FunctionPointerIsCompatibleWith<DecayedExpectedSignature, DecayedPossibleSignature>::Value ?
		static_cast<IndexType>(0) :
		(TailIndex::Value == InvalidIndex ? InvalidIndex : IncrementIndex(TailIndex::Value));
};

template <class Signature>
struct IndexOfCallbackWithSignature<Signature>
{
	static const size_t Value = InvalidIndex;
};


template <class Member, bool ShowError>
struct UnhandledMemberError
{
	// We take the ShowError bool parameter because if this just said "static_assert(false, ...)" it would cause MSVC to produce
	// a much less useful compile error.
	static_assert(!ShowError, "Not enough handlers provided for 'match'.  Look for 'UnhandledMemberError' in the compiler errors; the first type parameter is for the member that isn't handled.");
};


template <class Variant, class ReturnType, bool IsCatchAll>
struct ExecuteOnMemberOrCatchAll;

template <class Variant, class ReturnType>
struct ExecuteOnMemberOrCatchAll<Variant, ReturnType, false /*IsCatchAll*/>
{
	template <class Callback, class Member>
	static ReturnType Execute(const Callback& callback, const Member& member, const Variant& /*variant*/)
		noexcept(noexcept(callback(member)))
	{
		return callback(member);
	}

	template <class Callback, class Member>
	static ReturnType Execute(const Callback& callback, Member&& member, Variant&& /*variant*/)
		noexcept(noexcept(callback(std::forward<Member>(member))))
	{
		return callback(std::forward<Member>(member));
	}
};

template <class Variant, class ReturnType>
struct ExecuteOnMemberOrCatchAll<Variant, ReturnType, true /*IsCatchAll*/>
{
	template <class Callback, class Member>
	static ReturnType Execute(const Callback& callback, const Member& /*member*/, const Variant& variant)
		noexcept(noexcept(callback(variant)))
	{
		return callback(variant);
	}

	template <class Callback, class Member>
	static ReturnType Execute(const Callback& callback, Member&& /*member*/, Variant&& variant)
		noexcept(noexcept(callback(std::move(variant))))
	{
		return callback(std::move(variant));
	}
};


template <size_t MatchIndex, bool IsCatchAll>
struct MatchInfo
{
};

template <class Variant, class Member, class ReturnType, class... Callbacks>
struct FindMatch
{
	using ExpectedCallbackSignature = ReturnType (*)(const Member&);
	using CatchAllSignature = ReturnType (*)(const Variant&);

	static const auto MemberMatchIndex = IndexOfCallbackWithSignature<ExpectedCallbackSignature, Callbacks...>::Value;
	static const auto CatchAllMatchIndex = IndexOfCallbackWithSignature<CatchAllSignature, Callbacks...>::Value;

	static const auto Index = MemberMatchIndex != InvalidIndex ? MemberMatchIndex : CatchAllMatchIndex;
	static const auto IsCatchAll = MemberMatchIndex == InvalidIndex;

	using Type = MatchInfo<Index, IsCatchAll>;
};


template <class ReturnType, class Member, class Variant, class MatchInfo, class... Callbacks>
struct ExecuteCallbackWithMatchInfo;

template <class ReturnType, class Member, class Variant, int MatchIndex, bool IsCatchAll, class... Callbacks>
struct ExecuteCallbackWithMatchInfo<ReturnType, Member, Variant, MatchInfo<MatchIndex, IsCatchAll>, Callbacks...>
{
	static ReturnType Execute(const Member& member, const Variant& variant, const std::tuple<const Callbacks*...>& callbackTuple)
		noexcept(noexcept(ExecuteOnMemberOrCatchAll<Variant, ReturnType, IsCatchAll>::Execute(*std::get<MatchIndex>(callbackTuple), member, variant)))
	{
		const auto pCallback = std::get<MatchIndex>(callbackTuple);

		return ExecuteOnMemberOrCatchAll<Variant, ReturnType, IsCatchAll>::Execute(*pCallback, member, variant);
	}

	static ReturnType Execute(Member&& member, Variant&& variant, const std::tuple<const Callbacks*...>& callbackTuple)
		noexcept(noexcept(ExecuteOnMemberOrCatchAll<Variant, ReturnType, IsCatchAll>::Execute(*std::get<MatchIndex>(callbackTuple), std::move(member), std::move(variant))))
	{
		const auto pCallback = std::get<MatchIndex>(callbackTuple);

		return ExecuteOnMemberOrCatchAll<Variant, ReturnType, IsCatchAll>::Execute(*pCallback, std::move(member), std::move(variant));
	}
};

template <class ReturnType, class Member, class Variant, bool IsCatchAll, class... Callbacks>
struct ExecuteCallbackWithMatchInfo<ReturnType, Member, Variant, MatchInfo<InvalidIndex /*MatchingIndex*/, IsCatchAll>, Callbacks...>
{
	static ReturnType Execute(const Member& /*member*/, const Variant& /*variant*/, const std::tuple<const Callbacks*...>& /*callbackTuple*/) noexcept
	{
		// Show the unhandled-member error at compile-time by instantiating the UnhandledMemberError type.
		UnhandledMemberError<Member, true /*ShowError*/> ignored;
		(ignored);
	}

	static ReturnType Execute(Member&& /*member*/, Variant&& /*variant*/, const std::tuple<const Callbacks*...>& /*callbackTuple*/) noexcept
	{
		// Show the unhandled-member error at compile-time by instantiating the UnhandledMemberError type.
		UnhandledMemberError<Member, true /*ShowError*/> ignored;
		(ignored);
	}
};


template <size_t N, size_t... List>
struct OccurrenceCount;

template <size_t N, size_t Head, size_t... Tail>
struct OccurrenceCount<N, Head, Tail...>
{
	static const size_t BaseCount = OccurrenceCount<N, Tail...>::Value;
	static const size_t Value = (N == Head ? BaseCount + 1 : BaseCount);
};

template <size_t N>
struct OccurrenceCount<N>
{
	static const size_t Value = 0;
};

template <class T, size_t... UsedIndexes>
struct FirstUnusedIndexImpl;

template <size_t Head, size_t... Tail, size_t... UsedIndexes>
struct FirstUnusedIndexImpl<std::index_sequence<Head, Tail...>, UsedIndexes...>
{
	static const auto Occurrences = OccurrenceCount<Head, UsedIndexes...>::Value;
	static const auto FirstIndexAfter = FirstUnusedIndexImpl<std::index_sequence<Tail...>, UsedIndexes...>::Value;

	static const size_t Value = Occurrences == 0 ? Head : FirstIndexAfter;
};

template <size_t... UsedIndexes>
struct FirstUnusedIndexImpl<std::index_sequence<>, UsedIndexes...>
{
	static const size_t Value = InvalidIndex;
};

template <size_t Index, bool ShowError>
struct ShowUnusedIndexError
{
	// We take the ShowError bool parameter because if this just said "static_assert(false, ...)" it would cause MSVC to produce
	// a much less useful compile error.
	static_assert(!ShowError, "Not all callback objects are used in your 'match' function.  Look for 'ShowUnusedIndexError' in the compiler errors; the index argument for the 'ShowUnusedIndexError' type is the zero-based index of the callback that will never be called.");
};

template <bool ShowError>
struct ShowUnusedIndexError<InvalidIndex, ShowError>
{
};

template <size_t Count, size_t... UsedIndexes>
struct VerifyUsedIndexes
{
	static const auto FirstUnusedIndex =
		FirstUnusedIndexImpl<std::make_index_sequence<Count>, UsedIndexes...>::Value;

	using Verify = ShowUnusedIndexError<FirstUnusedIndex, true /*ShowError*/>;
};

template <class Variant, class ReturnType, class CallbackPack, class IndexSequence, class... Members>
struct VerifyAllCallbacksUsedBase;

template <class Variant, class ReturnType, class... Callbacks, size_t... UsedIndexes, class HeadMember, class... TailMembers>
struct VerifyAllCallbacksUsedBase<Variant, ReturnType, ParameterPack<Callbacks...>, std::index_sequence<UsedIndexes...>, HeadMember, TailMembers...>
{
	using Match = typename FindMatch<Variant, HeadMember, ReturnType, Callbacks...>::Type;

	template <class T>
	struct VerificationHelper;

	template <int MatchIndex, bool IsCatchAll>
	struct VerificationHelper<MatchInfo<MatchIndex, IsCatchAll>>
	{
		using Verify = typename VerifyAllCallbacksUsedBase<Variant, ReturnType, ParameterPack<Callbacks...>, std::index_sequence<UsedIndexes..., MatchIndex>, TailMembers...>::Verify;
	};

	using Verify = typename VerificationHelper<Match>::Verify;
};

template <class Variant, class ReturnType, class... Callbacks, size_t... UsedIndexes>
struct VerifyAllCallbacksUsedBase<Variant, ReturnType, ParameterPack<Callbacks...>, std::index_sequence<UsedIndexes...>>
{
	using Verify = typename VerifyUsedIndexes<sizeof...(Callbacks), UsedIndexes...>::Verify;
};

template <class Variant, class ReturnType, class CallbackPack, class... Members>
struct VerifyAllCallbacksUsed : public VerifyAllCallbacksUsedBase<Variant, ReturnType, CallbackPack, std::index_sequence<>, Members...>
{
};


template <class ReturnType, class Variant, class... Callbacks>
class ExecuteMatchingCallbackVisitor
{
public:
	ExecuteMatchingCallbackVisitor(const Variant& variant, const Callbacks&... callbacks) noexcept :
		m_variant(variant),
		m_callbackTuple(std::make_tuple<const Callbacks*...>(&callbacks...))
	{
	}

	template <class Member>
	ReturnType operator()(const Member& member) const
		noexcept(noexcept(ExecuteCallbackWithMatchInfo<ReturnType, Member, Variant, typename FindMatch<Variant, Member, ReturnType, Callbacks...>::Type, Callbacks...>::Execute(member, m_variant, m_callbackTuple)))
	{
		using MatchInfo = typename FindMatch<Variant, Member, ReturnType, Callbacks...>::Type;

		return ExecuteCallbackWithMatchInfo<ReturnType, Member, Variant, MatchInfo, Callbacks...>::Execute(
			member,
			m_variant,
			m_callbackTuple);
	}

private:
	const Variant& m_variant;
	// We store a tuple of const Callback* (vs. const Callback&) because if we stored 'const Callback&...',
	// then make_tuple would end up invoking the copy constructor for each callback object, and we'd end up
	// storing references to temporary callbacks that would be cleared from the stack.
	// With 'const Callback*', we get the result we want, which is a tuple of pointers to valid callbacks
	// that are further up the stack.
	std::tuple<const Callbacks*...> m_callbackTuple;
};


template <class ReturnType, class Variant, class... Callbacks>
class ExecuteMatchingCallbackWithMoveVisitor
{
public:
	ExecuteMatchingCallbackWithMoveVisitor(Variant& variant, const Callbacks&... callbacks) noexcept :
		m_variant(variant),
		m_callbackTuple(std::make_tuple<const Callbacks*...>(&callbacks...))
	{
	}

	// The enable_if expression prevents this overload from matching an lvalue reference (SomeMemberType&).
	template <class Member>
	std::enable_if_t<std::is_same<Member, std::remove_reference_t<Member>>::value, ReturnType>
		operator()(Member&& member) const
		noexcept(noexcept(ExecuteCallbackWithMatchInfo<ReturnType, Member, Variant, typename FindMatch<Variant, Member, ReturnType, Callbacks...>::Type, Callbacks...>::Execute(std::forward<Member>(member), std::move(m_variant), m_callbackTuple)))
	{
		using MatchInfo = typename FindMatch<Variant, Member, ReturnType, Callbacks...>::Type;

		return ExecuteCallbackWithMatchInfo<ReturnType, Member, Variant, MatchInfo, Callbacks...>::Execute(
			std::forward<Member>(member),
			std::move(m_variant),
			m_callbackTuple);
	}

private:
	Variant& m_variant;
	// We store a tuple of const Callback* (vs. const Callback&) because if we stored 'const Callback&...',
	// then make_tuple would end up invoking the copy constructor for each callback object, and we'd end up
	// storing references to temporary callbacks that would be cleared from the stack.
	// With 'const Callback*', we get the result we want, which is a tuple of pointers to valid callbacks
	// that are further up the stack.
	std::tuple<const Callbacks*...> m_callbackTuple;
};


#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
#endif

template <class ReturnType, class Variant, class InvalidVariantPolicy, class... Callbacks>
struct VariantMatchImpl;

template <class ReturnType, class... Members, class InvalidVariantPolicy, class... Callbacks>
struct VariantMatchImpl<ReturnType, std::variant<Members...>, InvalidVariantPolicy, Callbacks...>
{
	using Variant = std::variant<Members...>;

	static ReturnType Match(const Variant& variant, const Callbacks&... callbacks)
		// This noexcept logic doesn't precisely match the code below, because std::visit cannot be
		// conditionally noexcept, even if all the callbacks are. This is because it throws a bad_variant_access
		// exception in the rare case where the variant contains "nothing" because assignment/emplacement
		// threw, leaving the variant in an invalid state.
		//
		// Mso::match handles this differently: the InvalidVariantPolicy parameter controls how this corner
		// case is handled, and some policy implementations are noexcept, allowing us to guarantee that
		// std::visit won't actually throw.
		noexcept(noexcept(InvalidVariantPolicy::template OnInvalidVariant<ReturnType>()) &&
				(noexcept(std::declval<ExecuteMatchingCallbackVisitor<ReturnType, Variant, Callbacks...>>()(std::declval<Members>())) && ...))
	{
		using VerifyUsed = typename Details::VerifyAllCallbacksUsed<
			Variant,
			ReturnType,
			ParameterPack<Callbacks...>,
			Members...>::Verify;

		VerifyUsed dummy;
		(dummy);

		// If the variant contains no value (due to an exception during assignment), invoke the
		// InvalidVariantPolicy to handle it
		if (variant.valueless_by_exception())
			return InvalidVariantPolicy::template OnInvalidVariant<ReturnType>();

		return visit(
			ExecuteMatchingCallbackVisitor<ReturnType, Variant, Callbacks...>(variant, callbacks...),
			variant);
	}

	static ReturnType Match(Variant&& variant, const Callbacks&... callbacks)
		// This noexcept logic doesn't precisely match the code below, because std::visit cannot be
		// conditionally noexcept, even if all the callbacks are. This is because it throws a bad_variant_access
		// exception in the rare case where the variant contains "nothing" because assignment/emplacement
		// threw, leaving the variant in an invalid state.
		//
		// Mso::match handles this differently: the InvalidVariantPolicy parameter controls how this corner
		// case is handled, and some policy implementations are noexcept, allowing us to guarantee that
		// std::visit won't actually throw.
		noexcept(noexcept(InvalidVariantPolicy::template OnInvalidVariant<ReturnType>()) &&
				(noexcept(std::declval<ExecuteMatchingCallbackWithMoveVisitor<ReturnType, Variant, Callbacks...>>()(std::declval<Members>())) && ...))
	{
		using VerifyUsed = typename Details::VerifyAllCallbacksUsed<
			Variant,
			ReturnType,
			ParameterPack<Callbacks...>,
			Members...>::Verify;

		VerifyUsed dummy;
		(dummy);

		// If the variant contains no value (due to an exception during assignment), invoke the
		// InvalidVariantPolicy to handle it
		if (variant.valueless_by_exception())
			return InvalidVariantPolicy::template OnInvalidVariant<ReturnType>();

		return visit(
			ExecuteMatchingCallbackWithMoveVisitor<ReturnType, Variant, Callbacks...>(variant, callbacks...),
			std::move(variant));
	}
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif


}}
