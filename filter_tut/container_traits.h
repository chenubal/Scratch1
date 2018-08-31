#include <utility>
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

