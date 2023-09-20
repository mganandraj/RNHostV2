/**
	Wrappers for std algorithm library operations:
	- take care of std::begin, std:end (note: this works for fixed arrays)
	- overloads for array,length syntax
	
	**Note: It is undefined behavior to add declarations or definitions to namespace std with few exceptions.
	The following are NOT such exceptions, but are declared here anyways in order to not break existing usage.
	New functions should be added under the 'Mso' namespace and eventually the existing functions should be moved there as well.
	*/
#pragma once
#ifndef LIBLET_STLEXTENSIONS_ALGORITHMS_H
#define LIBLET_STLEXTENSIONS_ALGORITHMS_H

#include <sal.h>
#include <algorithm>
#include <map>
#include <array>
#include <tuple>
#include <vector>
#include <deque>
#include <list>
#include <forward_list>
#include <unordered_map>
#include <unordered_set>
#include <set>

#include <stlextensions/private/cpp20.h>

namespace Mso {

/*------------------------------------------------------------------------------
	PLookup(collection, pred);

	Returns a pointer to the first element that satisfies the predicate (for which
	pred(*p) evaluates to true) or nullptr if no such element exists in rg
------------------------------------------------------------------------------*/
template <class Col, class Pred>
constexpr typename std::remove_reference_t<Col>::value_type* PLookup(
	/*const*/ Col&& col, // col should be const, since pred must not modify it; however leave non-const to not break existing users
	const Pred& pred) noexcept
{
	using std::begin;
	using std::end;
	auto it = std::find_if(begin(col), end(col), pred);
	if (it == end(col))
		return nullptr;
	else
		return &*it;
}

/*------------------------------------------------------------------------------
	PLookup(collection, comp);

	Returns a pointer to the first element equivalent to val (such that
	comp(val, *p) and comp(*p, val) are both false) or nullptr is no such
	element exists in rg
------------------------------------------------------------------------------*/
template <class Col, class TElem, class TComp>
typename std::remove_reference_t<Col>::value_type* PLookupSort(Col&& rg, const TElem& val, const TComp& comp) noexcept
{
	using std::begin;
	using std::end;
	auto it = std::lower_bound(begin(rg), end(rg), val, comp); // *it >= val
	if (it == end(rg) || comp(val, *it))
		return nullptr;
	return &*it;
}

/**
	Find first element in `[first, last)` that is equal to value.

	Same logic as `std::find`, but returns a value type instead of an iterator.
	If found, return resulting value. Otherwise return a default value `def`.

		vector v = { 1, 2, 3, 4 }
		find_value(v.begin(), v.end(), 2, -1) == 2
		find_value(v.begin(), v.end(), 5, -1) == -1

	If no default is provided, use the default constructor of type `T`.

		find_value(v.begin(), v.end(), 8) == 0
*/
template <typename InputIterator,
	typename T = typename std::iterator_traits<InputIterator>::reference>
	constexpr T find_value(InputIterator&& first, InputIterator&& last, const T& value, const T& def = {}) noexcept
{
	auto result = std::find(std::forward<InputIterator>(first), std::forward<InputIterator>(last), value);
	return result == last ? def : *result;
}

/**
	Same as `find_value`, but searches entire collection `col`.

		vector v = { 1, 2, 3, 4 }
		find_value(v, 2, -1) == 2
		find_value(v, 5, -1) == -1
*/
template <typename Col,
	typename T = typename std::remove_reference_t<Col>::value_type>
	constexpr auto find_value(Col&& col, const T& value, const T& def = {}) noexcept
{
	using std::begin;
	using std::end;
	return find_value(begin(col), end(col), value, def);
}

/**
	Find first element in `[first, last)` that that satisfies unary predicate `pred`.

	Same logic as `std::find_if`, but returns a value type instead of an iterator.
	If found, return resulting value. Otherwise return a default value `def`.

		vector v = { 1, 2, 3, 4 }
		find_value_if(v.begin(), v.end(), [](int x) { return x > 2; }, -1) == 3
		find_value_if(v.begin(), v.end(), [](int x) { return x > 8; }, -1) == -1

	If no default is provided, use the default constructor.

		find_value_if(v.begin(), v.end(), [](int x) { return x > 8; }) == 0
*/
template <typename InputIterator, typename Pred,
	typename T = typename std::iterator_traits<InputIterator>::reference>
	constexpr T find_value_if(InputIterator&& first, InputIterator&& last, const Pred& pred, const T& def = {}) noexcept
{
	auto result = std::find_if(std::forward<InputIterator>(first), std::forward<InputIterator>(last), pred);
	return result == last ? def : *result;
}

/**
	Same as `find_value_if`, but searches entire collection `col`.

		vector v = { 1, 2, 3, 4 }
		find_value_if(v, [](int x) { return x > 2; }, -1) == 3
		find_value_if(v, [](int x) { return x > 8; }, -1) == -1
*/
template <typename Col, typename Pred,
	typename T = typename std::remove_reference_t<Col>::value_type>
	constexpr auto find_value_if(Col&& col, const Pred& pred, const T& def = {}) noexcept
{
	using std::begin;
	using std::end;
	return find_value_if(begin(col), end(col), pred, def);
}

/**
	std::find_only_of(myCollection, [] { return ... });
	Returns the only matching item in the collection or nothing
*/
template<typename Col, typename Pred>
constexpr auto find_only_of(Col&& col, const Pred& pred) noexcept
{
	using std::end;
	const auto it = std::ranges::find_if(col, pred);
	if (it != end(col))
	{
		if (std::none_of(std::next(it), end(col), pred))
			return it;
	}
	return end(col);
}

/**
	std::contains(myCollection, t);
*/
template<typename Col, typename T>
constexpr bool contains(const Col& col, const T& t) noexcept
{
	using std::begin;
	using std::end;
	#if _HAS_CXX20 || _LIBCPP_STD_VER >= 20 // C++20 vvv
	return std::find(begin(col), end(col), t) != end(col);
	#else // ^^^ C++20 / C++17 vvv
	return std::constexpr_::find(begin(col), end(col), t) != end(col);
	#endif // ^^^ C++17
}

/**
std::contains(rgCollection, cCollection, t);
*/
template<typename Col, typename T>
constexpr bool contains(_In_count_(count) const Col& col, size_t count, const T& t) noexcept
{
	return std::find(col, col + count, t) != (col + count);
}

/**
	std::contains(myCollection, t);
*/
template<typename Col, typename Pred>
constexpr bool contains_if(const Col& col, const Pred& pred) noexcept
{
	using std::begin;
	using std::end;
	return std::find_if(begin(col), end(col), pred) != std::end(col);
}

/**
std::contains(rgCollection, cCollection, t);
*/
template<typename Col, typename Pred>
constexpr bool contains_if(_In_count_(count) const Col& col, size_t count, const Pred& pred) noexcept
{
	return std::find_if(col, col + count, pred) != (col + count);
}

/**
	Run `remove` on a collection `col` and erase all removed elements.

		vector v = { 1, 2, 1, 3, 1 }
		erase_remove(v, 1);
		v == {2, 3}

	Returns iterator to position following last element erased from `col`.
*/
template <typename Col, typename T>
constexpr void erase_remove(Col& col, const T& value) noexcept
{
	using std::begin;
	using std::end;
	col.erase(
		std::remove(begin(col), end(col), value),
		end(col));
}

/**
	Run `remove_if` on a collection and erase all removed elements.

		vector v = { 1, 2, 3, 4, 5}
		erase_if(v, [](int x) { return x % 2; });
		v == {2, 4}

	Returns iterator to position following last element erased from `col`.
*/
template <typename Col, typename Pred>
constexpr void erase_if(Col& col, const Pred& predicate) noexcept
{
	using std::begin;
	using std::end;
	col.erase(
		std::remove_if(begin(col), end(col), predicate),
		end(col));
}

/**
	Concatenate two std::arrays with the same element type.
*/
template <typename T, std::size_t N, std::size_t M>
constexpr std::array<T, N + M> array_cat( const std::array<T, N>& lhs, const std::array<T, M>& rhs) noexcept
{
	auto tuple = std::tuple_cat(lhs, rhs);
	return std::apply([](auto&&... args) noexcept { return std::array<T, N + M>{ { args... } }; }, tuple);
}

/* erase and erase_if take a container and an element and remove that element from the container.
	For map-type containers with (key,value) pairs, the specified element is treated as the key. */
template<typename Value, typename Allocator, typename Element>
void erase(std::vector<Value, Allocator>& c, const Element& e) noexcept
{
	erase_remove(c, e);
}

template<typename Value, typename Allocator, typename Element>
void erase(std::deque<Value, Allocator>& c, const Element& e) noexcept
{
	erase_remove(c, e);
}

template<typename Value, typename Allocator, typename Element>
void erase(std::list<Value, Allocator>& c, const Element& e) noexcept
{
	c.remove(e);
}

template<typename Value, typename Allocator, typename Element>
void erase(std::forward_list<Value, Allocator>& c, const Element& e) noexcept
{
	c.remove(e);
}

template<typename Key, typename Value, typename Hash, typename Compare, typename Allocator, typename Element>
void erase(std::unordered_map<Key, Value, Hash, Compare, Allocator>& c, const Element& e) noexcept
{
	c.erase(e);
}

template<typename Key, typename Value, typename Hash, typename Compare, typename Allocator, typename Element>
void erase(std::unordered_multimap<Key, Value, Hash, Compare, Allocator>& c, const Element& e) noexcept
{
	c.erase(e);
}

template<typename Key, typename Hash, typename Compare, typename Allocator, typename Element>
void erase(std::unordered_set<Key, Hash, Compare, Allocator>& c, const Element& e) noexcept
{
	c.erase(e);
}

template<typename Key, typename Hash, typename Compare, typename Allocator, typename Element>
void erase(std::unordered_multiset<Key, Hash, Compare, Allocator>& c, const Element& e) noexcept
{
	c.erase(e);
}

template<typename Key, typename Value, typename Allocator, typename Element>
void erase(std::map<Key, Value, Allocator>& c, const Element& e) noexcept
{
	c.erase(e);
}

template<typename Key, typename Value, typename Allocator, typename Element>
void erase(std::multimap<Key, Value, Allocator>& c, const Element& e) noexcept
{
	c.erase(e);
}

template<typename Key, typename Value, typename Allocator, typename Element>
void erase(std::set<Key, Value, Allocator>& c, const Element& e) noexcept
{
	c.erase(e);
}

template<typename Key, typename Value, typename Allocator, typename Element>
void erase(std::multiset<Key, Value, Allocator>& c, const Element& e) noexcept
{
	c.erase(e);
}

template<typename Container, typename Predicate>
void erase_if_helper(Container& c, Predicate p) noexcept
{
	for (auto it = c.begin(), itEnd = c.end(); it != itEnd;)
	{
		if (p(*it))
			it = c.erase(it);
		else
			++it;
	}
}

template<typename Container, typename Predicate>
void erase_if_map_helper(Container& c, Predicate p) noexcept
{
	for (auto it = c.begin(); it != c.end();)
	{
		if (p((*it).first))
			it = c.erase(it);
		else
			++it;
	}
}

template<typename Value, typename Allocator, typename Predicate>
void erase_if(std::list<Value, Allocator>& c, Predicate p) noexcept
{
	c.remove_if(p);
}

template<typename Value, typename Allocator, typename Predicate>
void erase_if(std::forward_list<Value, Allocator>& c, Predicate p) noexcept
{
	c.remove_if(p);
}

template<typename Key, typename Value, typename Hash, typename Compare, typename Allocator, typename Predicate>
void erase_if(std::unordered_map<Key, Value, Hash, Compare, Allocator>& c, Predicate p) noexcept
{
	erase_if_map_helper(c, p);
}

template<typename Key, typename Value, typename Hash, typename Compare, typename Allocator, typename Predicate>
void erase_if(std::unordered_multimap<Key, Value, Hash, Compare, Allocator>& c, Predicate p) noexcept
{
	erase_if_map_helper(c, p);
}

template<typename Key, typename Hash, typename Compare, typename Allocator, typename Predicate>
void erase_if(std::unordered_set<Key, Hash, Compare, Allocator>& c, Predicate p) noexcept
{
	erase_if_helper(c, p);
}

template<typename Key, typename Hash, typename Compare, typename Allocator, typename Predicate>
void erase_if(std::unordered_multiset<Key, Hash, Compare, Allocator>& c, Predicate p) noexcept
{
	erase_if_helper(c, p);
}

template<typename Key, typename Value, typename Allocator, typename Predicate>
void erase_if(std::map<Key, Value, Allocator>& c, Predicate p) noexcept
{
	erase_if_map_helper(c, p);
}

template<typename Key, typename Value, typename Allocator, typename Predicate>
void erase_if(std::multimap<Key, Value, Allocator>& c, Predicate p) noexcept
{
	erase_if_map_helper(c, p);
}

template<typename Key, typename Value, typename Allocator, typename Predicate>
void erase_if(std::set<Key, Value, Allocator>& c, Predicate p) noexcept
{
	erase_if_helper(c, p);
}

template<typename Key, typename Value, typename Allocator, typename Predicate>
void erase_if(std::multiset<Key, Value, Allocator>& c, Predicate p) noexcept
{
	erase_if_helper(c, p);
}

template<class Container, class Type> inline
typename Container::iterator upper_bound(Container& container, const Type& val) noexcept
{
	return std::upper_bound(std::begin(container), std::end(container), val);
}

template<class Container, class Type, class BinaryPredicate> inline
typename Container::iterator upper_bound(Container& container, const Type& val, BinaryPredicate pred) noexcept
{
	return std::upper_bound(std::begin(container), std::end(container), val, pred);
}

} // namespace Mso


#endif // LIBLET_STLEXTENSIONS_ALGORITHMS_H
