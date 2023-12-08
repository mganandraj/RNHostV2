#pragma once

#include "algorithms.h"

#include <array>
#include <type_traits>
#include <vector>

//! Provides a view on an array of data.
//!
//! \remarks This facility is thread-safe.
//! \remarks All data is read-only.
template <typename T>
class array_view
{
public:
	using element_type = T;
	using value_type = std::remove_cv_t<T>;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;
	using iterator = const_pointer;

	/// Constructs an empty view whose data() == nullptr and size() == 0.
	constexpr array_view() noexcept = default;

	/// Constructs a view over the range [p, p + count); the resulting view has data() == std::to_address(first) and size() == c.
	constexpr array_view(const_pointer p, size_type c) noexcept
		: m_p(p)
		, m_c(c)
	{
	}

	constexpr array_view(const std::vector<value_type>& other) noexcept
		: array_view{other.data(), other.size()}
	{
	}

	template <class T2, size_type N>
	explicit constexpr array_view(const std::array<T2, N>& array) noexcept
		: m_p{ array.data() }
		, m_c{ array.size() }
	{
	}

	template <class T2, size_type N>
	constexpr array_view(T2 (&array)[N]) noexcept
		: m_p{ array }
		, m_c{ N }
	{
	}

	constexpr const_pointer data() const noexcept { return m_p; }
	constexpr size_type size() const noexcept { return m_c; }
	constexpr bool empty() const noexcept { return size() == 0; }
	constexpr iterator begin() const noexcept { return data(); }
	constexpr iterator end() const noexcept { return data() + size(); }

	/// Returns a reference to the idx-th element of the sequence. 
	/// The behavior is undefined if idx is out of range (i.e., if it is greater than or equal to size()).
	constexpr const_reference operator[](std::size_t idx) const noexcept
	{
#if _DEBUG
		// no access to Assert liblet
		if(idx >= m_c)
			std::terminate();
#endif
		return m_p[idx];
	}

	/// array views are considered equal if they have the same size and each corresponding element from the views compare equal 
	constexpr bool operator==(const array_view& other) const noexcept
	{
		return std::ranges::equal(*this, other);
	}

private:
	const_pointer m_p{ nullptr };
	size_type m_c{ 0 };
};

template<class T2, std::size_t N>
array_view(T2 (&)[N]) -> array_view<T2>;

template <typename T>
array_view(T) -> array_view<typename T::value_type>;