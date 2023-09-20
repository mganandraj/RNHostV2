#pragma once
#ifndef LIBLET_STLEXTENSIONS_PRIVATE_CPP20_H
#define LIBLET_STLEXTENSIONS_PRIVATE_CPP20_H

#include <type_traits>
#include <map>
#include <sal.h>
#if _HAS_CXX23 || _LIBCPP_STD_VER >= 23 // C++23 vvv

#error please delete this file

#elif _HAS_CXX20 || (_LIBCPP_STD_VER >= 20 && !defined(_LIBCPP_HAS_NO_INCOMPLETE_RANGES)) // ^^^ C++23 / C++20 vvv

// all algorithms
#include <algorithm>

// indentity
#include <functional>

// std::ssize
#include <iterator>

// erase_if especialization
#include <map>

#else // ^^^ C++20 / C++17 vvv

#include <algorithm>

namespace std {

#if !__clang__ || _LIBCPP_STD_VER <= 17
// specialization for std::map - https://en.cppreference.com/w/cpp/container/map/erase_if
// LLVM STL has this template specialization when C++20 is enabled so we don't need/want it
// if we're in here because of older LLVM STL bits missing pieces of std::ranges.
template <class Key, class T, class Compare, class Allocator, class Predicate>
typename map<Key, T, Compare, Allocator>::size_type erase_if(map<Key, T, Compare, Allocator>& c, Predicate pred)
{
	auto old_size = c.size();
	for (auto i = c.begin(), last = c.end(); i != last;)
	{
		if (pred(*i))
		{
			i = c.erase(i);
		}
		else
		{
			++i;
		}
	}
	return old_size - c.size();
}
#endif // !__clang__ || _LIBCPP_STD_VER <= 17

/**
	std::identity - https://en.cppreference.com/w/cpp/utility/functional/identity

		array arr {true, false};
		bool has_true = std::any_of(arr, identity);

	is a function object type whose operator() returns its argument unchanged.
	As implemeneted in the MSVC STL:
	https://github.com/microsoft/STL/blob/a6c10429b6326aa92b3e410ed6a1f3fa13ffa571/stl/inc/xutility#L187-L195
*/
struct identity
{
	template <typename T>
	[[nodiscard]] constexpr T&& operator()(T&& _Left) const noexcept
	{
		return std::forward<T>(_Left);
	}

	using is_transparent = int;
};
} // namespace std


namespace std::constexpr_ {
/**
	std::constexpr_::find(begin, end, x);
	note: this implementation should be replaced with std::find once it is made constexpr in c++20,
	std::ranges::find and std::find have different requirements, so we need this implementation to
	compile the code in both modes (c++17 and c++20)
*/
template <typename Iter, typename Val>
constexpr Iter find(Iter first, const Iter last, const Val& t) {
	for (auto it = first; it != last; ++it)
	{
		if (*it == t)
		{
			return it;
		}
	}
	return last;
}
}


namespace std::ranges {

/**
	std::find(myCollection, x);
	(note: this implementation should be replaced with std::find once it is made constexpr in c++20)
*/
template<typename Col, typename T>
constexpr auto find(Col &&col, const T &t)
{
	using std::begin;
	using std::end;
	return std::constexpr_::find(begin(col), end(col), t);
}

/**
	std::ranges::find_if(myCollection, [] { return ... });
	(note: this implementation should be replaced with std::find_if once it is made constexpr in c++20)
*/
template<typename Col, typename Pred>
constexpr auto find_if(Col &&col, const Pred &pred)
{
	using std::begin;
	using std::end;
	auto last = end(col);
	for (auto it = begin(col); it != last; ++it)
	{
		if (pred(*it))
		{
			return it;
		}
	}
	return last;
}

/**
	std::ranges::fill(myCollection, t);
*/
template<typename Col, typename T>
constexpr void fill(const Col &&col, const T &t)
{
	using std::begin;
	using std::end;
	std::fill(begin(col), end(col), t);
}

/**
	std::ranges::copy(col1, output);
*/
template<typename T1, typename OutputIt>
constexpr OutputIt copy(const T1 &t1, const OutputIt &output) noexcept
{
	using std::begin;
	using std::end;
	return std::copy(begin(t1), end(t1), output);
}

/**
	std::ranges::copy_if(col1, output, pred);
*/
template<typename T1, typename OutputIt, typename UnaryPredicate>
constexpr OutputIt copy_if(const T1 &t1, OutputIt output, UnaryPredicate pred) noexcept
{
	using std::begin;
	using std::end;
	return std::copy_if(begin(t1), end(t1), output, pred);
}

/**
	std::ranges::count(col, value);
*/
template<typename Col, typename T>
constexpr auto count(Col &&col, T &&value)
{
	using std::begin;
	using std::end;
	return std::count(begin(std::forward<Col>(col)), end(std::forward<Col>(col)), std::forward<T>(value));
}

/**
	std::ranges::count_if(col, [] { return ... });
*/
template<typename Col, typename Pred>
constexpr auto count_if(Col &&col, Pred &&pred)
{
	using std::begin;
	using std::end;
	return std::count_if(begin(std::forward<Col>(col)), end(std::forward<Col>(col)), std::forward<Pred>(pred));
}

/**
	std::ranges::equal(col1, col2);
*/
template<typename T1, typename T2>
constexpr bool equal(const T1 &t1, const T2 &t2) noexcept
{
	using std::begin;
	using std::end;
	return std::equal(begin(t1), end(t1), begin(t2), end(t2));
}

/**
	std::ranges::equal(col1, col2, pred);
*/
template<typename T1, typename T2, typename Pred>
constexpr bool equal(const T1 &t1, const T2 &t2, const Pred &pred) noexcept
{
	using std::begin;
	using std::end;
	return std::equal(begin(t1), end(t1), begin(t2), end(t2), pred);
}

/**
	std::ranges::all_of(myCollection, pred);
*/
template<typename Col, typename Pred>
constexpr bool all_of(const Col &col, const Pred &pred)
{
	using std::begin;
	using std::end;
	return std::all_of(begin(col), end(col), pred);
}

/**
	std::ranges::any_of(myCollection, pred);
*/
template<typename Col, typename Pred>
constexpr bool any_of(const Col &col, const Pred &pred)
{
	using std::begin;
	using std::end;
	return std::any_of(begin(col), end(col), pred);
}

/**
	std::ranges::none_of(myCollection, pred);
*/
template<typename Col, typename Pred>
constexpr bool none_of(const Col &col, const Pred &pred)
{
	using std::begin;
	using std::end;
	return std::none_of(begin(col), end(col), pred);
}

/**
	std::ranges::max_element(myCollection);
*/
template<typename Col>
constexpr auto max_element(Col &&col)
{
	using std::begin;
	using std::end;
	return std::max_element(begin(col), end(col));
}

/**
	std::ranges::max_element(myCollection, [] { .. });
*/
template<typename Col, typename Pred>
constexpr auto max_element(Col &&col, const Pred &pred)
{
	using std::begin;
	using std::end;
	return std::max_element(begin(col), end(col), pred);
}

/**
	std::ranges::for_each(myCollection, [] { ... });
*/
template<typename Col, typename Pred>
constexpr void for_each(
	Col &&col,
	const Pred &pred)
{
	using std::begin;
	using std::end;
	std::for_each(begin(col), end(col), pred);
}

/**
	std::ranges::remove(myCollection, value);
*/
template<typename Col, typename T>
constexpr typename Col::iterator remove(Col &col, const T &value)
{
	using std::begin;
	using std::end;
	return std::remove(begin(col), end(col), value);
}

/**
	std::ranges::remove_if(myCollection, [] { ... });
*/
template<typename Col, typename Pred>
constexpr typename Col::iterator remove_if(Col &col, const Pred &pred)
{
	using std::begin;
	using std::end;
	return std::remove_if(begin(col), end(col), pred);
}

/**
	std::ranges::sort(myCollection);
*/
template<typename Col>
constexpr void sort(Col &col)
{
	using std::begin;
	using std::end;
	std::sort(begin(col), end(col));
}

/**
	std::ranges::sort(myCollection, [] { ... });
*/
template<typename Col, typename Pred>
constexpr void sort(Col &col, const Pred &pred)
{
	using std::begin;
	using std::end;
	std::sort(begin(col), end(col), pred);
}

/**
	std::ranges::stable_sort(myCollection);
*/
template<typename Col>
constexpr void stable_sort(Col &col)
{
	using std::begin;
	using std::end;
	std::stable_sort(begin(col), end(col));
}

/**
	std::ranges::stable_sort(myCollection, [] { ... });
*/
template<typename Col, typename Pred>
constexpr void stable_sort(Col &col, const Pred &pred)
{
	using std::begin;
	using std::end;
	std::stable_sort(begin(col), end(col), pred);
}

/**
	auto itDiff = std::set_symmetric_difference(collectionA, collectionB, diffCollection);
*/
template<typename Col1, typename Col2, typename IterResult>
constexpr IterResult set_symmetric_difference(const Col1 &col1, const Col2 &col2, _Inout_ IterResult &iterResult)
{
	using std::begin;
	using std::end;
	return std::set_symmetric_difference(begin(col1), end(col1), begin(col2), end(col2), iterResult);
}

/**
	std::ranges::transform(col, output, op);
*/
template<typename Col, typename OutputIt, typename UnaryOperation>
constexpr OutputIt transform(const Col &col, OutputIt output, UnaryOperation op)
{
	using std::begin;
	using std::end;
	return std::transform(begin(col), end(col), output, op);
}

/**
	std::ranges::distance(myCollection);
*/
template <typename Col>
constexpr auto distance(const Col& col)
{
	using std::begin;
	using std::end;
	return std::distance(begin(col), end(col));
}

#if (_LIBCPP_STD_VER >= 20 && !defined(_LIBCPP_HAS_NO_INCOMPLETE_RANGES))
template<class R>
inline constexpr bool enable_borrowed_range = false;
#endif

} // namespace std::ranges

#endif // ^^^ C++17
#endif // LIBLET_STLEXTENSIONS_PRIVATE_CPP20_H
