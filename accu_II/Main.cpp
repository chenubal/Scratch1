#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <functional>

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
	for (auto& el : vec)
	{
		os << el << ' ';
	}
	return os;
}
using namespace std;

int main()
{
	vector<unsigned> v = { 11,2,33,4,5 };
	cout << "max=" << accumulate(v.cbegin(), v.cend(), 0, [](unsigned a, unsigned b) {return max(a, b); }) << '\n';
	cout << "min=" << accumulate(v.cbegin(), v.cend(), 0, [](unsigned a, unsigned b) {return min(a, b); }) << '\n';

	using NT = vector<double>;
	auto cp = accumulate(v.cbegin(), v.cend(), NT(), [](NT& v, unsigned x) { v.push_back(1.1*double(x)); return v; });
	cout << cp << '\n';

	bool results[] = { true, false, true, true, false };
	cout << "all_true  = " << accumulate(cbegin(results), cend(results), true, logical_and<bool>{}) << '\n';
	cout << "some_true = " << accumulate(cbegin(results), cend(results), false, logical_or<bool>{}) << '\n';
	cout << "none_true = " << !accumulate(cbegin(results), cend(results), false, logical_or<bool>{}) << '\n';
}
