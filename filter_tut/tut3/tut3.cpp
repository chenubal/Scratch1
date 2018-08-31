#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <string>
#include <unordered_map>

#include <functional>

namespace lv
{
	//--------------------------------------------------------------------------------------------
	// Base empty container trait
	//--------------------------------------------------------------------------------------------
	template <class Container>
	struct container_traits
	{
		// Type value_type
		// void inplace_add(C&,V)
	};

	//--------------------------------------------------------------------------------------------
	// Traits for sequence container
	//--------------------------------------------------------------------------------------------
	template<class Container>
	struct sequence_container_traits;

	template<template<class> class Container, class T>
	struct sequence_container_traits<Container<T>>
	{
		using value_type = T;
		static void inplace_add(Container<T>& container, const T& v) { container.push_back(v); }
	};

	template<template<class, class> class Container, class T, class Allocator>
	struct sequence_container_traits<Container<T, Allocator>>
	{
		using value_type = T;
		static void inplace_add(Container<T, Allocator>& container, const T& v) { container.push_back(v); }
	};

	template<template<class, class, class > class Container, class T, class Traits, class Allocator>
	struct sequence_container_traits<Container<T, Traits, Allocator>>
	{
		using value_type = T;
		static void inplace_add(Container<T, Traits, Allocator>& container, const T& v) { container.push_back(v); }
	};
	//--------------------------------------------------------------------------------------------
	// Traits for associative container
	//--------------------------------------------------------------------------------------------
	template<class Container>
	struct associative_container_traits;

	template<template<class, class, class> class Container, class Key, template<class> class Compare, class Allocator>
	struct associative_container_traits<Container<Key, Compare<Key>, Allocator>>
	{
		using value_type = Key;
		static void inplace_add(Container<Key, Compare<Key>, Allocator>& container, const Key& v) { container.insert(v); }
	};

	template<template<class, class, class, class> class Container, class Key, class T, template<class> class Compare, class Allocator>
	struct associative_container_traits<Container<Key, T, Compare<Key>, Allocator>>
	{
		using value_type = std::pair<Key, T>;
		static void inplace_add(Container<Key, T, Compare<Key>, Allocator>& container, const value_type& v) { container.insert(v); }
	};

	template<template<class, class, class, class> class Container, class Key, template<class> class Hash, template<class> class KeyEqual, class Allocator>
	struct associative_container_traits<Container<Key, Hash<Key>, KeyEqual<Key>, Allocator>>
	{
		using value_type = Key;
		static void inplace_add(Container<Key, Hash<Key>, KeyEqual<Key>, Allocator>& container, const Key& v) { container.insert(v); }
	};

	template<template<class, class, class, class, class> class Container, class Key, class T, template<class> class Hash, template<class> class KeyEqual, class Allocator>
	struct associative_container_traits<Container<Key, T, Hash<Key>, KeyEqual<Key>, Allocator>>
	{
		using value_type = std::pair<Key, T>;
		static void inplace_add(Container<Key, T, Hash<Key>, KeyEqual<Key>, Allocator>& container, const value_type& v) { container.insert(v); }
	};
	//--------------------------------------------------------------------------------------------
	// Traits for common container
	//--------------------------------------------------------------------------------------------

	template<class... Args>
	struct container_traits<std::vector<Args...>> : public sequence_container_traits<std::vector<Args...>> {};

	template<class... Args>
	struct container_traits<std::list<Args...>> : public sequence_container_traits<std::list<Args...>> {};

	template<class... Args>
	struct container_traits<std::deque<Args...>> : public sequence_container_traits<std::deque<Args...>> {};

	template<class... Args>
	struct container_traits<std::basic_string<Args...>> : public sequence_container_traits<std::basic_string<Args...>> {};

	template<class... Args>
	struct container_traits<std::set<Args...>> : public associative_container_traits<std::set<Args...>> {};

	template<class... Args>
	struct container_traits<std::multiset<Args...>> : public associative_container_traits<std::multiset<Args...>> {};

	template<class... Args>
	struct container_traits<std::map<Args...>> : public associative_container_traits<std::map<Args...>> {};

	template<class... Args>
	struct container_traits<std::multimap<Args...>> : public associative_container_traits<std::multimap<Args...>> {};

	template<class... Args>
	struct container_traits<std::unordered_map<Args...>> : public associative_container_traits<std::unordered_map<Args...>> {};

	/// @brief Generic filter function
	/// @param in gives the input container to be filtered
	/// @param f gives the filter predicate of type function<bool(T)> 
	/// @return an equal conaziner with filtered values 
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
	operator int() { return y; }
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
	VT v{ 0, 1,2,3,4,5,6,7,8,9 };
	cout << "\nVector = ";
	auto even = [](VT::value_type x) {return int(x) % 2 == 0; };
	for (auto x : lv::filter(v, even))
		cout << x << " ";

	//--------------------------------------------------------------------------------------------
	ST s{ 0, 1,2,3,4,5,6,7,8,9 };
	cout << "\nSet = ";
	auto mult3 = [](ST::value_type x) {return int(x) % 3 == 0; };
	for (auto x : lv::filter(s, mult3))
		cout << x << " ";

	//--------------------------------------------------------------------------------------------
	MT m{ { 0,11.0 },{ 1,12.0 },{ 2,13.0 },{ 3,14.0 },{ 4,15.0 },{ 5,16.0 },{ 6,17.0 },{ 7,18.0 } };
	cout << "\nMap = ";
	for (auto x : lv::filter(m, [](MT::value_type x) {return int(x.second) % 2 == 0; }))
		cout << x.first << "|" << x.second << " ";

	//--------------------------------------------------------------------------------------------
	auto isValidChar = [invalid = string("_-g")](char c) {return invalid.find(c) == string::npos; };
	cout << "\nstring = " << lv::filter(string("dsf_----___das_gjfh---_gdh"), isValidChar);

	cout << "\n";
	return 0;
}

