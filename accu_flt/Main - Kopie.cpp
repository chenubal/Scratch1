#include <iostream>
#include <vector>
#include <numeric>
#include <functional>
#include <iterator>
#include <algorithm>
  
// Get value type from container type
template <class C>
using value_t = typename C::value_type;

// Get simple predicate type from container type
template <class C>
using pred_t = std::function<bool(value_t<C>)>;
   
// Returns filtered data 
template<class C>
C filter_accu(C const& cont, pred_t<C> test)
{
	auto r = std::accumulate(cont.cbegin(), cont.cend(), C(), [test](C &cr, value_t<C> x)->C&
	{
		if (test(x)) cr.push_back(x);
		return cr;
	});
	return r;
}

// Get simple predicate type from container type
template <class C>
using pred2_t = std::function<bool(C&, value_t<C>)>;

// Returns filtered data 
template<class C>
C filter_accu(C const& cont, pred2_t<C> test)
{
	auto r = std::accumulate(cont.cbegin(), cont.cend(), C(), [test](C &cr, value_t<C> x)->C&
	{
		if (test(cr, x)) cr.push_back(x);
		return cr;
	});
	return r;
}

// Returns filtered data 
template<class C>
C filter_copy(C const& cont, pred_t<C> test)
{
	C r;
	std::copy_if(cont.cbegin(), cont.cend(), std::back_inserter(r), test);
	return r;
}

// For printing vectors 
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
	for (auto& el : vec) os << el << ' ';
	return os;
}

template<class T>
bool contains(std::vector<T> const& v, T x) { return std::find(v.cbegin(), v.cend(), x) != v.cend(); }

int main()
{
	using R = int;
	using RV = std::vector<R>;

	RV data{ -1, -2,-12, 1,2,3,4,5,7,8, 0,8,8,8 };
	std::cout << filter_accu(data, [](R x) {return x % 2 == 0; }) << "\n";
	std::cout << filter_copy(data, [](R x) {return x % 2 == 0; }) << "\n";
	std::cout << filter_accu<RV>(data, [](RV &cr, R x)->bool {return (x % 2 == 0) && !contains(cr, x); }) << "\n";
}
