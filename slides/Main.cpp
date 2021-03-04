#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

template<class C>
struct crop
{
	using I = std::conditional_t<std::is_const_v<C>, typename C::const_iterator, typename C::iterator>;
	using T = unsigned;

	crop(C & c, T i0, T iN) : c(c), s(i0), e(iN + 1) { e = std::min<T>(e, c.size()), s = std::min(s, e); }
	crop(C & c, T i0 = 0) : crop(c, i0, c.size() - 1) {}
	I begin() { return iter(s); }
	I end() { return iter(e); }
	I begin() const { return iter(s); }
	I end()   const { return iter(e); }
private:
	auto iter(T n)->I { auto i = c.begin(); std::advance(i, n); return i; }
	C& c;
	T s, e;
};

template<class C>
struct slice
{
	using VT = std::conditional_t<std::is_const_v<C>, typename C::value_type, typename C::value_type>;
	using I0 = std::conditional_t<std::is_const_v<C>, typename C::const_iterator, typename C::iterator>;
	using T = unsigned;
	struct Iterator : public std::iterator<std::forward_iterator_tag,VT, std::ptrdiff_t,I0>
	{
		Iterator() = default;
		Iterator(I0 iter, T d=1) : m_iter(iter), dist(d) {}
		VT& operator*() const { return *m_iter; }
		I0 operator->() { return m_iter; }
		Iterator& operator++() { std::advance(m_iter,dist); return *this; }
		Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
		friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_iter == b.m_iter; };
		friend bool operator!= (const Iterator& a, const Iterator& b) { return !(a == b); };

	private:
		T dist = 1;
		I0 m_iter ;
	};

	slice(C & c, T d=1) 
	{
		dist = d;
		auto end_ = T(std::distance(c.begin(),c.end()));
		end_ = (end_ / dist)*dist;
		s = Iterator(iter(c, 0));
		e = Iterator(iter(c, end_));
	}
	Iterator begin() { return s; }
	Iterator end() { return e; }
	Iterator begin() const { return s; }
	Iterator end()   const { return e; }
private:
	Iterator iter(C &c, T n) { auto i = c.begin(); std::advance(i, n); return { i,dist }; }
	T dist = 1;
	Iterator s, e;
};


template<class T>
auto makeNumberGen(T start = T(0), T inc = T(1))
{
	return [v = start, i = inc]()mutable->T {return std::exchange(v, v + i); };
}


int main()
{
	std::vector<std::string> vec = { "Hello", "from", "GCC", "version", "!","XX" };
	for (auto && x : slice(vec,3))
		x = x + "_";
	for (auto const& x : vec)
		std::cout << x << std::endl;


	std::vector<double> vd(10);
	std::generate(vd.begin(), vd.end(), makeNumberGen<double>(5, 4));
	for (auto&& x : vd)  std::cout << x << "\n";
}
