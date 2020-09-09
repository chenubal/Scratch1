#include <algorithm>
#include <iterator>
#include <iostream>
#include <vector>
#include <numeric>

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
	for (auto& el : vec) os << el << '\n';
	return os;
}

// Prints the primes up to N
int main()
{
	const auto N = 1000;
	std::vector<unsigned> v(N-1);
	std::iota(v.begin(), v.end(), 2); // create numbers 2...N

	for (auto iter = v.begin(); iter != v.end(); ++iter)
	{
		auto isMultiple = [r = *iter](auto v) {return v % r == 0; };
		v.erase(std::remove_if(std::next(iter), v.end(), isMultiple), v.end());
	}

	std::cout << v << "\n";
}