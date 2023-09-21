/**
\file

Implementation of an ML-style 'match' pattern using std::variant.

Extracting the value from a std::variant usually requires implementing a visitor class
which implements 'operator ()' for every member type of the variant.

match makes that easier by allowing you specify a set of lambdas, one of which will be
executed on the variant, depending on the variant's value.

match has not been discussed as part of the eventual std::variant pattern, so match
is an Mso extension that is built on Mso/std::variant.

Sample code:

	using MyVariant = std::variant<double, int, std::shared_ptr<Foo>>;

	auto v = MyVariant(3.8);

	auto description = Mso::match(
		v,
		[](double) { return L"double floating-point number"; },
		[](int) { return L"integer"; },
		[](const std::shared_ptr<Foo>&) { return L"Foo object"; });

	// 'description' is now set to "double floating-point number".

match has several features, which are inspired by the behavior of 'match' in functional languages
like ML.  These features provide added compile-time safety that make variants even more useful.

	(a) There will be a compile error if you don't handle all the possible cases.

			using MyVariant = std::variant<double, int, std::shared_ptr<Foo>>;

			auto v = MyVariant(3.8);

			auto description = Mso::match(
				v,
				[](double) { return L"double floating-point number"; },
				[](int) { return L"integer"; });

				// compile error: Didn't handle the case of 'std::shared_ptr<Foo>'.


	(b) There will be a compile error if you handle *too many* cases.

			using MyVariant = std::variant<double, int, std::shared_ptr<Foo>>;

			auto v = MyVariant(3.8);

			auto description = Mso::match(
				v,
				[](double) { return L"double floating-point number"; },
				[](int) { return L"integer"; },
				[](const std::shared_ptr<Foo>&) { return L"Foo object"; },
				[](const Bar& bar) { return L"Bar object"; });

				// compile error: 'Bar' is not a member of the 'MyVariant' type.


	(c) You can specify handlers in any order:

			using MyVariant = std::variant<double, int, std::shared_ptr<Foo>>;

			auto variant = MyVariant(3.8);

			auto description = Mso::match(
				v,
				[](const std::shared_ptr<Foo>&) { return L"Foo object"; },
				[](double) { return L"double floating-point number"; },
				[](int) { return L"integer"; });


	(d) You can provide a default handler that takes the variant object as a whole:

			using MyVariant = std::variant<double, int, std::shared_ptr<Foo>>;

			auto v = MyVariant(3.8);

			auto description = Mso::match(
				v,
				[](const std::shared_ptr<Foo>&) { return L"Foo object"; },
				[](const MyVariant& v) { return ScalarDescription(v); });

match is noexcept if all handlers are themselves noexcept *and* if the InvalidVariantPolicy parameter is
noexcept. This policy defines what happens when an invalid variant is encountered. If unspecified, the
InvalidVariantPolicy defaults to ThrowOnInvalidVariant, so if a non-throwing match() is desired, the
InvalidVariantPolicy must be specified as an explicit template parameter:

	using MyVariant = std::variant<double, int>;

	auto v = MyVariant(3.8);

	// This will be noexcept, because all handlers are, and Mso::DefaultOnInvalidVariant is also noexcept,
	// because default-constructing the return type (const wchar_t*) is non-throwing.
	auto result = Mso::match<Mso::DefaultOnInvalidVariant>(
		v,
		[](double) noexcept { return L"double floating-point number"; },
		[](int) noexcept { return L"integer"; });
*/

#pragma once

#include <core/restricthelper.h>
#include <crash/verifyelsecrash.h>

#include "Details/CommonReturnType.h"
#include "Details/VariantMatchImpl.h"

namespace Mso {

// An "invalid variant policy" that returns a default-constructed value when an invalid variant is encountered
struct DefaultOnInvalidVariant
{
	template <typename ReturnType>
	static ReturnType OnInvalidVariant() noexcept(noexcept(ReturnType()))
	{
		return {};
	}
};


// An "invalid variant policy" that crashes when an invalid variant is encountered
struct CrashOnInvalidVariant
{
	template <typename ReturnType>
	static ReturnType OnInvalidVariant() noexcept
	{
		VerifyElseCrashTag(false, 0x1e5462df /* tag_4vgl5 */);
	}
};


// An "invalid variant policy" that throws an exception when an invalid variant is encountered
struct ThrowOnInvalidVariant
{
	template <typename ReturnType>
	static ReturnType OnInvalidVariant()
	{
		throw std::bad_variant_access(); // This is what std::visit() throws in this circumstance
	}
};


template <class InvalidVariantPolicy, class Variant, class... Callbacks,
			MSO_ENABLE_FUNC_IF(!std::is_same<InvalidVariantPolicy, Variant>::value)>
Details::CommonReturnType<Callbacks...> match(const Variant& v, const Callbacks&... callbacks)
	noexcept(noexcept(Details::VariantMatchImpl<Details::CommonReturnType<Callbacks...>, Variant, InvalidVariantPolicy, Callbacks...>::Match(v, callbacks...)))
{
	using ReturnType = Details::CommonReturnType<Callbacks...>;
	static_assert(!std::is_same<ReturnType, Details::InconsistentReturnType>::value, "The callbacks you have provided to 'match' do not return the same type, so 'match' is unable to infer its own return type.");

	return Details::VariantMatchImpl<ReturnType, Variant, InvalidVariantPolicy, Callbacks...>::Match(v, callbacks...);
}

// Back-compat overload of the previous function with hard-coded InvalidVariantPolicy
template <class Variant, class... Callbacks>
Details::CommonReturnType<Callbacks...> match(const Variant& v, const Callbacks&... callbacks)
	noexcept(noexcept(match<ThrowOnInvalidVariant>(v, callbacks...)))
{
	return match<ThrowOnInvalidVariant>(v, callbacks...);
}

// The enable_if expression protects against a universal reference binding to an lvalue reference (variant&), which wouldn't build.
template <class InvalidVariantPolicy, class Variant, class... Callbacks,
			MSO_ENABLE_FUNC_IF(std::is_same<Variant, std::remove_reference_t<Variant>>::value),
			MSO_ENABLE_FUNC_IF(!std::is_same<InvalidVariantPolicy, Variant>::value)>
Details::CommonReturnType<Callbacks...> match(Variant&& v, const Callbacks&... callbacks)
	noexcept(noexcept(Details::VariantMatchImpl<Details::CommonReturnType<Callbacks...>, Variant, InvalidVariantPolicy, Callbacks...>::Match(std::forward<Variant>(v), callbacks...)))
{
	using ReturnType = Details::CommonReturnType<Callbacks...>;
	static_assert(!std::is_same<ReturnType, Details::InconsistentReturnType>::value, "The callbacks you have provided to 'match' do not return the same type, so 'match' is unable to infer its own return type.");

	return Details::VariantMatchImpl<ReturnType, Variant, InvalidVariantPolicy, Callbacks...>::Match(std::forward<Variant>(v), callbacks...);
}

// Back-compat overload of the previous function with hard-coded InvalidVariantPolicy
template <class Variant, class... Callbacks,
			MSO_ENABLE_FUNC_IF(std::is_same<Variant, std::remove_reference_t<Variant>>::value)>
Details::CommonReturnType<Callbacks...> match(Variant&& v, const Callbacks&... callbacks)
	noexcept(noexcept(match<ThrowOnInvalidVariant>(std::forward<Variant>(v), callbacks...)))
{
	return match<ThrowOnInvalidVariant>(std::forward<Variant>(v), callbacks...);
}

}
