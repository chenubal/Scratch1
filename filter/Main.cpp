#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <unordered_map>

#include <functional>

namespace jh
{
	template <class C>
	struct container_traits
	{
		// Type value_type
		// void inplace_add(C&,V)
	};

	template<class C>
	struct sequence_container_traits;

	template<template<class, class> class C, class V, class A>
	struct sequence_container_traits<C<V, A>>
	{
		using value_type = V;
		static void inplace_add(C<V, A>& c, const V& t) { c.push_back(t); }
	};


	template<class C>
	struct associative_container_traits;

	template<template<class, class, class> class C, class V, template<class> class L, class A>
	struct associative_container_traits<C<V, L<V>, A>>
	{
		using value_type = V;
		static void inplace_add(C<V, L<V>, A>& c, const V& t) { c.insert(t); }
	};

	template<template<class, class, class, class> class C, class K, class V, template<class> class L, class A>
	struct associative_container_traits<C<K, V, L<K>, A>>
	{
		using value_type = std::pair<K, V>;
		static void inplace_add(C<K, V, L<K>, A>& c, const value_type& t) { c.insert(t); }
	};

	template<template<class, class, class, class, class> class C, class K, class V, template<class> class H, template<class> class E, class A>
	struct associative_container_traits<C<K, V, H<K>, E<K>, A>>
	{
		using value_type = std::pair<K, V>;
		static void inplace_add(C<K, V, H<K>, E<K>, A>& c, const value_type& t) { c.insert(t); }
	};


	template<class... Args>
	struct container_traits<std::deque<Args...>> : public sequence_container_traits<std::deque<Args...>>
	{};

	template<class... Args>
	struct container_traits<std::list<Args...>> : public sequence_container_traits<std::list<Args...>>
	{};

	template<class... Args>
	struct container_traits<std::vector<Args...>> : public sequence_container_traits<std::vector<Args...>>
	{};

	template<class... Args>
	struct container_traits<std::multiset<Args...>> : public associative_container_traits<std::multiset<Args...>>
	{};

	template<class... Args>
	struct container_traits<std::set<Args...>> : public associative_container_traits<std::set<Args...>>
	{};

	template<class... Args>
	struct container_traits<std::map<Args...>> : public associative_container_traits<std::map<Args...>>
	{};

	template<class... Args>
	struct container_traits<std::multimap<Args...>> : public associative_container_traits<std::multimap<Args...>>
	{};

	template<class... Args>
	struct container_traits<std::unordered_map<Args...>> : public associative_container_traits<std::unordered_map<Args...>>
	{};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<class Container>
	using FunBVT = std::function<bool(typename container_traits<Container>::value_type)>;

	template<typename Container>
	Container filter(Container const& in, FunBVT<Container>&& f)
	{
		Container out;
		for (auto x : in)
			if (f(x))
				container_traits<Container>::inplace_add(out, x);
		return out;
	}
}

#include <iostream>

struct MyStruct
{
	MyStruct(double x) : x(x), y(int(2 * x)) {}
	double x;
	int y;
	operator int() { return int(x); }
};

std::ostream& operator<<(std::ostream& os, const MyStruct& s)
{
	os << s.x << '-' << s.y;
	return os;
}

using VT = std::vector<MyStruct>;
using ST = std::set<int>;
using MT = std::unordered_map<int, double>;

int main(int, char**)
{
	VT v = { 0, 1,2,3,4,5,6,7,8,9 };
	std::cout << "\nv=";
	for (auto x : jh::filter(v, [](VT::value_type x) {return int(x) % 2 == 0; }))
		std::cout << x << " ";

	ST s = { 0, 1,2,3,4,5,6,7,8,9 };
	std::cout << "\ns=";
	for (auto x : jh::filter(s, [](ST::value_type x) {return int(x) % 3 == 0; }))
		std::cout << x << " ";

	MT m = { { 0,11.0 },{ 1,12.0 },{ 2,13.0 },{ 3,14.0 },{ 4,15.0 },{ 5,16.0 },{ 6,17.0 },{ 7,18.0 } };
	std::cout << "\nm=";
	for (auto x : jh::filter(m, [](MT::value_type x) {return int(x.second) % 2 == 0; }))
		std::cout << x.first << "|" << x.second << " ";

	std::cout << "\n";
	return 0;

}

