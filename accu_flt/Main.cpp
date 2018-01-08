#include <iostream>
#include <vector>
#include <numeric>
#include <functional>

// Get value type from container type
template <class C>
using value_t = typename C::value_type;

// Get predicate type from container type
template <class C>
using pred_t = std::function<bool(value_t<C>)>;

// Returns filtered data 
template<class C>
C filter(C const& cont, pred_t<C> test)
{
	auto r = std::accumulate(cont.cbegin(), cont.cend(), C(), [test](C &cr, value_t<C> x)->C&
	{
		if (test(x)) cr.push_back(x);
		return cr;
	});
	return r;
}

// For printing vectors 
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
	for (auto& el : vec) os << el << ' ';
	return os;
}

int main()
{
	using test_t = int;
	using testvec_t = std::vector<test_t>;

	testvec_t data{ -1, -2,-12, 1,2,3,4,5,7,8, 0 };
	std::cout << filter(data, [](test_t x) {return x%2==0;}) << "\n";
}
