#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>

#include <functional>
#include <algorithm>

namespace lv
{
 	template <class C>
	struct container_traits
	{
		// Type value_type
		// void add_element(C&,T)
	};

	template<class C>
	struct sequence_container_traits;

	template<template<class, class> class C, class T, class A>
	struct sequence_container_traits<C<T, A>>
	{
		using value_type = T;
 		static void add_element(C<T, A>& c, const T& t)	{c.push_back(t); }
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

	/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<class C>
	struct associative_container_traits;

	template<template<class, class, class> class C, class T, template<class> class O, class A>
	struct associative_container_traits<C<T, O<T>, A>>
	{
		using value_type = T;
  		static void add_element(C<T, O<T>, A>& c, const T& t) { c.insert(t); }
	};

	template<class... Args>
	struct container_traits<std::multiset<Args...>> : public associative_container_traits<std::multiset<Args...>>
	{};

	template<class... Args>
	struct container_traits<std::set<Args...>> : public associative_container_traits<std::set<Args...>>
	{};

	template <class K ,class V>
	struct container_traits<std::map<K,V>>
	{
		using value_type = typename std::map<K, V>::value_type;// std::pair<K, V>;
		static void add_element(std::map<K, V>& c, const value_type& t) { c.insert(t); }
	};

	template<class Container>
	using FunBVT = std::function<bool(typename container_traits<Container>::value_type)>;


	template<typename Container>
	Container filter(Container const& in, FunBVT<Container>&& f)
	{
		Container out;
		for(auto x : in) if (f(x)) 	container_traits<Container>::add_element(out, x);
		return out;
	}
}
		

int main(int, char**)
{

	using ST = std::set<int>;
	ST a = { 0, 1,2,3,4,5,6,7,8,9};
	for (auto x : lv::filter(a, [](ST::value_type x) {return x % 3 == 0; })) 
		std::cout << x <<"\n";

	using MT = std::map<int, double>;
	MT b = { {0,10.0},{ 1,11.0 },{ 2,12.0 },{ 3,13.0 },{ 4,14.0 },{ 5,15.0 },{ 6,16.0 },{ 7,17.0 } };
	for (auto x : lv::filter(b, [](MT::value_type x)->bool {return x.first % 2 == 0; }))
		std::cout << x.first << "|" << x.second << "\n";

	return 0;

}

