#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <string>
#include <unordered_map>

#include <functional>

namespace jh
{
	//--------------------------------------------------------------------------------------------
	template <class C>
	struct container_traits
	{
		// Type value_type
		// void inplace_add(C&,V)
	};

	//--------------------------------------------------------------------------------------------
	template<class C>
	struct sequence_container_traits;

	template<template<class, class> class C, class V, class A>
	struct sequence_container_traits<C<V, A>>
	{
		using value_type = V;
		static void inplace_add(C<V, A>& c, const V& t) { c.push_back(t); }
	};

	//--------------------------------------------------------------------------------------------
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

	//--------------------------------------------------------------------------------------------
	template<class C>
	struct string_container_traits;

	template<template<class, class, class> class C, class T, template<class> class K, class A>
	struct string_container_traits<C<T, K<T>, A>>
	{
		using value_type = T;
		static void inplace_add(C<T, K<T>, A>& c, const T& t) {	c.push_back(t);}
	};

	//--------------------------------------------------------------------------------------------
	template<class... Args>
	struct container_traits<std::deque<Args...>> : public sequence_container_traits<std::deque<Args...>> {};

	template<class... Args>
	struct container_traits<std::vector<Args...>> : public sequence_container_traits<std::vector<Args...>> {};

	template<class... Args>
	struct container_traits<std::list<Args...>> : public sequence_container_traits<std::list<Args...>> {};

	template<class... Args>
	struct container_traits<std::set<Args...>> : public associative_container_traits<std::set<Args...>>	{};

	template<class... Args>
	struct container_traits<std::multiset<Args...>> : public associative_container_traits<std::multiset<Args...>> {};

	template<class... Args>
	struct container_traits<std::map<Args...>> : public associative_container_traits<std::map<Args...>>	{};

	template<class... Args>
	struct container_traits<std::multimap<Args...>> : public associative_container_traits<std::multimap<Args...>> {};

	template<class... Args>
	struct container_traits<std::unordered_map<Args...>> : public associative_container_traits<std::unordered_map<Args...>>	{};

	template<class... Args>
	struct container_traits<std::basic_string<Args...>> : public string_container_traits<std::basic_string<Args...>> {};

	//--------------------------------------------------------------------------------------------
 	template<typename Container>
	Container filter(Container const& in, std::function<bool(typename container_traits<Container>::value_type)>&& f)
	{
		Container out;
		auto add = &container_traits<Container>::inplace_add;
		for (auto const& x : in) 
			if (f(x))	
				add(out, x);
		return out;	   // RVO bzw &&-C'tor
	}
}

//--------------------------------------------------------------------------------------------
//---------- Tests ---------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

#include <iostream>

using namespace std;


struct TestStruct
{
	TestStruct(double x) : x(x), y(int(2 * x)) {}
	double x;
	int y;
	operator int() { return int(x); }
};

ostream& operator<<(ostream& os, const TestStruct& s)
{
	os << s.x << '-' << s.y;
	return os;
}

using VT = vector<TestStruct>;
using ST = set<int>;
using MT = unordered_map<int, double>;

int main(int, char**)
{
	//--------------------------------------------------------------------------------------------
	VT v = { 0, 1,2,3,4,5,6,7,8,9 };
	cout << "\nVector = ";
	auto predicate = [](VT::value_type x) {return int(x) % 2 == 0; };
	for (auto x : jh::filter(v, predicate))
		cout << x << " ";

	//--------------------------------------------------------------------------------------------
	ST s = { 0, 1,2,3,4,5,6,7,8,9 };
	cout << "\nSet = ";
	for (auto x : jh::filter(s, [](ST::value_type x) {return int(x) % 3 == 0; }))
		cout << x << " ";

	//--------------------------------------------------------------------------------------------
	MT m = { { 0,11.0 },{ 1,12.0 },{ 2,13.0 },{ 3,14.0 },{ 4,15.0 },{ 5,16.0 },{ 6,17.0 },{ 7,18.0 } };
	cout << "\nMap = ";
	for (auto x : jh::filter(m, [](MT::value_type x) {return int(x.second) % 2 == 0; }))
		cout << x.first << "|" << x.second << " ";

	//--------------------------------------------------------------------------------------------
	cout << "\nstring = " << jh::filter(string("dsf_----___das_gjfh---_gdh"), [r=string("_-g")](char c) {return r.find(c)== string::npos; });

	cout << "\n";
	return 0;
}

