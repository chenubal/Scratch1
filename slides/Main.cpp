#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

template<class C, class I>
auto iterator_n(C &c, I n) { auto i = c.begin(); std::advance(i, n); return i; }

template<class C>
struct crop
{
	using iterator = typename C::iterator;
	using const_iterator = const iterator;
	using value_type = typename C::value_type;
	using index_t = unsigned;

	crop(C & c, index_t i0, index_t iN) 
	{ 
		s = iterator_n(c, std::min<index_t>(i0, index_t(c.size())));
		e = iterator_n(c, std::min<index_t>(iN+1, index_t(c.size())));
	}
	crop(C & c, index_t i0 = 0) : crop(c, i0, index_t(c.size()) - 1) {}

	iterator begin() { return (s); }
	iterator end() { return (e); }
	const_iterator begin() const { return (s); }
	const_iterator end()   const { return (e); }
	auto size() const {return std::distance(begin(), end());}
private:
	iterator s, e;
};

template<class C>
struct slice
{
	using index_t = unsigned;
	using value_type = typename C::value_type;
	using container_iterator = std::conditional_t<std::is_const_v<C>, typename C::const_iterator, typename C::iterator>;
	struct iterator 
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = slice::value_type;
		using pointer = slice::container_iterator;
		using reference = value_type&;

		iterator() = default;
		iterator(pointer p, index_t d=1) : m_iter(p), dist(d) {}
		reference operator*() const { return *m_iter; }
		pointer operator->() { return m_iter; }
		iterator& operator++() { std::advance(m_iter,dist); return *this; }
		iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
		friend bool operator== (const iterator& a, const iterator& b) { return a.m_iter == b.m_iter; };
		friend bool operator!= (const iterator& a, const iterator& b) { return !(a == b); };

	private:
		index_t dist = 1;
		pointer m_iter;
	};
	using const_iterator = const iterator;

	slice(C & c, index_t d = 1): dist(d)
	{
		s = { iterator_n(c, 0), dist };
		e = { iterator_n(c, (index_t(c.size()) / dist)*dist),dist };
	}
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
auto makeNumberGen(T start = T(0), T inc = T(1))
{
	return [v = start, i = inc]()mutable->T {return std::exchange(v, v + i); };
}


int main()
{
	std::vector<double> vd(20);
	std::generate(vd.begin(), vd.end(), makeNumberGen<double>());

	for (auto&& x : crop(vd, 3, 7))  std::cout << x << "\n";
	std::cout << "---------\n";
	for (auto&& x : slice(vd, 2))  std::cout << x << "\n";
	std::cout << "---------\n";
	for (auto&& x : crop(slice(vd, 2), 2))  std::cout << x << "\n";
	std::cout << "---------\n";
	for (auto&& x : slice(crop(vd, 2), 3))  std::cout << x << "\n";
	for (auto&& x : slice(crop(vd, 2), 3)) x=99;
	std::cout << "---------\n";
	for (auto&& x : vd)  std::cout << x << "\n";
}
