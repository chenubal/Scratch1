// userdefinedTypes.cpp

#include <iostream>
#include <ostream>
#include <vector>
#include <algorithm>
#include <numeric>


template < class V, class U>
std::vector<V> convert(std::vector<U> const &u)
{
	std::vector<V> conv; conv.reserve(u.size());
	return std::accumulate(u.cbegin(), u.cend(), conv, [](std::vector<V>& s, U const& x) { s.emplace_back(V(x)); return s; });
}



int main()
{
	using T = std::vector<float>;
	std::vector<double> vi = { 1.1,2.2,5.5 };
	for (auto x : vi) std::cout << x << std::endl;
	for (auto x : convert<int>(vi)) std::cout << x << std::endl;
	std::cout << std::endl;
	return 0;

}