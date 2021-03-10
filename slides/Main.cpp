#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

using index_t = unsigned long;

template<class Iterable>
auto iterator_n(Iterable &c, index_t n) { auto iter = c.begin(); std::advance(iter, n); return iter; }

template<class Iterable, bool is_const>
struct crop
{
	using iterator = std::conditional_t<is_const, typename Iterable::const_iterator, typename Iterable::iterator>;
	using const_iterator = const iterator;
	using value_type = typename Iterable::value_type;

	template<class S>
	crop(S & c, index_t first, index_t last)
	{
		begin_ = iterator_n(c, std::min<index_t>(first, index_t(c.size())));
		end_ = iterator_n(c, std::min<index_t>(last + 1, index_t(c.size())));
	}
	template<class S>
	crop(S&& c, index_t first, index_t last)
	{
		begin_ = iterator_n(c, std::min<index_t>(first, index_t(c.size())));
		end_ = iterator_n(c, std::min<index_t>(last + 1, index_t(c.size())));
	}
	template<class S>
	crop(S & c, index_t first = 0) : crop(c, first, index_t(c.size()) - 1) {}
	template<class S>
	crop(S&& c, index_t first = 0) : crop(c, first, index_t(c.size()) - 1) {}
	crop() = default;

	auto begin() { return begin_; }
	auto end()   { return end_; }
	auto begin() const { return begin_; }
	auto end()   const { return end_; }
	auto size() const { return std::distance(begin_, end_); }
private:
	iterator begin_, end_;
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

template<class Iterable, bool is_const>
struct slice
{
	using value_type = typename Iterable::value_type;
	struct iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = slice::value_type;
		using pointer = std::conditional_t<is_const, typename Iterable::const_iterator, typename Iterable::iterator>;
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
	slice(S& c, index_t step = 1) : step_(step)
	{
		begin_ = { iterator_n(c, 0), step_ };
		end_ = { iterator_n(c, (index_t(c.size()) / step_)*step_),step_ };
	}
	template<class S>
	slice(S&& c, index_t step = 1) : step_(step)
	{
		begin_ = { iterator_n(c, 0), step_ };
		end_ = { iterator_n(c, (index_t(c.size()) / step_)*step_),step_ };
	}
	slice() = default;

	auto begin() { return begin_; }
	auto end()   { return end_; }
	auto begin() const { return begin_; }
	auto end()   const { return end_; }
	auto size()  const { return std::distance(begin_, end_); }
private:
	index_t step_ = 1;
	iterator begin_, end_;
};

// User CTAD
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
	// Test: modifying vector with combined ops
	V  w{ 1,2,3,4,5,6,7,8,9,0 };
	for (auto && x : slice(crop(w, 2), 2)) x = 99;
	for (auto const& x : w) std::cout << x << " ";
	std::cout << "\n---------\n";
	auto sc= slice(crop(w, 3), 2);
	std::for_each(sc.begin(), sc.end(), [](auto &x) {x = 77; });
	for (auto const& x : w) std::cout << x << " ";
	std::cout << "\n---------\n";
}
