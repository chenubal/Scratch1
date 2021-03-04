#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

template<class Iterable, class I>
auto iterator_n(Iterable &c, I n) { auto iter = c.begin(); std::advance(iter, n); return iter; }

using index_t = unsigned long;

template<class Iterable>
struct crop
{
	using iterator = typename Iterable::iterator;
	using const_iterator = const iterator;
	using value_type = typename Iterable::value_type;

	crop(Iterable & c, index_t i0, index_t iN)
	{
		s = iterator_n(c, std::min<index_t>(i0, index_t(c.size())));
		e = iterator_n(c, std::min<index_t>(iN + 1, index_t(c.size())));
	}
	crop(Iterable const& c, index_t i0, index_t iN)
	{
		s = iterator_n(c, std::min<index_t>(i0, index_t(c.size())));
		e = iterator_n(c, std::min<index_t>(iN + 1, index_t(c.size())));
	}
	crop(Iterable & c, index_t i0 = 0) : crop(c, i0, index_t(c.size()) - 1) {}
	crop(Iterable const& c, index_t i0 = 0) : crop(c, i0, index_t(c.size()) - 1) {}
	crop() = default;

	iterator begin() { return (s); }
	iterator end() { return (e); }
	const_iterator begin() const { return (s); }
	const_iterator end()   const { return (e); }
	auto size() const { return std::distance(begin(), end()); }
private:
	iterator s, e;
};

template<class Iterable>
struct slice
{
	using value_type = typename Iterable::value_type;
	struct iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = slice::value_type;
		using pointer = std::conditional_t<std::is_const<Iterable>::value, typename Iterable::const_iterator, typename Iterable::iterator>;
		using reference = value_type & ;

		iterator() = default;
		iterator(pointer p, index_t d = 1) : m_iter(p), dist(d) {}
		reference operator*() const { return *m_iter; }
		iterator& operator++() { std::advance(m_iter, dist); return *this; }
		iterator operator++(int) { return std::exchange(*this, ++(*this)); }
		friend bool operator== (const iterator& a, const iterator& b) { return a.m_iter == b.m_iter; };
		friend bool operator!= (const iterator& a, const iterator& b) { return !(a == b); };

	private:
		index_t dist = 1;
		pointer m_iter;
	};
	using const_iterator = const iterator;

	slice(Iterable& c, index_t d = 1) : dist(d)
	{
		s = { iterator_n(c, 0), dist };
		e = { iterator_n(c, (index_t(c.size()) / dist)*dist),dist };
	}
	slice(Iterable const& c, index_t d = 1) : dist(d)
	{
		s = { iterator_n(c, 0), dist };
		e = { iterator_n(c, (index_t(c.size()) / dist)*dist),dist };
	}
	slice() = default;

	iterator begin() { return s; }
	iterator end() { return e; }
	const_iterator begin() const { return s; }
	const_iterator end()   const { return e; }
	auto size() const { return std::distance(s, e); }
private:
	index_t dist = 1;
	iterator s, e;
};


template<class T>
auto makeNumberGenerator(T start = T(0), T inc = T(1))
{
	return [v = start, i = inc]()mutable->T {return std::exchange(v, v + i); };
}

int main()
{
	using V = std::vector<double>;
	V vd(20);
	std::generate(vd.begin(), vd.end(), makeNumberGenerator<double>());

	for (auto&& x : vd)  std::cout << x << " ";
	std::cout << "\n---------\n";

	for (auto&& x : crop(vd, 3, 7)) std::cout << x << " ";
	std::cout << "\n---------\n";

	for (auto&& x : slice(vd, 2)) std::cout << x << " ";
	std::cout << "\n---------\n";

	for (auto&& x : crop(slice(vd, 2), 2)) std::cout << x << " ";
	std::cout << "\n---------\n";

	for (auto&& x : slice(crop(vd, 2), 3)) std::cout << x << " ";
	for (auto&& x : slice(crop(vd, 2), 3)) x = 99;
	std::cout << "\n---------\n";

	for (auto&& x : vd)  std::cout << x << " ";
}
