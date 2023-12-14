// Helper type-traits used to implement Mso::variant.

#pragma once

#include <tuple>

#include "VariantUtilities.h"


namespace Mso {

template <class... Members>
class variant;

}


namespace Mso { namespace Details {


template <class Variant, class CandidateMember>
struct IsMember;

template <class... Members, class CandidateMember>
struct IsMember<variant<Members...>, CandidateMember>
{
	static const bool Value = Details::IndexOfType<CandidateMember, Members...>::Value != Details::InvalidIndex;
};


// Helper class that helps avoid a circular evaluation of std::is_move_assignable<variant>.
template <class Variant, class CandidateMember>
struct IsMoveAssignableMember;

template <class... Members, class CandidateMember>
struct IsMoveAssignableMember<variant<Members...>, CandidateMember>
{
	static const bool Value = IsMember<variant<Members...>, CandidateMember>::Value && std::is_move_assignable<CandidateMember>::value;
};

// Clang tries to evaluate this when deciding whether the assignment operator for 'Member' applies to the variant itself.
// This specialization helps short-circuit that evaluation.
template <class... Members>
struct IsMoveAssignableMember<variant<Members...>, variant<Members...>>
{
	static const bool Value = false;
};


template <class Variant>
struct IsDefaultConstructible;

template <>
struct IsDefaultConstructible<variant<>>
{
	static const bool Value = false;
};

template <class First, class... Rest>
struct IsDefaultConstructible<variant<First, Rest...>>
{
	static const bool Value = std::is_default_constructible<First>::value;
};


template <class Variant>
struct IsMoveConstructible;

template <class... Members>
struct IsMoveConstructible<variant<Members...>>
{
	static const bool Value = Details::VariadicTrait<std::is_move_constructible, Members...>::Value;
};


template <class Variant>
struct IsCopyConstructible;

template <class... Members>
struct IsCopyConstructible<variant<Members...>>
{
	// Mso::variant has a somewhat artificial limitation:
	// If the variant is not move-constructible, then it is not copy-constructible either.
	// This is driven by a limitation in the GeneratedMethodsBase technique for suppressing auto-generated methods.
	// We err on the side of caution here, vs. the alternative which would be to allow the compiler
	// to occasionally generate a move constructor when we don't expect it to.
	// An auto-generated move constructor for Mso::variant would be unsafe, because it would basically just memcpy
	// the storage buffer without calling constructors and destructors on the member type.
	// This would lead to all kinds of correctness issues (for example, memory leaks when smart pointers are used
	// in Mso::variant).
	static const bool Value =
		IsMoveConstructible<variant<Members...>>::Value &&
		Details::VariadicTrait<std::is_copy_constructible, Members...>::Value;
};


template <class Variant>
struct IsMoveAssignable;

template <class... Members>
struct IsMoveAssignable<variant<Members...>>
{
	static const bool Value =
		Details::VariadicTrait<std::is_move_constructible, Members...>::Value &&
		Details::VariadicTrait<std::is_move_assignable, Members...>::Value;
};


template <class Variant>
struct IsCopyAssignable;

template <class... Members>
struct IsCopyAssignable<variant<Members...>>
{
	// See comments for IsCopyConstructible.  The same limitation applies here:
	// If the variant is not move-assignable, then it cannot be copy-assignable either.
	static const bool Value =
		IsMoveAssignable<variant<Members...>>::Value &&
		Details::VariadicTrait<std::is_copy_constructible, Members...>::Value &&
		Details::VariadicTrait<std::is_copy_assignable, Members...>::Value;
};


}}