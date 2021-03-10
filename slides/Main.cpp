#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

template<class C, class I>
auto iterator_n(C &c, I n) { auto iter = c.begin(); std::advance(iter, n); return iter; }

using index_t = unsigned long;

template<class C, bool is_const>
struct crop
{
	using iterator = std::conditional_t<is_const, typename C::const_iterator, typename C::iterator>;
	using const_iterator = const iterator;
	using value_type = typename C::value_type;
	iterator s, e;

	template<class S>
	crop(S & c, index_t i0, index_t iN)
	{
		s = iterator_n(c, std::min<index_t>(i0, index_t(c.size())));
		e = iterator_n(c, std::min<index_t>(iN + 1, index_t(c.size())));
	}
	template<class S>
	crop(S&& c, index_t i0, index_t iN)
	{
		s = iterator_n(c, std::min<index_t>(i0, index_t(c.size())));
		e = iterator_n(c, std::min<index_t>(iN + 1, index_t(c.size())));
	}
	template<class S>
	crop(S & c, index_t i0 = 0) : crop(c, i0, index_t(c.size()) - 1) {}
	template<class S>
	crop(S&& c, index_t i0 = 0) : crop(c, i0, index_t(c.size()) - 1) {}
	crop() = default;

	auto begin() { return s; }
	auto end() { return e; }
	auto begin() const { return s; }
	auto end()   const { return e; }
	auto size() const { return std::distance(s, e); }
};

// User CTAD
template <typename S>
crop(S&, index_t, index_t)->crop<S, std::is_const_v<std::remove_reference_t<S>>>;
template <typename S>
crop(S&, index_t)->crop<S, std::is_const_v<std::remove_reference_t<S>>>;
template <typename S>
crop(S&)->crop<S, std::is_const_v<std::remove_reference_t<S>>>;
template <typename S>
crop(S&&, index_t, index_t)->crop<S, std::is_const_v<std::remove_reference_t<S>>>;
template <typename S>
crop(S&&, index_t)->crop<S, std::is_const_v<std::remove_reference_t<S>>>;
template <typename S>
crop(S&&)->crop<S, std::is_const_v<std::remove_reference_t<S>>>;

template<class C, bool is_const>
struct slice
{
	using value_type = typename C::value_type;
	struct iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = slice::value_type;
		using pointer = std::conditional_t<is_const, typename C::const_iterator, typename C::iterator>;
		using reference = value_type & ;

		iterator() = default;
		iterator(pointer p, index_t d = 1) : m_iter(p), dist(d) {}
		auto& operator*() const { return *m_iter; }
		auto& operator++() { std::advance(m_iter, dist); return *this; }
		auto operator++(int) { return std::exchange(*this, ++(*this)); }
		friend bool operator== (const iterator& a, const iterator& b) { return a.m_iter == b.m_iter; };
		friend bool operator!= (const iterator& a, const iterator& b) { return !(a == b); };

	private:
		index_t dist = 1;
		pointer m_iter;
	};
	using const_iterator = const iterator;

	template<class S>
	slice(S& c, index_t d = 1) : dist(d)
	{
		s = { iterator_n(c, 0), dist };
		e = { iterator_n(c, (index_t(c.size()) / dist)*dist),dist };
	}
	template<class S>
	slice(S&& c, index_t d = 1) : dist(d)
	{
		s = { iterator_n(c, 0), dist };
		e = { iterator_n(c, (index_t(c.size()) / dist)*dist),dist };
	}
	slice() = default;

	auto begin() { return s; }
	auto end() { return e; }
	auto begin() const { return s; }
	auto end()   const { return e; }
	auto size() const { return std::distance(s, e); }
private:
	index_t dist = 1;
	iterator s, e;
};

template <typename S>
slice(S&, index_t)->slice<S, std::is_const_v<std::remove_reference_t<S>>>;
template <typename S>
slice(S&)->slice<S, std::is_const_v<std::remove_reference_t<S>>>;
template <typename S>
slice(S&&, index_t)->slice<S, std::is_const_v<std::remove_reference_t<S>>>;
template <typename S>
slice(S&&)->slice<S, std::is_const_v<std::remove_reference_t<S>>>;

int main()
{
	using V = std::vector<double>;
	// Tests on const vector: single and combined ops
	const V  v{ 1,2,3,4,5,6,7,8,9,0 };
	for (auto && x : slice(crop(v, 2), 2)) std::cout << x << " ";
	std::cout << "\n---------\n";
	for (auto && x : slice(v, 2)) std::cout << x << " ";
	std::cout << "\n---------\n";
	for (auto && x : crop(v, 3, 6)) std::cout << x << " ";
	std::cout << "\n---------\n";
	for (auto && x : crop(slice(v, 2), 2)) std::cout << x << " ";
	std::cout << "\n---------\n";
	// Test: modifying vector: nd combined ops
	V  w{ 1,2,3,4,5,6,7,8,9,0 };
	for (auto && x : slice(crop(w, 2), 2)) x = 99;
	for (auto const& x : w) std::cout << x << " ";
	std::cout << "\n---------\n";
}
