/**
	One of the contracts of Mso::variant is that std::is_copy_constructible<variant<T...>> is
	only true if std::is_copy_constructible<T> is true for all member T types.

	Because Mso::variant stores its data as an opaque buffer, the compiler does not satisfy this
	contract automatically.  So it is up to us to properly enable the copy and move constructors
	based on the type traits of each of the member types.

	The technique used in this file is described well in Andrzej Krzemienski's blog post:
	https://akrzemi1.wordpress.com/2015/03/02/a-conditional-copy-constructor/

	By specifying "= delete" on a base class, we can suppress auto-generation of that particular constructor or
	assignment operator.
*/

#pragma once

namespace Mso { namespace Details {


template <bool>
struct CopyConstructible { };

template <bool>
struct MoveConstructible { };

template <bool>
struct CopyAssignable { };

template <bool>
struct MoveAssignable { };


template <
	class CopyConstructibleArg,
	class MoveConstructibleArg,
	class CopyAssignableArg,
	class MoveAssignableArg>
struct GeneratedMethodsBase;


#define GENERATED_METHODS_BASE_SPECIALIZATION(isCopyConstructible, isMoveConstructible, isCopyAssignable, isMoveAssignable, copyConstructImpl, moveConstructImpl, copyAssignImpl, moveAssignImpl) \
	template<> \
	struct GeneratedMethodsBase< \
		CopyConstructible<isCopyConstructible>, \
		MoveConstructible<isMoveConstructible>, \
		CopyAssignable<isCopyAssignable>, \
		MoveAssignable<isMoveAssignable>> \
	{ \
		GeneratedMethodsBase() = default; \
		GeneratedMethodsBase(const GeneratedMethodsBase&) = copyConstructImpl; \
		GeneratedMethodsBase(GeneratedMethodsBase&&) = moveConstructImpl; \
		GeneratedMethodsBase& operator =(const GeneratedMethodsBase&) = copyAssignImpl; \
		GeneratedMethodsBase& operator =(GeneratedMethodsBase&&) = moveAssignImpl; \
	};

// The GeneratedMethodsBase technique has an important limitation:
// If a base class has a copy constructor, then a derived class will still have an auto-generated move constructor.
// (I haven't confirmed whether this is in the C++ standard, but MSVC and Clang both seem consistent on this point.)
// This means that if you want to suppress an auto-generated move constructor via this base class, then you also need
// to suppress the auto-generated copy constructor in the base class.
// Therefore, it's unsupported to have a GeneratedMethodsBase with CopyConstructible<true> and MoveConstructible<false>.
// This specialization on that case is commented out, meaning that it is undefined.
// It is up to the consumer of GeneratedMethodsBase to avoid using that case.
// The same limitation applies to assignment operators.
GENERATED_METHODS_BASE_SPECIALIZATION(true, true, true, true, default, default, default, default)
//GENERATED_METHODS_BASE_SPECIALIZATION(true, true, true, false, default, default, default, delete)
GENERATED_METHODS_BASE_SPECIALIZATION(true, true, false, true, default, default, delete, default)
GENERATED_METHODS_BASE_SPECIALIZATION(true, true, false, false, default, default, delete, delete)
//GENERATED_METHODS_BASE_SPECIALIZATION(true, false, true, true, default, delete, default, default)
//GENERATED_METHODS_BASE_SPECIALIZATION(true, false, true, false, default, delete, default, delete)
//GENERATED_METHODS_BASE_SPECIALIZATION(true, false, false, true, default, delete, delete, default)
//GENERATED_METHODS_BASE_SPECIALIZATION(true, false, false, false, default, delete, delete, delete)
GENERATED_METHODS_BASE_SPECIALIZATION(false, true, true, true, delete, default, default, default)
//GENERATED_METHODS_BASE_SPECIALIZATION(false, true, true, false, delete, default, default, delete)
GENERATED_METHODS_BASE_SPECIALIZATION(false, true, false, true, delete, default, delete, default)
GENERATED_METHODS_BASE_SPECIALIZATION(false, true, false, false, delete, default, delete, delete)
GENERATED_METHODS_BASE_SPECIALIZATION(false, false, true, true, delete, delete, default, default)
//GENERATED_METHODS_BASE_SPECIALIZATION(false, false, true, false, delete, delete, default, delete)
GENERATED_METHODS_BASE_SPECIALIZATION(false, false, false, true, delete, delete, delete, default)
GENERATED_METHODS_BASE_SPECIALIZATION(false, false, false, false, delete, delete, delete, delete)


}}